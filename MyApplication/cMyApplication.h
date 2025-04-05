#pragma once
#include <Application/iApplication.h>
#include <Graphics/Graphics.h>
#include <Assets/cMeshManager.h>

class cMyApplication : public iApplication
{
public:
	cMyApplication();

private:
	void MainLoopFunc() override;
	void CustomInitialization() override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override;

private:
	
	cVertexShaderProgram* m_displayProgram;
	cVertexShaderProgram* m_lightProgram;

	bool m_input_leftMouseButton = false;
	bool m_input_rightMouseButton = false;
	bool m_input_ctrlKey = false;
	double m_input_mouseLocationWhenPressedX = INT_MIN;
	double m_input_mouseLocationWhenPressedY = INT_MIN;
	glm::mat4 m_viewMatWhenPressed;

	glm::mat4 m_viewMat;
	glm::mat4 m_projectionMat;

	glm::vec3 m_lightPosition;

	std::shared_ptr<cMesh> m_lightMesh;

	/*
	* 
	*/
	cMeshSystem* m_meshSystem;

	std::vector<std::string> m_files;
	int m_newMeshSelected = -1;

	sMeshInstance* m_selectedMesh = nullptr;

	enum eTransformation {
		LOCATION_X = 0,
		LOCATION_Y = 1,
		LOCATION_Z = 2, 
		ROTATION_X = 3,
		ROTATION_Y = 4,
		ROTATION_Z = 5,
		SCALE = 6,
		COUNT = 7
	};

	eTransformation m_transformationAction = LOCATION_X;
};

