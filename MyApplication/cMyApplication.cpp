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
	cy::TriMesh* i_mesh = new cy::TriMesh();
	i_mesh->LoadFromFileObj("Assets/teapot.obj");
	UploadTriMeshVertices(i_mesh);
}

void cMyApplication::DisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
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
