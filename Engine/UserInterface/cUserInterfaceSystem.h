#pragma once
#include "Include.h"

struct sUIComponent {
	bool b_active = true;

	ImVec2 m_position;
	ImVec2 m_size;
	ImTextureID m_texture;

	sUIComponent()
		: m_position(0, 0), m_size(0, 0), m_texture(-1) {}

	sUIComponent(float i_positionX, float i_positionY,
		float i_sizeX, float i_sizeY,
		GLuint i_textureID)
		: m_position(i_positionX, i_positionY),
		m_size(i_sizeX, i_sizeY),
		m_texture(ImTextureID(i_textureID)) {}

	virtual void Render() = 0;
};

struct sButton :sUIComponent {
	std::function<void()> m_callback;
	std::string m_text;
	sButton(float i_positionX, float i_positionY,
		float i_sizeX, float i_sizeY,
		std::string i_text,
		std::function<void()> i_callback)
		: sUIComponent(i_positionX, i_positionY, i_sizeX, i_sizeY, -1),
		m_text(i_text),
		m_callback(std::move(i_callback)) {}

	void Render() override;
};

struct sImageButton : sUIComponent {
	std::function<void()> m_callback;
	sImageButton(float i_positionX, float i_positionY,
		float i_sizeX, float i_sizeY,
		GLuint i_textureID,
		std::function<void()> i_callback)
		: sUIComponent(i_positionX, i_positionY, i_sizeX, i_sizeY, i_textureID),
		m_callback(std::move(i_callback)) {}

	void Render() override;
};

struct sRgbPicker : sUIComponent {
	sRgbPicker(float i_positionX, float i_positionY,
		float i_size) :
		sUIComponent(i_positionX, i_positionY, i_size, i_size, -1){}

	float m_color[3] = { 0.0f, 0.0f, 0.0f };
	void Render() override;
};

struct sPanel {
	bool b_active = true;

	const char* m_name;
	ImVec2 m_position;
	ImVec2 m_size;
	ImVec4 m_color;
	
	std::vector<sUIComponent*> m_components;

	sPanel()
		: m_name(""), m_position(0, 0), m_size(0, 0), m_color(0, 0, 0, 0) {};
	sPanel(const char* i_name, float i_positionX, float i_positionY,
		float i_sizeX, float i_sizeY,
		float i_colorR, float i_colorG, float i_colorB, float i_colorA)
		: m_name(i_name),
		m_position(i_positionX, i_positionY),
		m_size(i_sizeX, i_sizeY),
		m_color(i_colorR, i_colorG, i_colorB, i_colorA) {};
};

class cUserInterfaceSystem
{
private:
	GLFWwindow* m_window;
	std::vector<sPanel*> m_panels;

public:
	cUserInterfaceSystem(GLFWwindow* i_window, const char* i_openglVersion);
	void Destroy();
	inline void AddPanel(sPanel* i_panel) {
		m_panels.push_back(i_panel);
	}
	void DrawUI();
};

