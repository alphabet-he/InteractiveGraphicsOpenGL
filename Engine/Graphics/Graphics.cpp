#include "Graphics.h"

void cMesh::UploadADSTexture(eTextureUsageFlags i_textureUsage, std::string i_textureDir)
{
	const char* i_textureFileName;
	switch (i_textureUsage)
	{
	case AMBIENT:
		i_textureFileName = m_cyMesh->M(0).map_Ka.data;
		break;
	case DIFFUSE:
		i_textureFileName = m_cyMesh->M(0).map_Kd.data;
		break;
	case SPECULAR:
		i_textureFileName = m_cyMesh->M(0).map_Ks.data;
		break;
	default:
		i_textureFileName = "";
		break;
	}
	std::string i_texture = i_textureDir + std::string(i_textureFileName);
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, i_texture); // use loadpng to decode it
	if (error) {
		std::cout << "LodePNG decode error " << error << ": " << lodepng_error_text(error) << std::endl;
	}

	// upload the texture to OpenGL
	GLuint TexInt;
	glGenTextures(1, &TexInt);
	glBindTexture(GL_TEXTURE_2D, TexInt);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

	// set paramaters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	m_textureBinding.push_back(std::pair<GLuint, eTextureUsageFlags>(TexInt, i_textureUsage));
}

cVertexShaderProgram::cVertexShaderProgram()
{
	//TODO: Abstract an actual parent class
}

