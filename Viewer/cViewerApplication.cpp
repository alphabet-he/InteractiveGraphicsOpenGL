#include "cViewerApplication.h"
#include <JSON/json.hpp>
#include <fstream>

cViewerApplication::cViewerApplication()
{
    m_windowWidth = 800;
    m_windowHeight = 600;
    m_windowTitle = "Viewer";

    m_meshSystem = new cMeshSystem();
}

void cViewerApplication::CustomInitialization()
{


    // set up display program with deferred shading
    {
        m_displayProgram = new cVertexShaderProgram(new sVertexBufferStruct(true, true, true, false));
        m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0));
        m_viewMatWhenPressed = m_viewMat;
        m_projectionMat = glm::perspective(
            glm::radians(45.0f),    // Field of View
            (float)m_windowWidth / (float)m_windowHeight, // Aspect Ratio
            0.1f, 100.0f  // Near & Far plane
        );

        m_displayProgram->LinkShaders("../Assets/shader/ScreenQuadVertexShader.glsl", "../Assets/shader/ScreenQuadMultiLightsShadowFragShader.glsl");

        m_displayProgram->SetMVPMatrix(m_projectionMat, PROJECTION);
        m_displayProgram->InitializeShadowMap(2048, 2048);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    // initialize g buffer
    m_displayProgram->InitializeGBuffer(m_windowWidth, m_windowHeight);

    // parse json and load scene
	{
        using json = nlohmann::json;
        json sceneJson;

        try {
            std::ifstream file("../Export/scene.json");
            if (!file) {
                std::cerr << "Failed to open scene.json\n";
                exit(1);
            }

            file >> sceneJson; 
            file.close();

            std::cout << "Scene loaded successfully!\n";
        }
        catch (const json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << '\n';
            exit(1);
        }
        catch (const std::exception& e) {
            std::cerr << "Other error: " << e.what() << '\n';
            exit(1);
        }


        for (const auto& i_meshJson : sceneJson["meshes"]) {

            std::vector<float> i_flatModelMat = i_meshJson["model_mat"].get<std::vector<float>>();
            glm::mat4 i_modelMat = glm::make_mat4(i_flatModelMat.data());

            if (i_meshJson["category"] == "light") {
                glm::vec3 i_lightPos = glm::vec3(i_modelMat[3]);
                glm::vec3 i_lightColor = glm::vec3(i_meshJson["color"][0].get<float>(), 
                    i_meshJson["color"][1].get<float>(),
                    i_meshJson["color"][2].get<float>());
                m_lights.push_back(new sLight(i_lightPos, i_lightColor));
            }
            else {
                // create mesh
                std::string i_path = i_meshJson["file"];
                sMeshInstance* i_instance = m_meshSystem->RegisterMesh(i_path.c_str());
                i_instance->GetMesh().lock()->SetModelMat(i_modelMat);
                m_displayProgram->UploadMesh(i_instance->GetMesh(), new sTextureUsage(false, false, false));

                // attach textures
                json texturesJson = i_meshJson["textures"];

                for (auto& [flagStr, pathStr] : texturesJson.items()) {
                    int usageFlag = std::stoi(flagStr);  // convert key from string to int
                    std::string path = pathStr.get<std::string>();

                    // Only add if the path is not empty
                    if (!path.empty()) {
                        i_instance->GetMesh().lock()->UploadPNGTexture(static_cast<eTextureUsageFlags>(usageFlag), path);
                    }
                }

                if (i_meshJson["category"] == "player") {
                    m_playerMesh = i_instance->GetMesh();
                }
            }
        }
	}

}

