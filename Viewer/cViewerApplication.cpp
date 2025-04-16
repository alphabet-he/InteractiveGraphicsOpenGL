#include "cViewerApplication.h"

void cViewerApplication::CustomInitialization()
{
	m_windowWidth = 800;
	m_windowHeight = 600;
	m_windowTitle = "Viewer";

	m_meshSystem = new cMeshSystem();
}

void cViewerApplication::MainLoopFunc()
{
}

void cViewerApplication::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

void cViewerApplication::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}
