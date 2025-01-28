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

	// generate VAO and VBO
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// bind VAO
	glBindVertexArray(VAO);

	// bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// configure VAO as positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

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

void iApplication::UploadTriMeshVertices(cyTriMesh* i_mesh)
{
	GLfloat* vertices = new GLfloat[i_mesh->NV() * 3];
	for (int i = 0; i < i_mesh->NV(); i++) {
		vertices[i * 3 + 0] = i_mesh->V(i).x;
		vertices[i * 3 + 1] = i_mesh->V(i).y;
		vertices[i * 3 + 2] = i_mesh->V(i).z;
	}

	// upload VBO data
	glBufferData(GL_ARRAY_BUFFER,
		i_mesh->NV() * 3 * sizeof(GLfloat),
		vertices,
		GL_STATIC_DRAW);

	// the data is already uploaded to GPU, so vertices can be freed
	delete[] vertices;
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