void cViewerApplication::MainLoopFunc()
{
    // input
    {
        // camera
        if (m_input_middleMouseButton) {
            double i_mousePos_x = INT_MIN;
            double i_mousePos_y = INT_MIN;
            glfwGetCursorPos(m_applicationWindow, &i_mousePos_x, &i_mousePos_y);
            if (i_mousePos_x != INT_MIN && i_mousePos_y != INT_MIN) {

                m_viewMat = glm::rotate(
                    m_viewMatWhenPressed,
                    glm::radians((float)i_mousePos_x - (float)m_input_mouseLocationWhenPressedX),
                    glm::vec3(0, 0.5, 0));
                m_viewMat = glm::rotate(
                    m_viewMat,
                    glm::radians((float)i_mousePos_y - (float)m_input_mouseLocationWhenPressedY),
                    glm::vec3(0.5, 0, 0));

            }
        }
        else if (m_input_rightMouseButton) {
            double i_mousePos_x = INT_MIN;
            double i_mousePos_y = INT_MIN;
            glfwGetCursorPos(m_applicationWindow, &i_mousePos_x, &i_mousePos_y);
            if (i_mousePos_x != INT_MIN && i_mousePos_y != INT_MIN) {
                m_viewMat = glm::translate(
                    m_viewMatWhenPressed,
                    glm::vec3(0, 0, (m_input_mouseLocationWhenPressedY - i_mousePos_y) * 0.02));
            }
        }

        // player
        if (m_playerMesh.lock()) {
            float i_translationStep = 0.05f;
            glm::mat4 i_modelMat = m_playerMesh.lock()->m_modelMat;
            if (glfwGetKey(m_applicationWindow, GLFW_KEY_W)) {
                i_modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -i_translationStep)) * i_modelMat;
            }
            if (glfwGetKey(m_applicationWindow, GLFW_KEY_S)) {
                i_modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, i_translationStep)) * i_modelMat;
            }
            if (glfwGetKey(m_applicationWindow, GLFW_KEY_A)) {
                i_modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(-i_translationStep, 0.0f, 0.0f)) * i_modelMat;
            }
            if (glfwGetKey(m_applicationWindow, GLFW_KEY_D)) {
                i_modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(i_translationStep, 0.0f, 0.0f)) * i_modelMat;
            }
            if (glfwGetKey(m_applicationWindow, GLFW_KEY_LEFT_SHIFT)) {
                i_modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -i_translationStep, 0.0f)) * i_modelMat;
            }
            if (glfwGetKey(m_applicationWindow, GLFW_KEY_SPACE)) {
                i_modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, i_translationStep, 0.0f)) * i_modelMat;
            }
            m_playerMesh.lock()->SetModelMat(i_modelMat);
        }
    }

    // shadow map
    {
        m_displayProgram->RenderMultiSpotLightShadowMap(m_lights,
            glm::vec3(0.0f),
            120.0f, 0.1f, 10.0f);
    }

    // first pass - G buffer
    {
        m_displayProgram->SetMVPMatrix(m_viewMat, VIEW);
        m_displayProgram->RenderGBuffer();
    }

    // second pass - lighting and shadow
    {
        m_displayProgram->SetLights(m_lights);
        glm::mat4 i_viewInverse = glm::inverse(m_viewMat);
        glm::vec3 i_cameraPos = glm::vec3(i_viewInverse[3]);
        m_displayProgram->SetCameraPosition(i_cameraPos);
        m_displayProgram->DrawCallWithGBuffer();
    }
}

void cViewerApplication::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        ExitApplication();
    }
}

void cViewerApplication::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS && !m_input_rightMouseButton) {
            m_input_middleMouseButton = true;
            glfwGetCursorPos(window, &m_input_mouseLocationWhenPressedX, &m_input_mouseLocationWhenPressedY);
            m_viewMatWhenPressed = m_viewMat;
            return;
        }
        if (action == GLFW_RELEASE) {
            m_input_middleMouseButton = false;
            return;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS && !m_input_middleMouseButton) {
            m_input_rightMouseButton = true;
            glfwGetCursorPos(window, &m_input_mouseLocationWhenPressedX, &m_input_mouseLocationWhenPressedY);
            m_viewMatWhenPressed = m_viewMat;
            return;
        }
        if (action == GLFW_RELEASE) {
            m_input_rightMouseButton = false;
            return;
        }
    }
}
