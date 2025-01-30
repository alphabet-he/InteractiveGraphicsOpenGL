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
	UploadTriMeshVertices();

	// shader
	LinkShaders("Assets/StandardVertexShader.glsl", "Assets/StandardFragmentShader.glsl");

	// set time
	m_lastBackgroundChangeTime = glfwGetTime();
}

void cMyApplication::MainLoopFunc()
{
	glUseProgram(ShaderProgram);

	glm::mat4 i_modelMat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
	i_modelMat = glm::scale(i_modelMat, glm::vec3(0.1f, 0.1f, 0.1f));
	glm::mat4 i_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0));
	glm::mat4 i_projectionMat = glm::perspective(
		glm::radians(45.0f),    // Field of View
		(float)m_windowWidth / (float)m_windowHeight, // Aspect Ratio
		0.1f, 100.0f  // Near & Far plane
	);
	glm::mat4 i_mvpMat = i_projectionMat * i_viewMat * i_modelMat;
	GLuint MVP = glGetUniformLocation(ShaderProgram, "MVP");
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(i_mvpMat));

	glDrawElements(GL_POINTS, m_meshToRender->NF() * 3, GL_UNSIGNED_INT, 0);
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
		glClear(GL_COLOR_BUFFER_BIT);

		m_lastBackgroundChangeTime = glfwGetTime();
	}
}
