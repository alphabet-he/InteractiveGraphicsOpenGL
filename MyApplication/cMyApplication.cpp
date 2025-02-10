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
	UploadTriMesh("Assets/teapot/teapot.obj");
	m_meshToRender->ComputeBoundingBox();

	// shader
	LinkShaders("Assets/TextureVertexShader.glsl", "Assets/TextureFragmentShader.glsl");

	// set time
	m_lastBackgroundChangeTime = glfwGetTime();

	// set background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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

	m_lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
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

	// recompile shaders
	if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
		LinkShaders("Assets/NormalVertexShader.glsl", "Assets/NormalFragmentShader.glsl");
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

	glUseProgram(ShaderProgram);

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

	GLuint i_view = glGetUniformLocation(ShaderProgram, "view");
	glUniformMatrix4fv(i_view, 1, GL_FALSE, glm::value_ptr(m_viewMat));

	glm::mat4 i_viewInverse = glm::inverse(m_viewMat);
	glm::vec3 i_cameraPos = glm::vec3(i_viewInverse[3]);
	GLuint i_camera = glGetUniformLocation(ShaderProgram, "camera_position");
	glUniform3f(i_camera, i_cameraPos.x, i_cameraPos.y, i_cameraPos.z);

	GLuint i_light = glGetUniformLocation(ShaderProgram, "light_position");
	glUniform3f(i_light, m_lightPosition.x, m_lightPosition.y, m_lightPosition.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTex);
	glUniform1i(glGetUniformLocation(ShaderProgram, "texture_diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularTex);
	glUniform1i(glGetUniformLocation(ShaderProgram, "texture_specular"), 1);

	glEnable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLES, 0, m_meshToRender->NF() * 3);
	
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