cVertexShaderProgram::cVertexShaderProgram(sVertexBufferStruct* i_vertexBufferStruct)
{
	m_vertexBufferStruct = i_vertexBufferStruct;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	m_shaderProgram = glCreateProgram();

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	m_bufferSize = 2 * 1024 * 1024;

	// Allocate buffer storage with Persistent Mapping & Coherent flags
	glBufferStorage(GL_ARRAY_BUFFER, m_bufferSize, NULL,
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	// Map the buffer once and keep it mapped
	m_mappedBuffer = (GLfloat*)glMapBufferRange(GL_ARRAY_BUFFER, 0, m_bufferSize,
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	// Track offset
	m_VBOOffset = 0;
	m_VBOVerticeOffset = 0;

	GLuint i_attributeIndex = 0;
	size_t i_stride = 0;

	// Compute stride
	if (m_vertexBufferStruct->hasAttribute(POSITION))  i_stride += 3 * sizeof(GLfloat);
	if (m_vertexBufferStruct->hasAttribute(NORMAL))    i_stride += 3 * sizeof(GLfloat);
	if (m_vertexBufferStruct->hasAttribute(TEXCOORD))  i_stride += 2 * sizeof(GLfloat);

	// set attributes
	size_t offset = 0;
	if (m_vertexBufferStruct->hasAttribute(POSITION)) {
		glVertexAttribPointer(i_attributeIndex, 3, GL_FLOAT, GL_FALSE, i_stride, (void*)0);
		glEnableVertexAttribArray(i_attributeIndex);
		i_attributeIndex++;
		offset += 3;
	}
	if (m_vertexBufferStruct->hasAttribute(NORMAL)) {
		glVertexAttribPointer(i_attributeIndex, 3, GL_FLOAT, GL_FALSE, i_stride, (void*)(offset * sizeof(GLfloat)));
		glEnableVertexAttribArray(i_attributeIndex);
		i_attributeIndex++;
		offset += 3;
	}
	if (m_vertexBufferStruct->hasAttribute(TEXCOORD)) {
		glVertexAttribPointer(i_attributeIndex, 2, GL_FLOAT, GL_FALSE, i_stride, (void*)(offset * sizeof(GLfloat)));
		glEnableVertexAttribArray(i_attributeIndex);
		i_attributeIndex++;
		offset += 2;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

cMesh* cVertexShaderProgram::UploadMesh(const char* i_meshObjPath, sTextureUsage* i_textureUsage)
{
	cMesh* i_mesh = new cMesh();
	i_mesh->m_cyMesh = new cy::TriMesh();
	i_mesh->m_cyMesh->LoadFromFileObj(i_meshObjPath);

	std::vector<GLfloat> i_vertices;

	for (int i = 0; i < (int)i_mesh->m_cyMesh->NF(); i++) {
		for (int j = 0; j < 3; j++) {
			if (m_vertexBufferStruct->hasAttribute(POSITION)) {
				unsigned int i_vertexInd = i_mesh->m_cyMesh->F(i).v[j];
				i_vertices.push_back(i_mesh->m_cyMesh->V(i_vertexInd).x);
				i_vertices.push_back(i_mesh->m_cyMesh->V(i_vertexInd).y);
				i_vertices.push_back(i_mesh->m_cyMesh->V(i_vertexInd).z);
			}

			if (m_vertexBufferStruct->hasAttribute(NORMAL)) {
				unsigned int i_normalInd = i_mesh->m_cyMesh->FN(i).v[j];
				i_vertices.push_back(i_mesh->m_cyMesh->VN(i_normalInd).x);
				i_vertices.push_back(i_mesh->m_cyMesh->VN(i_normalInd).y);
				i_vertices.push_back(i_mesh->m_cyMesh->VN(i_normalInd).z);
			}

			if (m_vertexBufferStruct->hasAttribute(TEXCOORD)) {
				unsigned int i_uvInd = i_mesh->m_cyMesh->FT(i).v[j];
				i_vertices.push_back(i_mesh->m_cyMesh->VT(i_uvInd).x);
				i_vertices.push_back(1.0f - i_mesh->m_cyMesh->VT(i_uvInd).y);
			}
		}
	}

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	// upload data to the mapped buffer
	size_t i_dataSize = i_vertices.size() * sizeof(GLfloat);

	if (!m_mappedBuffer) {
		std::cerr << "ERROR: Persistent buffer not initialized!" << std::endl;
		return nullptr;
	}

	if (m_VBOOffset + i_dataSize > m_bufferSize) { // BUFFER_SIZE should be total allocated buffer size
		std::cerr << "ERROR: Not enough buffer space for mesh data!" << std::endl;
		// TODO: expand the buffer
		return nullptr;
	}

	memcpy(m_mappedBuffer + m_VBOOffset, i_vertices.data(), i_dataSize);

	i_mesh->m_bufferOffset = m_VBOVerticeOffset;
	m_VBOOffset += i_dataSize/sizeof(GLfloat);
	m_VBOVerticeOffset += i_mesh->m_cyMesh->NF() * 3;


	// upload textures
	if (i_textureUsage->useAnyTexture()) {
		// get and decode texture png
		size_t i_lastSlash = std::string(i_meshObjPath).find_last_of("/\\");
		std::string directory = (i_lastSlash == std::string::npos) ? "" : std::string(i_meshObjPath).substr(0, i_lastSlash + 1);

		// ambient
		if (i_textureUsage->useTexture(AMBIENT)) {
			i_mesh->UploadADSTexture(AMBIENT, directory);
		}
		// diffuse
		if (i_textureUsage->useTexture(DIFFUSE)) {
			i_mesh->UploadADSTexture(DIFFUSE, directory);
		}
		// specular
		if (i_textureUsage->useTexture(SPECULAR)) {
			i_mesh->UploadADSTexture(SPECULAR, directory);
		}
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_meshes.push_back(i_mesh);
	return i_mesh;
}

void cVertexShaderProgram::LinkShaders(char const* i_vertexShaderFilename, char const* i_fragmentShaderFilename)
{

	// get the number of attached shaders
	GLint shaderCount = 0;
	glGetProgramiv(m_shaderProgram, GL_ATTACHED_SHADERS, &shaderCount);

	// detach all shaders
	if (shaderCount > 0) {
		GLuint* shaders = new GLuint[shaderCount];
		glGetAttachedShaders(m_shaderProgram, shaderCount, nullptr, shaders);

		// Detach and delete each shader
		for (int i = 0; i < shaderCount; i++) {
			glDetachShader(m_shaderProgram, shaders[i]);
			glDeleteShader(shaders[i]); // Free GPU memory
		}

		delete[] shaders; // Free allocated memory
	}

	glDeleteProgram(m_shaderProgram);

	cy::GLSLShader* i_vertexShader = new cy::GLSLShader();
	cy::GLSLShader* i_fragmentShader = new cy::GLSLShader();
	i_vertexShader->CompileFile(i_vertexShaderFilename, GL_VERTEX_SHADER);
	i_fragmentShader->CompileFile(i_fragmentShaderFilename, GL_FRAGMENT_SHADER);
	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, i_vertexShader->GetID());
	glAttachShader(m_shaderProgram, i_fragmentShader->GetID());
	glLinkProgram(m_shaderProgram);

	glUseProgram(m_shaderProgram);
	m_textureKa = glGetUniformLocation(m_shaderProgram, "texture_Ka");
	m_textureKd = glGetUniformLocation(m_shaderProgram, "texture_Kd");
	m_textureKs = glGetUniformLocation(m_shaderProgram, "texture_Ks");
	m_textureSkyboxReflection = glGetUniformLocation(m_shaderProgram, "skybox");

	m_shaderModelMat = glGetUniformLocation(m_shaderProgram, "model");
	m_shaderViewMat = glGetUniformLocation(m_shaderProgram, "view");
	m_shaderProjectionMat = glGetUniformLocation(m_shaderProgram, "projection");

	m_shaderCameraPosition = glGetUniformLocation(m_shaderProgram, "camera_position");
	m_shaderLightingPosition = glGetUniformLocation(m_shaderProgram, "light_position");

	delete i_vertexShader;
	delete i_fragmentShader;
}

void cVertexShaderProgram::SetMVPMatrix(glm::mat4 i_matrix, eMVPMatrixFlags i_matrixName)
{
	glUseProgram(m_shaderProgram);
	switch (i_matrixName) 
	{
	case MODEL:
		glUniformMatrix4fv(m_shaderModelMat, 1, GL_FALSE, glm::value_ptr(i_matrix));
		break;
	case VIEW:
		glUniformMatrix4fv(m_shaderViewMat, 1, GL_FALSE, glm::value_ptr(i_matrix));
		break;
	case PROJECTION:
		glUniformMatrix4fv(m_shaderProjectionMat, 1, GL_FALSE, glm::value_ptr(i_matrix));
		break;
	}
}

void cVertexShaderProgram::SetCameraPosition(glm::vec3 i_cameraPos)
{
	glUseProgram(m_shaderProgram);
	glUniform3f(m_shaderCameraPosition, i_cameraPos.x, i_cameraPos.y, i_cameraPos.z);
}

void cVertexShaderProgram::SetLightingPosition(glm::vec3 i_lightPos)
{
	glUseProgram(m_shaderProgram);
	glUniform3f(m_shaderLightingPosition, i_lightPos.x, i_lightPos.y, i_lightPos.z);
}

void cVertexShaderProgram::DrawCall()
{
	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_VAO);

	for (cMesh* i_mesh : m_meshes) {

		int i_textureUnit = 0;

		for (const auto& i_texBinding : i_mesh->m_textureBinding) {

			glActiveTexture(GL_TEXTURE0 + i_textureUnit);
			switch (i_texBinding.second) {
			case AMBIENT:
				glBindTexture(GL_TEXTURE_2D, i_texBinding.first);
				glUniform1i(m_textureKa, i_textureUnit);
				break;
			case DIFFUSE:
				glBindTexture(GL_TEXTURE_2D, i_texBinding.first);
				glUniform1i(m_textureKd, i_textureUnit);
				break;
			case SPECULAR:
				glBindTexture(GL_TEXTURE_2D, i_texBinding.first);
				glUniform1i(m_textureKs, i_textureUnit);
				break;
			case SKYBOX_REFLECTION:
				glBindTexture(GL_TEXTURE_CUBE_MAP, i_texBinding.first);
				glUniform1i(m_textureSkyboxReflection, i_textureUnit);
				break;
			case SCREEN_TEXTURE:
				if (m_screenTextureInfo) {
					glBindTexture(GL_TEXTURE_2D, i_texBinding.first);
					glUniform1i(m_screenTextureInfo->m_screenTextureTexPosition, i_textureUnit);
					GLenum err;
					while ((err = glGetError()) != GL_NO_ERROR) {
						std::cerr << "OpenGL ERROR: " << err << std::endl;
					}
				}
				else {
					std::cerr << "ERROR: No screen reflection information" << std::endl;
				}
				break;
			case SHADOW_MAP:
				if (m_shadowMapInfo) {
					glBindTexture(GL_TEXTURE_2D, i_texBinding.first);
					glUniform1i(m_shadowMapInfo->m_shadowMapTexPosition, i_textureUnit);
				}
				else {
					std::cerr << "ERROR: No shadow map information" << std::endl;
				}
				break;
			}

			i_textureUnit++;
		}

		glUniformMatrix4fv(m_shaderModelMat, 1, GL_FALSE, glm::value_ptr(i_mesh->m_modelMat));
		glDrawArrays(GL_TRIANGLES, i_mesh->m_bufferOffset, i_mesh->m_cyMesh->NF() * 3);
	}
	glBindVertexArray(0);
}

void cVertexShaderProgram::InitializeScreenTexture(uint16_t i_width, uint16_t i_height)
{
	m_screenTextureInfo = new sScreenTextureInfo();

	m_screenTextureInfo->m_textureWidth = i_width;
	m_screenTextureInfo->m_textureHeight = i_height;

	// frame buffer object
	glGenFramebuffers(1, &(m_screenTextureInfo->m_FBO));
	glBindFramebuffer(GL_FRAMEBUFFER, m_screenTextureInfo->m_FBO);

	glGenTextures(1, &(m_screenTextureInfo->m_texture));
	glBindTexture(GL_TEXTURE_2D, m_screenTextureInfo->m_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_width, i_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // BILINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // MIP MAPPING
	float maxAniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso); // Get maximum level
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// attach the screen texture to the frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_screenTextureInfo->m_texture, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Framebuffer is not complete! Status: " << status << std::endl;
	}

	// render buffer object
	glGenRenderbuffers(1, &(m_screenTextureInfo->m_RBO));
	glBindRenderbuffer(GL_RENDERBUFFER, m_screenTextureInfo->m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, i_width, i_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_screenTextureInfo->m_RBO);

	// unbind framebuffer
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(m_shaderProgram);
	m_screenTextureInfo->m_screenTextureTexPosition = glGetUniformLocation(m_shaderProgram, "screen_texture");
}

GLuint cVertexShaderProgram::RenderToScreenTexture(glm::vec3 i_cameraLocation, glm::vec3 i_faceDirection, std::vector<cVertexShaderProgram*> i_programsToDraw)
{
	if (!m_screenTextureInfo) {
		std::cout << "ERROR: Render to screen texture not initialized!" << std::endl;
		return 0;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_screenTextureInfo->m_FBO);

	glViewport(0, 0, m_screenTextureInfo->m_textureWidth, m_screenTextureInfo->m_textureHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 i_viewMatrix = glm::lookAt(i_cameraLocation, i_cameraLocation + i_faceDirection, glm::vec3(0.0f, 1.0f, 0.0f));

	for (cVertexShaderProgram* i_program : i_programsToDraw) {
		i_program->SetMVPMatrix(i_viewMatrix, VIEW);
		i_program->DrawCall();
	}
	// generate mipmaps
	glBindTexture(GL_TEXTURE_2D, m_screenTextureInfo->m_texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	for (cMesh* i : m_meshes) {
		i->UploadTexture(m_screenTextureInfo->m_texture, SCREEN_TEXTURE);
	}

	return m_screenTextureInfo->m_texture;
}

void cVertexShaderProgram::InitializeShadowMap(uint16_t i_width, uint16_t i_height)
{
	m_shadowMapInfo = new sShadowMapInfo();

	m_shadowMapInfo->m_textureWidth = i_width;
	m_shadowMapInfo->m_textureHeight = i_height;

	// frame buffer object
	glGenFramebuffers(1, &(m_shadowMapInfo->m_FBO));
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapInfo->m_FBO);

	glGenTextures(1, &(m_shadowMapInfo->m_texture));
	glBindTexture(GL_TEXTURE_2D, m_shadowMapInfo->m_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, i_width, i_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // nearest filtering

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // clamp to border and set border value
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// bind depth texture to fbo
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMapInfo->m_texture, 0);

	// unbind frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// create shader program for shadow map
	cy::GLSLShader* i_vertexShader = new cy::GLSLShader();
	cy::GLSLShader* i_fragmentShader = new cy::GLSLShader();
	i_vertexShader->CompileFile("Assets/shader/StandardVertexShader.glsl", GL_VERTEX_SHADER);
	i_fragmentShader->CompileFile("Assets/shader/StandardFragmentShader.glsl", GL_FRAGMENT_SHADER);
	m_shadowMapInfo->m_shadowShaderProgram = glCreateProgram();
	glAttachShader(m_shadowMapInfo->m_shadowShaderProgram, i_vertexShader->GetID());
	glAttachShader(m_shadowMapInfo->m_shadowShaderProgram, i_fragmentShader->GetID());
	glLinkProgram(m_shadowMapInfo->m_shadowShaderProgram);
	delete i_vertexShader;
	delete i_fragmentShader;

	glUseProgram(m_shaderProgram);
	m_shadowMapInfo->m_shadowMapTexPosition = glGetUniformLocation(m_shaderProgram, "shadow_map");

	glUseProgram(m_shadowMapInfo->m_shadowShaderProgram);
	m_shadowMapInfo->m_shadowShaderModelMat = glGetUniformLocation(m_shadowMapInfo->m_shadowShaderProgram, "model");
	m_shadowMapInfo->m_shadowShaderViewMat = glGetUniformLocation(m_shadowMapInfo->m_shadowShaderProgram, "view");
	m_shadowMapInfo->m_shadowShaderProjMat = glGetUniformLocation(m_shadowMapInfo->m_shadowShaderProgram, "projection");
}

GLuint cVertexShaderProgram::RenderShadowMap(glm::vec3 i_lightLocation, glm::vec3 i_targetLocation, 
	float i_lightConeAgnle, float i_lightingNearPlane, float i_lightingFarPlane)
{
	if (!m_shadowMapInfo) {
		std::cout << "ERROR: Shadow map not initialized!" << std::endl;
		return 0;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapInfo->m_FBO);
	
	glUseProgram(m_shadowMapInfo->m_shadowShaderProgram);

	glViewport(0, 0, m_shadowMapInfo->m_textureWidth, m_shadowMapInfo->m_textureHeight);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 i_viewMatrix = glm::lookAt(i_lightLocation, i_targetLocation, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 i_projMatrix = glm::perspective(glm::radians(i_lightConeAgnle), 
		(float)m_shadowMapInfo->m_textureWidth / (float)m_shadowMapInfo->m_textureHeight,
		i_lightingNearPlane, i_lightingFarPlane);

	glUniformMatrix4fv(m_shadowMapInfo->m_shadowShaderViewMat, 1, GL_FALSE, glm::value_ptr(i_viewMatrix));
	glUniformMatrix4fv(m_shadowMapInfo->m_shadowShaderProjMat, 1, GL_FALSE, glm::value_ptr(i_projMatrix));

	glBindVertexArray(m_VAO);
	for (cMesh* i_mesh : m_meshes) {
		glUniformMatrix4fv(m_shadowMapInfo->m_shadowShaderModelMat, 1, GL_FALSE, glm::value_ptr(i_mesh->m_modelMat));
		glDrawArrays(GL_TRIANGLES, i_mesh->m_bufferOffset, i_mesh->m_cyMesh->NF() * 3);
	}
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(m_shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "lightSpaceVP"), 1, GL_FALSE, 
		glm::value_ptr(i_projMatrix * i_viewMatrix));

	for (cMesh* i : m_meshes) {
		i->UploadTexture(m_shadowMapInfo->m_texture, SHADOW_MAP);
	}

	return m_shadowMapInfo->m_texture;
}

GLuint cVertexShaderProgram::RenderShadowMap(glm::vec3 i_lightDirection,
	float i_orthoSize, float i_lightingNearPlane, float i_lightingFarPlane)
{
	if (!m_shadowMapInfo) {
		std::cout << "ERROR: Shadow map not initialized!" << std::endl;
		return 0;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapInfo->m_FBO);

	glViewport(0, 0, m_shadowMapInfo->m_textureWidth, m_shadowMapInfo->m_textureHeight);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 i_viewMatrix = glm::lookAt(-glm::normalize(i_lightDirection) * 10.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 i_projMatrix = glm::ortho(-i_orthoSize, i_orthoSize, -i_orthoSize, i_orthoSize, i_lightingNearPlane, i_lightingFarPlane);

	SetMVPMatrix(i_viewMatrix, VIEW);
	SetMVPMatrix(i_projMatrix, PROJECTION);
	DrawCall();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return m_screenTextureInfo->m_texture;
}

cEnvironmentShaderProgram::cEnvironmentShaderProgram()
{
	m_environmentCube = new cy::TriMesh();
	m_environmentCube->LoadFromFileObj("Assets/background/cube.obj");

	std::vector<GLfloat> i_vertices;

	for (int i = 0; i < (int)m_environmentCube->NF(); i++) {
		for (int j = 0; j < 3; j++) {
			unsigned int i_vertexInd = m_environmentCube->F(i).v[j];
			i_vertices.push_back(m_environmentCube->V(i_vertexInd).x);
			i_vertices.push_back(m_environmentCube->V(i_vertexInd).y);
			i_vertices.push_back(m_environmentCube->V(i_vertexInd).z);
		}
	}

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	m_shaderProgram = glCreateProgram();

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, i_vertices.size() * sizeof(GLfloat), i_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	LinkShaders("Assets/background/EnvCubeVertexShader.glsl", "Assets/background/EnvCubeFragmentShader.glsl");
}

bool cEnvironmentShaderProgram::UploadEnvironmentTexture(std::vector<std::string> i_fileNames)
{
	if (i_fileNames.size() != 6) return false;
	glGenTextures(1, &m_environmentTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentTexture);

	for (GLuint i = 0; i < 6; i++) {
		std::vector<unsigned char> image;
		unsigned width, height;
		unsigned error = lodepng::decode(image, width, height, i_fileNames[i]); // use loadpng to decode it
		if (error) {
			std::cout << "LodePNG decode error " << error << ": " << lodepng_error_text(error) << std::endl;
			return false;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return true;
}

void cEnvironmentShaderProgram::DrawCall()
{

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentTexture);
	glDrawArrays(GL_TRIANGLES, 0, m_environmentCube->NF() * 3);

	glDepthMask(GL_TRUE);
	glBindVertexArray(0);
}
