#include "cMyApplication.h"
#include <CyCode/cyTriMesh.h>
#include <JSON/json.hpp>
#include <filesystem>

cMyApplication::cMyApplication()
{
	m_windowWidth = 800;
	m_windowHeight = 600;
	m_windowTitle = "Junxuan's OpenGL Application";

	m_meshSystem = new cMeshSystem();
	
}


void cMyApplication::CustomInitialization()
{

	// display program
	{
		sVertexBufferStruct* i_displayBufferStruct = new sVertexBufferStruct(true, true, true, false);
		m_displayProgram = new cVertexShaderProgram(i_displayBufferStruct);

		m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0));
		m_viewMatWhenPressed = m_viewMat;
		m_projectionMat = glm::perspective(
			glm::radians(45.0f),    // Field of View
			(float)m_windowWidth / (float)m_windowHeight, // Aspect Ratio
			0.1f, 100.0f  // Near & Far plane
		);
		m_displayProgram->LinkShaders("../Assets/shader/TextureVertexShader.glsl", "../Assets/shader/TextureBlinnFragmentShader.glsl");

		m_displayProgram->SetMVPMatrix(m_projectionMat, PROJECTION);
		m_displayProgram->InitializeShadowMap(2048, 2048);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// light program
	{

		m_lightProgram = new cVertexShaderProgram(new sVertexBufferStruct(true, false, false, false));
		m_lightProgram->LinkShaders("../Assets/shader/StandardVertexShader.glsl", "../Assets/shader/StandardFragmentShader.glsl");

		sMeshInstance* i_light = m_meshSystem->RegisterMesh("../Assets/sphere.obj");
		m_lightMesh = i_light->GetMesh();
		m_lightProgram->UploadMesh(m_lightMesh, new sTextureUsage(false, false, false));
		{
			glm::mat4 i_ModelMat = glm::mat4(1.0f);
			i_ModelMat = glm::translate(i_ModelMat, glm::vec3(0.8f, 1.2f, 1.0f));
			i_ModelMat = glm::scale(i_ModelMat, glm::vec3(0.02f));
			m_lightMesh.lock()->SetModelMat(i_ModelMat);
		}
		m_lightProgram->SetMVPMatrix(m_projectionMat, PROJECTION);
	}

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// initialize m_fileNameModelPairs
	std::vector<sMeshInstance*> i_meshInstances;
	cMeshSystem* m_tempMeshSystem = new cMeshSystem();
	{
		
		sMeshInstance* i_planeInstance = m_tempMeshSystem->RegisterMesh("../Assets/plane.obj");
		sMeshInstance* i_teapotInstance = m_tempMeshSystem->RegisterMesh("../Assets/teapot/teapot.obj");
		glm::mat4 i_planeModelMat = glm::mat4(1.0f);
		glm::mat4 i_teapotModelMat = glm::mat4(1.0f);
		// plane model mat
		{
			glm::vec3 i_center = (i_planeInstance->m_boundingBoxMax + i_planeInstance->m_boundingBoxMin) * 0.5f;
			i_planeModelMat = glm::scale(i_planeModelMat, glm::vec3(1.0f));
			i_planeModelMat = glm::rotate(i_planeModelMat, glm::radians(90.0f), glm::vec3(1, 0, 0));
			i_planeModelMat = glm::translate(i_planeModelMat, -i_center);
			m_spritePlaneModelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f)) * i_planeModelMat;
		}
		// teapot model mat
		{
			glm::vec3 i_center = (i_teapotInstance->m_boundingBoxMax + i_teapotInstance->m_boundingBoxMin) * 0.5f;
			i_teapotModelMat = glm::scale(i_teapotModelMat, glm::vec3(0.1f));
			i_teapotModelMat = glm::rotate(i_teapotModelMat, glm::radians(-90.0f), glm::vec3(1, 0, 0));
			i_teapotModelMat = glm::translate(i_teapotModelMat, -i_center);
		}

		m_fileNameModelPairs.push_back({ "../Assets/plane.obj", i_planeModelMat });
		m_fileNameModelPairs.push_back({ "../Assets/teapot/teapot.obj", i_teapotModelMat });
		m_fileNameModelPairs.push_back({ "../Assets/sprite/ditto.png", i_planeModelMat });

		i_meshInstances.push_back(i_planeInstance);
		i_meshInstances.push_back(i_teapotInstance);
		i_meshInstances.push_back(i_planeInstance);
	}

	// ui program for mesh selection
	{
		sVertexBufferStruct* i_displayBufferStruct = new sVertexBufferStruct(true, true, true, false);
		cVertexShaderProgram* i_uiObjectProgram = new cVertexShaderProgram(i_displayBufferStruct);

		glm::mat4 i_projectionMat = glm::perspective(
			glm::radians(45.0f),    // Field of View
			1.0f, // Aspect Ratio
			0.1f, 100.0f  // Near & Far plane
		);
		i_projectionMat[1][1] *= -1.0f;
		i_uiObjectProgram->LinkShaders("../Assets/shader/TextureVertexShader.glsl", "../Assets/shader/TextureFragmentShader.glsl");

		i_uiObjectProgram->SetMVPMatrix(i_projectionMat, PROJECTION);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		m_meshSelectionPanel = new sPanel("MeshSelection", 0, m_windowHeight - 128, m_windowWidth, 128, 1, 1, 1, 0.1);

		for (int i = 0; i < m_fileNameModelPairs.size(); i++) {
			std::pair<std::string, glm::mat4> i_file = m_fileNameModelPairs[i];
			GLuint i_objTexture;
			if (i_file.first.compare(i_file.first.length() - 4, 4, ".png") == 0) {
				i_objTexture = Graphics::GenerateTextureFromImage(i_file.first);

			}
			else {
				cVertexShaderProgram* i_fboProgram = new cVertexShaderProgram(new sVertexBufferStruct(true, true, true, false));
				i_fboProgram->InitializeScreenTexture(600, 600);
				glm::mat4 i_proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
				i_fboProgram->SetMVPMatrix(i_proj, PROJECTION);

				i_uiObjectProgram->UploadMesh(i_meshInstances[i]->GetMesh(), new sTextureUsage(false, true, false));
				i_meshInstances[i]->GetMesh().lock()->SetModelMat(i_file.second);
				i_objTexture = i_fboProgram->RenderToScreenTexture(glm::vec3(0, 0, 5), glm::vec3(0, 0, -5), std::vector<cVertexShaderProgram*>{i_uiObjectProgram});
				m_tempMeshSystem->DeleteMesh(i_meshInstances[i]);
			}
			sMeshSelectionButton* i_button = new sMeshSelectionButton(
				32 * (i + 1) + 64 * i,
				32, 64, 64,
				i_objTexture,
				i,
				nullptr
			);

			// capture i_button and this
			i_button->m_callback = [this, i_button]() {
				m_newMeshSelected = i_button->m_selectionInd;
				};
			m_meshSelectionPanel->m_components.push_back(i_button);
		}

		m_UiSystem->AddPanel(m_meshSelectionPanel);
	}

	// transformation ui
	{
		sPanel* i_panel = new sPanel("Transformation", m_windowWidth - 176, 0, 160, 160, 1, 1, 1, 0.1);
		std::vector<std::vector<std::string>> i_uiAssets;
		std::vector<std::string> i_move = { "../Assets/ui/movex.png", "../Assets/ui/movey.png", "../Assets/ui/movez.png" };
		std::vector<std::string> i_rotate = { "../Assets/ui/rotatex.png", "../Assets/ui/rotatey.png", "../Assets/ui/rotatez.png" };
		std::vector<std::string> i_scale = { "../Assets/ui/scale.png"};
		i_uiAssets = { i_move, i_rotate, i_scale };
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (j < i_uiAssets[i].size()) {
					GLuint i_objTexture = Graphics::GenerateTextureFromImage(i_uiAssets[i][j]);
					sTransformationButton* i_button = new sTransformationButton(
						16 * (j + 1) + 32 * j,
						16 * (i + 1) + 32 * i, 
						32, 32,
						i_objTexture,
						i * 3 + j,
						nullptr
					);

					// capture i_button and this
					i_button->m_callback = [this, i_button]() {
						m_transformationAction = i_button->m_transformation;
						};
					i_panel->m_components.push_back(i_button);
				}
			}
		}
		m_UiSystem->AddPanel(i_panel);
	}

	// player sprite selection ui
	{
		m_playerSprites = { "../Assets/sprite/Mario.png", "../Assets/sprite/ditto.png" };
		m_spriteSelectionPanel = new sPanel("SpriteSelection", 0, m_windowHeight - 128, m_windowWidth, 128, 1, 1, 1, 0.1);
		for (int i = 0; i < m_playerSprites.size(); i++) {
			GLuint i_objTexture = Graphics::GenerateTextureFromImage(m_playerSprites[i]);
			sMeshSelectionButton* i_button = new sMeshSelectionButton(
				32 * (i + 1) + 64 * i,
				32, 64, 64,
				i_objTexture,
				i,
				nullptr
			);

			// capture i_button and this
			i_button->m_callback = [this, i_button]() {
				if (!m_playerMesh.lock()) {
					m_newPlayerSpriteSelected = i_button->m_selectionInd;
				}
				};
			m_spriteSelectionPanel->m_components.push_back(i_button);
		}
		m_spriteSelectionPanel->b_active = false;
		m_UiSystem->AddPanel(m_spriteSelectionPanel);
	}

	// switching panel ui
	{
		sPanel* i_panel = new sPanel("SwitchingPanel", 0, m_windowHeight - 128 - 32, 158, 32, 1, 1, 1, 0.1);
		sButton* i_meshButton = new sButton(
			0, 0,
			64, 32,
			"Mesh",
			[this]() {
				m_spriteSelectionPanel->b_active = false;
				m_meshSelectionPanel->b_active = true;
			}
		);
		sButton* i_spriteButton = new sButton(
			64, 0,
			94, 32,
			"PlayerPawn",
			[this]() {
				m_meshSelectionPanel->b_active = false;
				m_spriteSelectionPanel->b_active = true;
			}
		);
		i_panel->m_components.push_back(i_meshButton);
		i_panel->m_components.push_back(i_spriteButton);
		m_UiSystem->AddPanel(i_panel);
	}

	// export ui
	{
		sPanel* i_panel = new sPanel("ExportPanel", 0, 0, 64, 32, 1, 1, 1, 0.1);
		sButton* i_Button = new sButton(
			0, 0,
			64, 32,
			"Export",
			[this]() {
				Export();
			}
		);
		i_panel->m_components.push_back(i_Button);
		m_UiSystem->AddPanel(i_panel);
	}
}

