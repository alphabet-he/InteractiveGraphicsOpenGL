#pragma once
#include <Application/iApplication.h>
#include <Assets/cMeshManager.h>

class cViewerApplication : public iApplication
{
private:
	void CustomInitialization() override;
	void MainLoopFunc() override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override;

private:

	cVertexShaderProgram* m_displayProgram;

	bool m_input_leftMouseButton = false;
	bool m_input_rightMouseButton = false;
	double m_input_mouseLocationWhenPressedX = INT_MIN;
	double m_input_mouseLocationWhenPressedY = INT_MIN;
	glm::mat4 m_viewMatWhenPressed;

	glm::mat4 m_viewMat;
	glm::mat4 m_projectionMat;

	glm::vec3 m_lightPos;

	cMeshSystem* m_meshSystem;
};