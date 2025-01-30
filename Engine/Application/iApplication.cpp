#include "iApplication.h"


iApplication::iApplication()
{
}

void iApplication::Initialize()
{

	// initialize glfw
	glfwInit();

	// create window
	m_applicationWindow = glfwCreateWindow(m_windowWidth, m_windowHeight, m_windowTitle.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(m_applicationWindow);

	glfwSwapInterval(1);

	// initialize glew
	glewInit();

	CustomInitialization();
}

void iApplication::Run()
{
	m_running = true;

	//glfwSetKeyCallback(m_applicationWindow, DisplayFunc);

	while (!glfwWindowShouldClose(m_applicationWindow)) {
		MainLoopFunc();
		glfwPollEvents();
		glfwSwapBuffers(m_applicationWindow);
	}
}

void iApplication::UploadTriMeshVertices()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	std::vector<GLfloat> i_vertices;
	std::vector<GLuint> i_indices;

	for (int i = 0; i < m_meshToRender->NV(); i++) {
		i_vertices.push_back(m_meshToRender->V(i).x);
		i_vertices.push_back(m_meshToRender->V(i).y);
		i_vertices.push_back(m_meshToRender->V(i).z);
	}
	for (int i = 0; i < m_meshToRender->NF(); i++) {
		i_indices.push_back(m_meshToRender->F(i).v[0]);
		i_indices.push_back(m_meshToRender->F(i).v[1]);
		i_indices.push_back(m_meshToRender->F(i).v[2]);
	}

	// Bind VAO (always bind VAO first)
	glBindVertexArray(VAO);

	// Bind VBO and upload vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, i_vertices.size() * sizeof(GLfloat), i_vertices.data(), GL_STATIC_DRAW);

	// Set up vertex attributes *before* binding EBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// Now bind and upload index buffer (EBO)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_indices.size() * sizeof(GLuint), i_indices.data(), GL_STATIC_DRAW);

}

void iApplication::LinkShaders(char const* i_vertexShaderFilename, char const* i_fragmentShaderFilename)
{
	cy::GLSLShader* i_vertexShader = new cy::GLSLShader();
	cy::GLSLShader* i_fragmentShader = new cy::GLSLShader();
	i_vertexShader->CompileFile(i_vertexShaderFilename, GL_VERTEX_SHADER);
	i_fragmentShader->CompileFile(i_fragmentShaderFilename, GL_FRAGMENT_SHADER);
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, i_vertexShader->GetID());
	//glAttachShader(ShaderProgram, i_fragmentShader->GetID());
	glLinkProgram(ShaderProgram);
}

void iApplication::MainLoopFunc()
{
}

void iApplication::CustomInitialization()
{
}

void iApplication::ExitApplication()
{
	m_running = false;
	glfwTerminate();
}

iApplication::~iApplication()
{
}
