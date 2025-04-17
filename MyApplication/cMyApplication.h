#pragma once
#include <Application/iApplication.h>
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

	bool m_input_middleMouseButton = false;
	bool m_input_rightMouseButton = false;
	double m_input_mouseLocationWhenPressedX = INT_MIN;
	double m_input_mouseLocationWhenPressedY = INT_MIN;
	glm::mat4 m_viewMatWhenPressed;

	glm::mat4 m_viewMat;
	glm::mat4 m_projectionMat;

	std::weak_ptr<cMesh> m_playerMesh;

	std::vector<std::pair<std::weak_ptr<cMesh>, glm::vec3>> m_lights;

	/*
	* 
	*/
	cMeshSystem* m_meshSystem;

	std::vector<std::pair<std::string, glm::mat4>> m_fileNameModelPairs;
	std::vector<std::string> m_playerSprites;
	int m_newMeshSelected = -1;
	int m_newPlayerSpriteSelected = -1;
	glm::vec3 m_lightingColorPicked = glm::vec3(-1, -1, -1);

	sMeshInstance* m_selectedMesh = nullptr;

	eTransformation m_transformationAction = COUNT;

	glm::mat4 m_spritePlaneModelMat;
	glm::mat4 m_lightModelMat;

	/*
	* ui elements
	*/
	sPanel* m_meshSelectionPanel;
	sPanel* m_spriteSelectionPanel;
	sPanel* m_lightingPanel;
	sRgbPicker* m_lightingRgbPicker;
};

struct sMeshSelectionButton :sImageButton {
	int m_selectionInd = -1;
	sMeshSelectionButton(float i_positionX, float i_positionY,
		float i_sizeX, float i_sizeY,
		GLuint i_textureID,
		int i_selectionInd,
		std::function<void()> i_callback)
		: sImageButton(i_positionX, i_positionY, i_sizeX, i_sizeY, i_textureID, i_callback),
		m_selectionInd(i_selectionInd) {}
};

struct sTransformationButton :sImageButton {
	eTransformation m_transformation = COUNT;
	sTransformationButton(float i_positionX, float i_positionY,
		float i_sizeX, float i_sizeY,
		GLuint i_textureID,
		int i_transformation,
		std::function<void()> i_callback)
		: sImageButton(i_positionX, i_positionY, i_sizeX, i_sizeY, i_textureID, i_callback),
		m_transformation(static_cast<eTransformation>(i_transformation)) {}
};

