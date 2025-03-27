#include "cMyApplication.h"
#include <CyCode/cyTriMesh.h>

cMyApplication::cMyApplication()
{
	m_windowWidth = 800;
	m_windowHeight = 600;
	m_windowTitle = "Junxuan's OpenGL Application";
}


void cMyApplication::CustomInitialization()
{

	// display vertex buffer
	sVertexBufferStruct* i_displayBufferStruct = new sVertexBufferStruct(true, true, true, true);
	m_displayProgram = new cVertexShaderProgram(i_displayBufferStruct);
	m_displayProgram->SetTessellationShader("Assets/shader/TessellationVertexShader.glsl", 
		"Assets/shader/TessellationControlShader.glsl",
		"Assets/shader/TessellationEvaluationShader.glsl",
		"Assets/shader/TessellationFragmentShader.glsl");
	m_displayProgram->InitializeGeometryShaderProgram();
	
	// plane
	cMesh* i_planeMesh = m_displayProgram->UploadMesh("Assets/plane.obj", new sTextureUsage(false, false, false));
	{
		i_planeMesh->m_cyMesh->ComputeBoundingBox();
		cy::Vec3<float> i_centerCy = (i_planeMesh->m_cyMesh->GetBoundMax() + i_planeMesh->m_cyMesh->GetBoundMin()) * 0.5f;
		glm::vec3 i_center = glm::vec3(i_centerCy.x, i_centerCy.y, i_centerCy.z);
		glm::mat4 i_ModelMat = glm::mat4(1.0f);
		i_ModelMat = glm::rotate(i_ModelMat, glm::radians(90.0f), glm::vec3(1, 0, 0));
		i_ModelMat = glm::translate(i_ModelMat, -i_center);
		i_planeMesh->SetModelMat(i_ModelMat);
		i_planeMesh->UploadPNGTexture(NORMAL_MAP, "Assets/teapot_normal.png");
		i_planeMesh->UploadPNGTexture(DISPLACEMENT_MAP, "Assets/teapot_disp.png");
	}

	m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0));
	m_viewMatWhenPressed = m_viewMat;
	m_projectionMat = glm::perspective(
		glm::radians(45.0f),    // Field of View
		(float)m_windowWidth / (float)m_windowHeight, // Aspect Ratio
		0.1f, 100.0f  // Near & Far plane
	);
	
	m_displayProgram->SetMVPMatrix(m_projectionMat, PROJECTION);
	m_displayProgram->InitializeShadowMap(2048, 2048);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_lightPosition = glm::vec3(0.8f, 1.2f, 1.0f);

	m_lightProgram = new cVertexShaderProgram(new sVertexBufferStruct(true, false, false, false));
	m_lightProgram->LinkShaders("Assets/shader/StandardVertexShader.glsl", "Assets/shader/StandardFragmentShader.glsl");
	cMesh* i_lightMesh = m_lightProgram->UploadMesh("Assets/sphere.obj", new sTextureUsage(false, false, false));
	{
		glm::mat4 i_ModelMat = glm::mat4(1.0f);
		i_ModelMat = glm::translate(i_ModelMat, m_lightPosition);
		i_ModelMat = glm::scale(i_ModelMat, glm::vec3(0.05f));
		i_lightMesh->SetModelMat(i_ModelMat);
	}
	m_lightProgram->SetMVPMatrix(m_projectionMat, PROJECTION);

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
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

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		b_showingWireframe = !b_showingWireframe;
	}

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		m_tessellationLevel = std::max(1, m_tessellationLevel - 1);
		m_displayProgram->SetTessellationLevel(m_tessellationLevel);
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		m_tessellationLevel = std::min(10, m_tessellationLevel + 1);
		m_displayProgram->SetTessellationLevel(m_tessellationLevel);
	}
}

void cMyApplication::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
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
	}
	
	m_displayProgram->RenderSpotLightShadowMap(m_lightPosition,
		glm::vec3(0.0f),
		120.0f, 0.1f, 10.0f);

	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, m_windowWidth, m_windowHeight);

		m_displayProgram->SetMVPMatrix(m_viewMat, VIEW);
		glm::mat4 i_viewInverse = glm::inverse(m_viewMat);
		glm::vec3 i_cameraPos = glm::vec3(i_viewInverse[3]);
		m_displayProgram->SetCameraPosition(i_cameraPos);
		m_displayProgram->SetLightingPosition(m_lightPosition);

		m_displayProgram->DrawCall();
		if (b_showingWireframe) {
			m_displayProgram->GeometryDrawCall();
		}
	}

	{
		glm::mat4 i_ModelMat = glm::mat4(1.0f);
		i_ModelMat = glm::translate(i_ModelMat, m_lightPosition);
		i_ModelMat = glm::scale(i_ModelMat, glm::vec3(0.02f));
		m_lightProgram->m_meshes[0]->SetModelMat(i_ModelMat);
		m_lightProgram->SetMVPMatrix(m_viewMat, VIEW);
		m_lightProgram->DrawCall();
	}
	
}

void cMyApplication::ChangeBackground(double i_deltaTime)
{
	if (glfwGetTime() - m_lastBackgroundChangeTime > i_deltaTime) {
		GLfloat i_currentColor[4];
		glGetFloatv(GL_COLOR_CLEAR_VALUE, i_currentColor);
		if (i_currentColor[0] == 0.0f) {
			glClearColor(0.1f, 0.0f, 0.2f, 1.0f);
		}
		else {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		}

		m_lastBackgroundChangeTime = glfwGetTime();
	}
}