void cMyApplication::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// exit
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		ExitApplication();
	}

	// delete mesh
	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS && m_selectedMesh) {
		m_meshSystem->DeleteMesh(m_selectedMesh);
		m_selectedMesh = nullptr;
	}
}

void cMyApplication::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
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

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

		glm::ivec4 i_viewport(0, 0, m_windowWidth, m_windowHeight);
		double i_mouseX = INT_MIN;
		double i_mouseY = INT_MIN;
		glfwGetCursorPos(window, &i_mouseX, &i_mouseY);
		glm::vec3 rayStart = glm::unProject(glm::vec3(i_mouseX, m_windowHeight - i_mouseY, 0.0f),
			m_viewMat, m_projectionMat, i_viewport);
		glm::vec3 rayEnd = glm::unProject(glm::vec3(i_mouseX, m_windowHeight - i_mouseY, 1.0f),
			m_viewMat, m_projectionMat, i_viewport);
		glm::vec3 rayDir = glm::normalize(rayEnd - rayStart);

		if (m_newMeshSelected != -1 || m_newPlayerSpriteSelected != -1) {

			std::string i_filename;
			if (m_newMeshSelected != -1) {
				i_filename = m_fileNameModelPairs[m_newMeshSelected].first;
			}
			else {
				i_filename = m_playerSprites[m_newPlayerSpriteSelected];
			}
			bool b_isPng = false;
			const char* i_meshName;
			if (i_filename.compare(i_filename.length() - 4, 4, ".png") == 0) {
				b_isPng = true;
				i_meshName = "../Assets/plane.obj";
			}
			else {
				i_meshName = i_filename.c_str();
			}
			
			sMeshInstance* i_instance = m_meshSystem->RegisterMesh(i_meshName);
			m_displayProgram->UploadMesh(i_instance->GetMesh(), new sTextureUsage(false, true, false));
			{
				glm::mat4 i_model;
				if (b_isPng) {
					i_model = m_spritePlaneModelMat;
				}
				else {
					i_model = m_fileNameModelPairs[m_newMeshSelected].second;
				}
				
				glm::vec3 i_worldPos = rayStart + rayDir * 5.0f;
				i_model = glm::translate(i_model, i_worldPos);
				i_instance->GetMesh().lock()->SetModelMat(i_model);
			}
			if (b_isPng) {
				i_instance->GetMesh().lock()->UploadPNGTexture(FLAT_SPRITE, i_filename);
			}
			if (m_newPlayerSpriteSelected != -1) {
				m_playerMesh = i_instance->GetMesh();
			}
			m_newMeshSelected = -1;
			m_newPlayerSpriteSelected = -1;
		}
		else {
			sMeshInstance* i = m_meshSystem->SelectMesh(rayStart, rayDir);
			if (i) {
				std::cout << "Selected mesh: " << i->GetMesh().lock() << "\n";
				m_selectedMesh = i;
			}
			else {
				m_selectedMesh = nullptr;
			}
		}
	}
}

