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

	std::vector<GLfloat> i_vertices;

	for (int i = 0; i < (int)m_meshToRender->NF(); i++) {
		for (int j = 0; j < 3; j++) {
			unsigned int i_vertexInd = m_meshToRender->F(i).v[j];
			unsigned int i_normalInd = m_meshToRender->FN(i).v[j];
			unsigned int i_uvInd = m_meshToRender->FT(i).v[j];

			i_vertices.push_back(m_meshToRender->V(i_vertexInd).x);
			i_vertices.push_back(m_meshToRender->V(i_vertexInd).y);
			i_vertices.push_back(m_meshToRender->V(i_vertexInd).z);

			i_vertices.push_back(m_meshToRender->VN(i_normalInd).x);
			i_vertices.push_back(m_meshToRender->VN(i_normalInd).y);
			i_vertices.push_back(m_meshToRender->VN(i_normalInd).z);

			i_vertices.push_back(m_meshToRender->VT(i_uvInd).x);
			i_vertices.push_back(1.0f - m_meshToRender->VT(i_uvInd).y);
		}
	}

	// Bind VAO (always bind VAO first)
	glBindVertexArray(VAO);

	// Bind VBO and upload vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, i_vertices.size() * sizeof(GLfloat), i_vertices.data(), GL_STATIC_DRAW);

	// Set attributes for position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// Set attributes for normal data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Set attributes for uv coordinate data
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// get and decode texture png
	size_t i_lastSlash = std::string(i_meshPath).find_last_of("/\\");
	std::string directory = (i_lastSlash == std::string::npos) ? "" : std::string(i_meshPath).substr(0, i_lastSlash + 1);

	// diffuse texture
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

	// specular texture
	char* i_textureSpecularFileName = m_meshToRender->M(0).map_Ks.data; // get from mtl
	std::string i_textureSpecular = directory + std::string(i_textureSpecularFileName);
	error = lodepng::decode(image, width, height, i_textureSpecular); // use loadpng to decode it
	if (error) {
		std::cout << "LodePNG decode error " << error << ": " << lodepng_error_text(error) << std::endl;
	}

	// upload the texture to OpenGL
	glGenTextures(1, &specularTex);
	glBindTexture(GL_TEXTURE_2D, specularTex);
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

	delete i_vertexShader;
	delete i_fragmentShader;
}

void iApplication::InitRenderToTexture()
{
	// frame buffer object
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	
	glGenTextures(1, &renderToTex);
	glBindTexture(GL_TEXTURE_2D, renderToTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // BILINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // MIP MAPPING
	float maxAniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso); // Get maximum level
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// attach the screen texture to the frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderToTex, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Framebuffer is not complete! Status: " << status << std::endl;
	}

	// render buffer object
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_windowWidth, m_windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// unbind framebuffer
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// plane obj
	renderToTexturePlane = new cy::TriMesh();
	renderToTexturePlane->LoadFromFileObj("Assets/testplane.obj");
	glGenVertexArrays(1, &RenderToTextureVAO);
	glGenBuffers(1, &RenerToTextureVBO);
	std::vector<GLfloat> i_vertices;

	for (int i = 0; i < (int)renderToTexturePlane->NF(); i++) {
		for (int j = 0; j < 3; j++) {
			unsigned int i_vertexInd = renderToTexturePlane->F(i).v[j];
			unsigned int i_uvInd = renderToTexturePlane->FT(i).v[j];

			i_vertices.push_back(renderToTexturePlane->V(i_vertexInd).x);
			i_vertices.push_back(renderToTexturePlane->V(i_vertexInd).y);
			i_vertices.push_back(renderToTexturePlane->V(i_vertexInd).z);

			i_vertices.push_back(renderToTexturePlane->VT(i_uvInd).x);
			i_vertices.push_back(1.0f - renderToTexturePlane->VT(i_uvInd).y);
		}
	}

	// bind VAO, VBO, attributes
	glBindVertexArray(RenderToTextureVAO);
	glBindBuffer(GL_ARRAY_BUFFER, RenerToTextureVBO);
	glBufferData(GL_ARRAY_BUFFER, i_vertices.size() * sizeof(GLfloat), i_vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0); // position
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // uv
	glEnableVertexAttribArray(1);


	// plane shaders
	cy::GLSLShader* i_vertexShader = new cy::GLSLShader();
	cy::GLSLShader* i_fragmentShader = new cy::GLSLShader();
	i_vertexShader->CompileFile("Assets/RenderToTextureVertexShader.glsl", GL_VERTEX_SHADER);
	i_fragmentShader->CompileFile("Assets/RenderToTextureFragmentShader.glsl", GL_FRAGMENT_SHADER);
	RenderToTextureShaderProgram = glCreateProgram();
	glAttachShader(RenderToTextureShaderProgram, i_vertexShader->GetID());
	glAttachShader(RenderToTextureShaderProgram, i_fragmentShader->GetID());
	glLinkProgram(RenderToTextureShaderProgram);

	delete i_vertexShader;
	delete i_fragmentShader;
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
