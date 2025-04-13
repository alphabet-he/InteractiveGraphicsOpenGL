#include "cMyApplication.h"
#include <CyCode/cyTriMesh.h>

cMyApplication::cMyApplication()
{
	m_windowWidth = 800;
	m_windowHeight = 600;
	m_windowTitle = "Junxuan's OpenGL Application";

	m_files.push_back("Assets/plane.obj");
	m_files.push_back("Assets/teapot/teapot.obj");
	m_files.push_back("Assets/sprite/Mario.png");

	m_meshSystem = new cMeshSystem();
}


void cMyApplication::CustomInitialization()
{
	// display vertex buffer
	sVertexBufferStruct* i_displayBufferStruct = new sVertexBufferStruct(true, true, true, false);
	m_displayProgram = new cVertexShaderProgram(i_displayBufferStruct);

	m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0));
	m_viewMatWhenPressed = m_viewMat;
	m_projectionMat = glm::perspective(
		glm::radians(45.0f),    // Field of View
		(float)m_windowWidth / (float)m_windowHeight, // Aspect Ratio
		0.1f, 100.0f  // Near & Far plane
	);
	m_displayProgram->LinkShaders("Assets/shader/TextureVertexShader.glsl", "Assets/shader/TextureFragmentShader.glsl");
	
	m_displayProgram->SetMVPMatrix(m_projectionMat, PROJECTION);
	m_displayProgram->InitializeShadowMap(2048, 2048);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_lightPosition = glm::vec3(0.8f, 1.2f, 1.0f);

	m_lightProgram = new cVertexShaderProgram(new sVertexBufferStruct(true, false, false, false));
	m_lightProgram->LinkShaders("Assets/shader/StandardVertexShader.glsl", "Assets/shader/StandardFragmentShader.glsl");
	m_lightMesh = m_lightProgram->UploadMesh("Assets/sphere.obj", new sTextureUsage(false, false, false));
	{
		glm::mat4 i_ModelMat = glm::mat4(1.0f);
		i_ModelMat = glm::translate(i_ModelMat, m_lightPosition);
		i_ModelMat = glm::scale(i_ModelMat, glm::vec3(0.05f));
		m_lightMesh->SetModelMat(i_ModelMat);
	}
	m_lightProgram->SetMVPMatrix(m_projectionMat, PROJECTION);

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// ui
	{
		sVertexBufferStruct* i_displayBufferStruct = new sVertexBufferStruct(true, true, true, false);
		cVertexShaderProgram* i_uiObjectProgram = new cVertexShaderProgram(i_displayBufferStruct);

		glm::mat4 i_projectionMat = glm::perspective(
			glm::radians(45.0f),    // Field of View
			(float)m_windowWidth / (float)m_windowHeight, // Aspect Ratio
			0.1f, 100.0f  // Near & Far plane
		);
		i_uiObjectProgram->LinkShaders("Assets/shader/TextureVertexShader.glsl", "Assets/shader/TextureFragmentShader.glsl");

		i_uiObjectProgram->SetMVPMatrix(i_projectionMat, PROJECTION);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		i_uiObjectProgram->UploadMesh("Assets/plane.obj", new sTextureUsage(false, true, false));

		i_uiObjectProgram->InitializeScreenTexture(600, 600);
		GLuint i_objTexture = i_uiObjectProgram->RenderToScreenTexture(glm::vec3(5, 8, 5), glm::vec3(-5, -8, -5), std::vector<cVertexShaderProgram*>{i_uiObjectProgram});

		sPanel* i_panel = new sPanel("Test", 0, m_windowHeight-128, m_windowWidth, 128, 1, 0, 0, 0.1);

		//GLuint i = Graphics::GenerateTextureFromImage("Assets/sprite/Mario.png");
		sButton* i_button = new sButton(32, 32, 64, 64, i_objTexture, []() {std::cout << "Button clicked!\n"; });
		i_panel->m_components.push_back(i_button);

		m_UiSystem->AddPanel(i_panel);
	}
}

void cMyApplication::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// exit
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		ExitApplication();
	}

	// control
	if (key == GLFW_KEY_LEFT_CONTROL) {
		if (action == GLFW_PRESS) {
			m_input_ctrlKey = true;
		}
		else if (action == GLFW_RELEASE){
			m_input_ctrlKey = false;
		}
	}

	// select new mesh
	if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9 && action == GLFW_PRESS) {
		if (key - GLFW_KEY_1 < m_files.size()) {
			m_newMeshSelected = key - GLFW_KEY_1;
		}
	}

	// delete mesh
	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS && m_selectedMesh) {
		m_meshSystem->DeleteMesh(m_selectedMesh);
		m_selectedMesh = nullptr;
	}

	// switch transformation
	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		m_transformationAction = static_cast<eTransformation>(
			(m_transformationAction == eTransformation::COUNT - 1)
			? 0
			: m_transformationAction + 1
			);
	}
}

