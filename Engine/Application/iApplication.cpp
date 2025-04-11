#include "iApplication.h"

iApplication* iApplication::instance = nullptr;

iApplication::iApplication()
{
}

void iApplication::Initialize()
{
	instance = this;

	// initialize glfw
	glfwInit();

	// set profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window
	m_applicationWindow = glfwCreateWindow(m_windowWidth, m_windowHeight, m_windowTitle.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(m_applicationWindow);

	glfwSwapInterval(1);

	// initialize glew
	glewInit();
	glewExperimental = GL_TRUE;

	// initialize ui (ImGUI)
	m_UiSystem = new cUserInterfaceSystem(m_applicationWindow, "#version 330");

	CustomInitialization();
}

void iApplication::Run()
{
	m_running = true;

	glfwSetKeyCallback(m_applicationWindow, _KeyCallBack);
	glfwSetMouseButtonCallback(m_applicationWindow, _MouseButtonCallback);
	glfwSetCursorPosCallback(m_applicationWindow, _MoveCursorCallback);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	while (m_running) {
		MainLoopFunc();
		m_UiSystem->DrawUI();
		glfwPollEvents();
		glfwSwapBuffers(m_applicationWindow);
	}
}


void iApplication::MainLoopFunc()
{
}

void iApplication::CustomInitialization()
{
}

void iApplication::ExitApplication()
{
	m_running = false;
	instance = nullptr;
	m_UiSystem->Destroy();

	glfwTerminate();
}

void iApplication::_KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	instance->KeyCallback(window, key, scancode, action, mods);
}

void iApplication::_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	instance->MouseButtonCallback(window, button, action, mods);
}

void iApplication::_MoveCursorCallback(GLFWwindow* window, double xpos, double ypos)
{
	instance->MoveCursorCallback(window, xpos, ypos);
}

void iApplication::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

void iApplication::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void iApplication::MoveCursorCallback(GLFWwindow* window, double xpos, double ypos)
{
}

iApplication::~iApplication()
{
}
