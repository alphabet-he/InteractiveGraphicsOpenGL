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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	while (m_running) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		MainLoopFunc();
		glfwPollEvents();
		glfwSwapBuffers(m_applicationWindow);
	}
}

void iApplication::UploadTriMesh(char const* i_meshPath)
{
	m_meshToRender->LoadFromFileObj(i_meshPath);

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


	// generate vbo for UV coordinates
	glGenBuffers(1, &uvVBO);

	// upload uv coordinate data
	std::vector<GLfloat> i_uvCoordinate;
	for (int i = 0; i < m_meshToRender->NF(); i++) {
		for (int j = 0; j < 3; j++) {
			unsigned int i_uvInd = m_meshToRender->FT(i).v[j];

			i_uvCoordinate.push_back(m_meshToRender->VT(i_uvInd).x);
			i_uvCoordinate.push_back(1.0 - m_meshToRender->VT(i_uvInd).y);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
	glBufferData(GL_ARRAY_BUFFER, i_uvCoordinate.size() * sizeof(GLfloat), i_uvCoordinate.data(), GL_STATIC_DRAW);

	// set up uv coordinate array attributes
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	// get and decode texture png
	size_t i_lastSlash = std::string(i_meshPath).find_last_of("/\\");
	std::string directory = (i_lastSlash == std::string::npos) ? "" : std::string(i_meshPath).substr(0, i_lastSlash + 1);

	char* i_textureDiffuseFileName = m_meshToRender->M(0).map_Kd.data; // get from mtl
	std::string i_textureDiffuse = directory + std::string(i_textureDiffuseFileName);
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, i_textureDiffuse); // use loadpng to decode it
	if (error) {
		std::cout << "LodePNG decode error " << error << ": " << lodepng_error_text(error) << std::endl;
	}

	// upload the texture to OpenGL
	glGenTextures(1, &diffuseTex);
	glBindTexture(GL_TEXTURE_2D, diffuseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

	// set paramaters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