void cMyApplication::Export()
{
	using json = nlohmann::json;

	json sceneJson;

	m_meshSystem->ForEachMesh([&sceneJson, this](sMeshInstance* i_mesh) {
		
		std::weak_ptr<cMesh> i = i_mesh->GetMesh();
		if (auto i_meshPtr = i.lock()) {
			json i_meshJson;
			std::filesystem::path utf8_path = i_meshPtr->m_filePath;
			i_meshJson["file"] = utf8_path.u8string();

			// flatten model matrix to 16-element array
			std::vector<float> matArray(16);
			const float* matPtr = glm::value_ptr(i_meshPtr->m_modelMat);
			for (int i = 0; i < 16; ++i) {
				matArray[i] = matPtr[i];
			}
			i_meshJson["model_mat"] = matArray;

			// category
			if (i.lock() == m_lightMesh.lock()) {
				i_meshJson["category"] = "light";
			}
			else if (i.lock() == m_playerMesh.lock()) {
				i_meshJson["category"] = "player";
			}
			else {
				i_meshJson["category"] = "mesh";
			}

			json i_texturesJson;
			for (const auto& [usage, path] : i_meshPtr->m_textureFiles) {
				std::filesystem::path utf8_path = path;
				i_texturesJson[std::to_string(static_cast<int>(usage))] = utf8_path.u8string();
			}
			i_meshJson["textures"] = i_texturesJson;

			sceneJson["meshes"].push_back(i_meshJson);
		}
		
		});

	sceneJson["vertex_shader"] = m_displayProgram->GetVertexShaderPath();
	sceneJson["fragment_shader"] = m_displayProgram->GetFragmentShaderPath();

	std::ofstream file("../Export/scene.json");
	if (!file) {
		std::cerr << "Failed to open output file\n";
		return;
	}

	file << sceneJson.dump(4);
	file.close();
}

