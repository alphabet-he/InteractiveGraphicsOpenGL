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
	GLuint VAO, VBO, EBO;
	GLFWwindow* m_applicationWindow;

	bool m_running = false;

public:
	iApplication();

	~iApplication();

	void Initialize();

	void Run();

protected:
	void UploadTriMeshVertices();

	void LinkShaders(char const* i_vertexShaderFilename, char const* i_fragmentShaderFilename);

	virtual void MainLoopFunc();
	virtual void CustomInitialization();
	virtual void ExitApplication();
};