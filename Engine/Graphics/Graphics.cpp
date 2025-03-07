#include "Graphics.h"

void cMesh::UploadTexture(eTextureUsageFlags i_textureUsage, std::string i_textureDir)
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

	m_bufferSize = 1 * 1024 * 1024;

	// Allocate buffer storage with Persistent Mapping & Coherent flags
	glBufferStorage(GL_ARRAY_BUFFER, m_bufferSize, NULL,
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	// Map the buffer once and keep it mapped
	m_mappedBuffer = (GLfloat*)glMapBufferRange(GL_ARRAY_BUFFER, 0, m_bufferSize,
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	// Track offset
	m_VBOoffset = 0;

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


	if (m_VBOoffset + i_dataSize > m_bufferSize) { // BUFFER_SIZE should be total allocated buffer size
		std::cerr << "ERROR: Not enough buffer space for mesh data!" << std::endl;
		// TODO: expand the buffer
		return nullptr;
	}

	memcpy(m_mappedBuffer + m_VBOoffset, i_vertices.data(), i_dataSize);
	i_mesh->m_bufferOffset = 0;
	m_VBOoffset += i_dataSize;

	// upload textures
	if (i_textureUsage->useAnyTexture()) {
		// get and decode texture png
		size_t i_lastSlash = std::string(i_meshObjPath).find_last_of("/\\");
		std::string directory = (i_lastSlash == std::string::npos) ? "" : std::string(i_meshObjPath).substr(0, i_lastSlash + 1);

		// ambient
		if (i_textureUsage->useTexture(AMBIENT)) {
			i_mesh->UploadTexture(AMBIENT, directory);
		}
		// diffuse
		if (i_textureUsage->useTexture(DIFFUSE)) {
			i_mesh->UploadTexture(DIFFUSE, directory);
		}
		// specular
		if (i_textureUsage->useTexture(SPECULAR)) {
			i_mesh->UploadTexture(SPECULAR, directory);
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
			glBindTexture(GL_TEXTURE_2D, i_texBinding.first);

			switch (i_texBinding.second) {
			case AMBIENT:
				glUniform1i(m_textureKa, i_textureUnit);
				break;
			case DIFFUSE:
				glUniform1i(m_textureKd, i_textureUnit);
				break;
			case SPECULAR:
				glUniform1i(m_textureKs, i_textureUnit);
				break;
			}

			i_textureUnit++;
		}
		glDrawArrays(GL_TRIANGLES, i_mesh->m_bufferOffset, i_mesh->m_cyMesh->NF() * 3);
	}
	glBindVertexArray(0);
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
	///glClear(GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentTexture);
	glDrawArrays(GL_TRIANGLES, 0, m_environmentCube->NF() * 3);

	glDepthMask(GL_TRUE);
	glBindVertexArray(0);
}
