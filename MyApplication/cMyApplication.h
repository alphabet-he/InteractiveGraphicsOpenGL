#pragma once
#include <Application/iApplication.h>
#include <Graphics/Graphics.h>
#include <Assets/cMeshManager.h>

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

class cMyApplication : public iApplication
{
public:
	cMyApplication();

private:
	void MainLoopFunc() override;
	void CustomInitialization() override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override;

	void Export();

private:
	
	cVertexShaderProgram* m_displayProgram;
	cVertexShaderProgram* m_lightProgram;

	bool m_input_leftMouseButton = false;
	bool m_input_rightMouseButton = false;
	double m_input_mouseLocationWhenPressedX = INT_MIN;
	double m_input_mouseLocationWhenPressedY = INT_MIN;
	glm::mat4 m_viewMatWhenPressed;

	glm::mat4 m_viewMat;
	glm::mat4 m_projectionMat;

	std::weak_ptr<cMesh> m_lightMesh;

	/*
	* 
	*/
	cMeshSystem* m_meshSystem;
	cMeshSystem* m_tempMeshSystem;

	std::vector<std::pair<std::string, glm::mat4>> m_fileNameModelPairs;
	int m_newMeshSelected = -1;

	sMeshInstance* m_selectedMesh = nullptr;

	eTransformation m_transformationAction = COUNT;

};

struct sMeshSelectionButton :sButton {
	int m_selectionInd = -1;
	sMeshSelectionButton(float i_positionX, float i_positionY,
		float i_sizeX, float i_sizeY,
		GLuint i_textureID,
		int i_selectionInd,
		std::function<void()> i_callback)
		: sButton(i_positionX, i_positionY, i_sizeX, i_sizeY, i_textureID, i_callback),
		m_selectionInd(i_selectionInd) {}
};

struct sTransformationButton :sButton {
	eTransformation m_transformation = COUNT;
	sTransformationButton(float i_positionX, float i_positionY,
		float i_sizeX, float i_sizeY,
		GLuint i_textureID,
		int i_transformation,
		std::function<void()> i_callback)
		: sButton(i_positionX, i_positionY, i_sizeX, i_sizeY, i_textureID, i_callback),
		m_transformation(static_cast<eTransformation>(i_transformation)) {}
};

