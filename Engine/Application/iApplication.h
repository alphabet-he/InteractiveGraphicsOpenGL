#pragma once
#include "Include.h"

class iApplication
{
protected:
	uint16_t m_windowHeight;
	uint16_t m_windowWidth;
	std::string m_windowTitle;

	GLFWwindow* m_applicationWindow;

	cUserInterfaceSystem* m_UiSystem;

	bool m_running = false;

public:
	iApplication();

	~iApplication();

	void Initialize();

	void Run();

private:
	static iApplication* instance;
	static void _KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void _MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void _MoveCursorCallback(GLFWwindow* window, double xpos, double ypos);

protected:

	virtual void MainLoopFunc();
	virtual void CustomInitialization();
	virtual void ExitApplication();
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	virtual void MoveCursorCallback(GLFWwindow* window, double xpos, double ypos);
};