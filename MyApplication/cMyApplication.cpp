#include "cMyApplication.h"
#include <CyCode/cyTriMesh.h>

cMyApplication::cMyApplication()
{
	m_windowWidth = 800;
	m_windowHeight = 600;
	m_windowTitle = "Junxuan's OpenGL Application";
}

void cMyApplication::HandleKeyboardFunc(unsigned char key, int x, int y)
{
	if (key == 27) {
		ExitApplication();
	}
}

void cMyApplication::CustomInitialization()
{
	glutTimerFunc(0, ChangeBackground, 0);

	// mesh
	m_meshToRender = new cy::TriMesh();
	m_meshToRender->LoadFromFileObj("Assets/teapot.obj");
	UploadTriMeshVertices();

	// shader
	LinkShaders("Assets/StandardVertexShader.glsl", "Assets/StandardFragmentShader.glsl");
}

void cMyApplication::DisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(ShaderProgram);

	glm::mat4 i_modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
	glm::mat4 i_viewMat = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 5.0f), // Camera position
		glm::vec3(0.0f, 0.0f, 0.0f), // Look-at position
		glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
	);
	glm::mat4 i_projectionMat = glm::perspective(
		glm::radians(45.0f),    // Field of View
		(float)m_windowWidth / (float)m_windowHeight, // Aspect Ratio
		0.1f, 100.0f  // Near & Far plane
	);
	glm::mat4 i_mvpMat = i_projectionMat * i_viewMat * i_modelMat;
	GLuint MVP = glGetUniformLocation(ShaderProgram, "MVP");
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(i_mvpMat));

	glDrawElements(GL_POINTS, m_meshToRender->NF() * 3, GL_UNSIGNED_INT, 0);
	glutSwapBuffers();
}

void cMyApplication::ChangeBackground(int value)
{
	GLfloat i_currentColor[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, i_currentColor);
	if (i_currentColor[0] == 1.0f) {
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	}
	else {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	glutPostRedisplay();
	glutTimerFunc(1000, ChangeBackground, 0);
}
