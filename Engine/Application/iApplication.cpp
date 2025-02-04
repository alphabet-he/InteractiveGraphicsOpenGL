#include "iApplication.h"

iApplication* iApplication::instance = nullptr;

iApplication::iApplication()
{
}

void iApplication::Initialize()
{
	instance = this;

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

	glfwSetKeyCallback(m_applicationWindow, _KeyCallBack);
	glfwSetMouseButtonCallback(m_applicationWindow, _MouseButtonCallback);
	glfwSetCursorPosCallback(m_applicationWindow, _MoveCursorCallback);

	while (m_running) {
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
	glGenBuffers(1, &normalVBO);

	std::vector<GLfloat> i_vertices;
	std::vector<GLuint> i_indices;
	std::vector<GLfloat> i_normals;

	for (int i = 0; i < (int)m_meshToRender->NV(); i++) {
		i_vertices.push_back(m_meshToRender->V(i).x);
		i_vertices.push_back(m_meshToRender->V(i).y);
		i_vertices.push_back(m_meshToRender->V(i).z);
	}
	for (int i = 0; i < (int)m_meshToRender->NF(); i++) {
		i_indices.push_back(m_meshToRender->F(i).v[0]);
		i_indices.push_back(m_meshToRender->F(i).v[1]);
		i_indices.push_back(m_meshToRender->F(i).v[2]);
	}
	for (int i = 0; i < (int)m_meshToRender->NVN(); i++) {
		i_normals.push_back(m_meshToRender->VN(i).x);
		i_normals.push_back(m_meshToRender->VN(i).y);
		i_normals.push_back(m_meshToRender->VN(i).z);
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

	// bind normal buffer and upload normal data
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, i_normals.size() * sizeof(GLfloat), i_normals.data(), GL_STATIC_DRAW);
	
	// set up normal vertex buffer attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

}

void iApplication::LinkShaders(char const* i_vertexShaderFilename, char const* i_fragmentShaderFilename)
{
	if (ShaderProgram) {

		// get the number of attached shaders
		GLint shaderCount = 0;
		glGetProgramiv(ShaderProgram, GL_ATTACHED_SHADERS, &shaderCount);

		// detach all shaders
		if (shaderCount > 0) {
			GLuint* shaders = new GLuint[shaderCount];
			glGetAttachedShaders(ShaderProgram, shaderCount, nullptr, shaders);

			// Detach and delete each shader
			for (int i = 0; i < shaderCount; i++) {
				glDetachShader(ShaderProgram, shaders[i]);
				glDeleteShader(shaders[i]); // Free GPU memory
			}

			delete[] shaders; // Free allocated memory
		}

		glDeleteProgram(ShaderProgram);
	}

	cy::GLSLShader* i_vertexShader = new cy::GLSLShader();
	cy::GLSLShader* i_fragmentShader = new cy::GLSLShader();
	i_vertexShader->CompileFile(i_vertexShaderFilename, GL_VERTEX_SHADER);
	i_fragmentShader->CompileFile(i_fragmentShaderFilename, GL_FRAGMENT_SHADER);
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, i_vertexShader->GetID());
	glAttachShader(ShaderProgram, i_fragmentShader->GetID());
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
	instance = nullptr;
	glfwTerminate();
}

void iApplication::_KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	instance->KeyCallback(window, key, scancode, action, mods);
}

void iApplication::_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	instance->MouseButtonCallback(window, button, action, mods);
}

void iApplication::_MoveCursorCallback(GLFWwindow* window, double xpos, double ypos)
{
	instance->MoveCursorCallback(window, xpos, ypos);
}

void iApplication::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

void iApplication::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void iApplication::MoveCursorCallback(GLFWwindow* window, double xpos, double ypos)
{
}

iApplication::~iApplication()
{
}
