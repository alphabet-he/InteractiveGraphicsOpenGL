#include "iApplication.h"

iApplication* iApplication::instance = nullptr;

iApplication::iApplication()
{
}

void iApplication::_DisplayFunc()
{
	instance->DisplayFunc();
}

void iApplication::_IdleFunc()
{
	instance->IdleFunc();
}

void iApplication::_HandleKeyboardFunc(unsigned char key, int x, int y)
{
	instance->HandleKeyboardFunc(key, x, y);
}

void iApplication::Initialize(int argc, char** argv)
{
	instance = this;

	// create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(m_windowWidth, m_windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(m_windowTitle.c_str());

	// initialize glew
	glewInit();

	// set up functions
	glutDisplayFunc(_DisplayFunc);
	glutIdleFunc(_IdleFunc);
	glutKeyboardFunc(_HandleKeyboardFunc);

	CustomInitialization();
}

void iApplication::Run()
{
	m_running = true;
	glutMainLoop();
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
	glAttachShader(ShaderProgram, i_fragmentShader->GetID());
	glLinkProgram(ShaderProgram);

}

void iApplication::DisplayFunc()
{
}

void iApplication::IdleFunc()
{
}

void iApplication::HandleKeyboardFunc(unsigned char key, int x, int y)
{
}

void iApplication::CustomInitialization()
{
}

void iApplication::ExitApplication()
{
	m_running = false;
	glutLeaveMainLoop();
	delete this;
}

iApplication::~iApplication()
{
}
