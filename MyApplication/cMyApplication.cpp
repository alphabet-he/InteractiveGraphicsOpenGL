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
	// mesh
	m_meshToRender = new cy::TriMesh();
	m_meshToRender->LoadFromFileObj("Assets/teapot.obj");
	m_meshToRender->ComputeBoundingBox();
	UploadTriMeshVertices();

	// shader
	LinkShaders("Assets/NormalVertexShader.glsl", "Assets/NormalFragmentShader.glsl");

	// set time
	m_lastBackgroundChangeTime = glfwGetTime();

	// set background
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);

	// set mvp matrix
	float i_centerZ = (m_meshToRender->GetBoundMax().z + m_meshToRender->GetBoundMin().z) * 0.5f;
	m_modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	m_modelMat = glm::rotate(m_modelMat, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	m_modelMat = glm::translate(m_modelMat, glm::vec3(0, 0, -i_centerZ));
	m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0));
	m_viewMatWhenPressed = m_viewMat;
	m_projectionMat = glm::perspective(
		glm::radians(45.0f),    // Field of View
		(float)m_windowWidth / (float)m_windowHeight, // Aspect Ratio
		0.1f, 100.0f  // Near & Far plane
	);
	
	glUseProgram(ShaderProgram);

	GLuint i_model = glGetUniformLocation(ShaderProgram, "model");
	glUniformMatrix4fv(i_model, 1, GL_FALSE, glm::value_ptr(m_modelMat));

	GLuint i_projection = glGetUniformLocation(ShaderProgram, "projection");
	glUniformMatrix4fv(i_projection, 1, GL_FALSE, glm::value_ptr(m_projectionMat));
}

void cMyApplication::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// exit
	if (key == GLFW_KEY_ESCAPE) {
		ExitApplication();
	}

	// recompile shaders
	if (key == GLFW_KEY_F6) {
		LinkShaders("Assets/StandardVertexShader.glsl", "Assets/OrangeFragmentShader.glsl");
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
	ChangeBackground(10.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(ShaderProgram);

	if (m_input_leftMouseButton) {
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

	GLuint i_view = glGetUniformLocation(ShaderProgram, "view");
	glUniformMatrix4fv(i_view, 1, GL_FALSE, glm::value_ptr(m_viewMat));

	glEnable(GL_PROGRAM_POINT_SIZE);
	glDrawElements(GL_TRIANGLES, m_meshToRender->NF() * 3, GL_UNSIGNED_INT, 0);
	
}

void cMyApplication::ChangeBackground(double i_deltaTime)
{
	if (glfwGetTime() - m_lastBackgroundChangeTime > i_deltaTime) {
		GLfloat i_currentColor[4];
		glGetFloatv(GL_COLOR_CLEAR_VALUE, i_currentColor);
		if (i_currentColor[0] == 1.0f) {
			glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		}

		m_lastBackgroundChangeTime = glfwGetTime();
	}
}
