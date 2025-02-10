#pragma once
#include "Include.h"

class iApplication
{
protected:
	uint16_t m_windowHeight;
	uint16_t m_windowWidth;
	std::string m_windowTitle;
	GLuint ShaderProgram;
	cy::TriMesh* m_meshToRender;
	// basic mesh
	GLuint VAO, VBO;
	// textures
	GLuint diffuseTex, specularTex;
	GLFWwindow* m_applicationWindow;

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
	void UploadTriMesh(char const* i_meshPath);

	void LinkShaders(char const* i_vertexShaderFilename, char const* i_fragmentShaderFilename);

	virtual void MainLoopFunc();
	virtual void CustomInitialization();
	virtual void ExitApplication();
	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	virtual void MoveCursorCallback(GLFWwindow* window, double xpos, double ypos);
};