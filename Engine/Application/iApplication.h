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

	bool m_running = false;

public:
	iApplication();

	~iApplication();

	void Initialize(int argc, char** argv);

	void Run();

private:
	static iApplication* instance;
	static void _DisplayFunc();
	static void _IdleFunc();
	static void _HandleKeyboardFunc(unsigned char key, int x, int y);

protected:
	void UploadTriMeshVertices();

	void LinkShaders(char const* i_vertexShaderFilename, char const* i_fragmentShaderFilename);

	virtual void DisplayFunc();
	virtual void IdleFunc();
	virtual void HandleKeyboardFunc(unsigned char key, int x, int y);
	virtual void CustomInitialization();
	virtual void ExitApplication();
};