void cMyApplication::MainLoopFunc()
{
	// input
	{
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

		if ((glfwGetKey(m_applicationWindow, GLFW_KEY_LEFT) || glfwGetKey(m_applicationWindow, GLFW_KEY_RIGHT)) && m_selectedMesh) {
			int i_transformDir = 1;
			if (glfwGetKey(m_applicationWindow, GLFW_KEY_LEFT)) i_transformDir = -1;
			glm::mat4 i_modelMat = m_selectedMesh->GetMesh().lock()->m_modelMat;
			float i_translationStep = 0.05f;
			float i_rotationStep = glm::radians(3.0f);
			float i_scaleStep = 0.05f;
			glm::mat4 i_uniform = glm::mat4(1.0f);
			switch (m_transformationAction)
			{
			case LOCATION_X:
				i_modelMat = glm::translate(i_uniform, glm::vec3(i_transformDir * i_translationStep, 0.0f, 0.0f)) * i_modelMat;
				break;
			case LOCATION_Y:
				i_modelMat = glm::translate(i_uniform, glm::vec3(0.0f, i_transformDir * i_translationStep, 0.0f)) * i_modelMat;
				break;
			case LOCATION_Z:
				i_modelMat = glm::translate(i_uniform, glm::vec3(0.0f, 0.0f, i_transformDir * i_translationStep)) * i_modelMat;
				break;
			case ROTATION_X:
				i_modelMat = glm::rotate(i_uniform, i_transformDir * i_rotationStep, glm::vec3(1.0f, 0.0f, 0.0f)) * i_modelMat;
				break;
			case ROTATION_Y:
				i_modelMat = glm::rotate(i_uniform, i_transformDir * i_rotationStep, glm::vec3(0.0f, 1.0f, 0.0f)) * i_modelMat;
				break;
			case ROTATION_Z:
				i_modelMat = glm::rotate(i_uniform, i_transformDir * i_rotationStep, glm::vec3(0.0f, 0.0f, 1.0f)) * i_modelMat;
				break;
			case SCALE:
				i_modelMat = glm::scale(i_uniform, glm::vec3(1.0f + i_transformDir * i_scaleStep)) * i_modelMat;
				break;
			case COUNT:
			default:
				break;
			}
			m_selectedMesh->GetMesh().lock()->SetModelMat(i_modelMat);
		}
	}

	m_displayProgram->RenderSpotLightShadowMap(glm::vec3(m_lightMesh.lock()->m_modelMat[3]),
		glm::vec3(0.0f),
		120.0f, 0.1f, 10.0f);

	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(0, 0, m_windowWidth, m_windowHeight);

		m_displayProgram->SetMVPMatrix(m_viewMat, VIEW);
		glm::mat4 i_viewInverse = glm::inverse(m_viewMat);
		glm::vec3 i_cameraPos = glm::vec3(i_viewInverse[3]);
		m_displayProgram->SetCameraPosition(i_cameraPos);
		m_displayProgram->SetLightingPosition(glm::vec3(m_lightMesh.lock()->m_modelMat[3]));

		m_displayProgram->DrawCall();
	}

	{
		m_lightProgram->SetMVPMatrix(m_viewMat, VIEW);
		m_lightProgram->DrawCall();
	}
	
}