void cMyApplication::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (action == GLFW_PRESS && !m_input_rightMouseButton) {
			m_input_leftMouseButton = true;
			glfwGetCursorPos(window, &m_input_mouseLocationWhenPressedX, &m_input_mouseLocationWhenPressedY);
			m_viewMatWhenPressed = m_viewMat;
			return;
		}
		if (action == GLFW_RELEASE) {
			m_input_leftMouseButton = false;
			return;
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS && !m_input_leftMouseButton) {
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

		if (m_newMeshSelected != -1) {
			std::string i_filename = m_files[m_newMeshSelected];
			bool b_isPng = false;
			const char* i_meshName;
			if (i_filename.compare(i_filename.length() - 4, 4, ".png") == 0) {
				b_isPng = true;
				i_meshName = "Assets/plane.obj";
			}
			else {
				i_meshName = i_filename.c_str();
			}
			std::shared_ptr<cMesh> i_mesh = m_displayProgram->UploadMesh(i_meshName, new sTextureUsage(false, true, false));
			m_meshSystem->RegisterMesh(i_mesh);
			{
				glm::vec3 i_worldPos = rayStart + rayDir * 5.0f;
				glm::mat4 i_model = glm::translate(glm::mat4(1.0f), i_worldPos);
				i_mesh->SetModelMat(i_model);
			}
			if (b_isPng) {
				i_mesh->UploadPNGTexture(FLAT_SPRITE, i_filename);
			}
			m_newMeshSelected = -1;
		}
		else {
			sMeshInstance* i = m_meshSystem->SelectMesh(rayStart, rayDir);
			if (i) {
				std::cout << "Selected mesh: " << i->m_mesh.get() << "\n";
				m_selectedMesh = i;
			}
			else {
				m_selectedMesh = nullptr;
			}
		}
	}
}

void cMyApplication::MainLoopFunc()
{
	// input
	{
		if (m_input_leftMouseButton) {
			double i_mousePos_x = INT_MIN;
			double i_mousePos_y = INT_MIN;
			glfwGetCursorPos(m_applicationWindow, &i_mousePos_x, &i_mousePos_y);
			if (i_mousePos_x != INT_MIN && i_mousePos_y != INT_MIN) {
				if (m_input_ctrlKey) {
					glm::mat4 i_rotationMatrix = glm::rotate(
						glm::mat4(1.0f),
						glm::radians((float)i_mousePos_x - (float)m_input_mouseLocationWhenPressedX) * 0.1f,
						glm::vec3(0.0f, 1.0f, 0.0f));
					m_lightPosition = i_rotationMatrix * glm::vec4(m_lightPosition, 1.0f);
				}
				else {
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
			glm::mat4 i_modelMat = m_selectedMesh->m_mesh->m_modelMat;
			float i_translationStep = 0.05f;
			float i_rotationStep = glm::radians(3.0f);
			float i_scaleStep = 0.05f;
			switch (m_transformationAction)
			{
			case cMyApplication::LOCATION_X:
				i_modelMat = glm::translate(i_modelMat, glm::vec3(i_transformDir * i_translationStep, 0.0f, 0.0f));
				break;
			case cMyApplication::LOCATION_Y:
				i_modelMat = glm::translate(i_modelMat, glm::vec3(0.0f, i_transformDir * i_translationStep, 0.0f));
				break;
			case cMyApplication::LOCATION_Z:
				i_modelMat = glm::translate(i_modelMat, glm::vec3(0.0f, 0.0f, i_transformDir * i_translationStep));
				break;
			case cMyApplication::ROTATION_X:
				i_modelMat = glm::rotate(i_modelMat, i_transformDir * i_rotationStep, glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case cMyApplication::ROTATION_Y:
				i_modelMat = glm::rotate(i_modelMat, i_transformDir * i_rotationStep, glm::vec3(0.0f, 1.0f, 0.0f));
				break;
			case cMyApplication::ROTATION_Z:
				i_modelMat = glm::rotate(i_modelMat, i_transformDir * i_rotationStep, glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case cMyApplication::SCALE:
				i_modelMat = glm::scale(i_modelMat, glm::vec3(1.0f + i_transformDir * i_scaleStep));
				break;
			case cMyApplication::COUNT:
				break;
			default:
				break;
			}
			m_selectedMesh->m_mesh->SetModelMat(i_modelMat);
		}
	}

	m_displayProgram->RenderSpotLightShadowMap(m_lightPosition,
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
		m_displayProgram->SetLightingPosition(m_lightPosition);

		m_displayProgram->DrawCall();
	}

	{
		glm::mat4 i_ModelMat = glm::mat4(1.0f);
		i_ModelMat = glm::translate(i_ModelMat, m_lightPosition);
		i_ModelMat = glm::scale(i_ModelMat, glm::vec3(0.02f));
		m_lightMesh->SetModelMat(i_ModelMat);
		m_lightProgram->SetMVPMatrix(m_viewMat, VIEW);
		m_lightProgram->DrawCall();
	}
	
}
