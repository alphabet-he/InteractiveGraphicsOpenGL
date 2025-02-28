#pragma once
#include <Application/iApplication.h>

class cMyApplication : public iApplication
{
public:
	cMyApplication();

private:
	void MainLoopFunc() override;
	void CustomInitialization() override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override;

	void ChangeBackground(double i_deltaTime);

private:
	double m_lastBackgroundChangeTime = 0;

	bool m_input_leftMouseButton = false;
	bool m_input_rightMouseButton = false;
	bool m_input_ctrlKey = false;
	double m_input_mouseLocationWhenPressedX = INT_MIN;
	double m_input_mouseLocationWhenPressedY = INT_MIN;
	glm::mat4 m_viewMatWhenPressed;

	glm::mat4 m_modelMat;
	glm::mat4 m_viewMat;
	glm::mat4 m_projectionMat;

	glm::vec3 m_lightPosition;
};

