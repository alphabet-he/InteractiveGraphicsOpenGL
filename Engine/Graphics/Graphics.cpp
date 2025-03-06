#include "Graphics.h"

cVertexShaderProgram::cVertexShaderProgram(sVertexBufferStruct i_vertexBufferStruct)
{
	m_vertexBufferStruct = i_vertexBufferStruct;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	m_shaderProgram = glCreateProgram();

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	// Allocate buffer storage with Persistent Mapping & Coherent flags
	glBufferStorage(GL_ARRAY_BUFFER, 1024 * sizeof(GLfloat), NULL,
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	// Map the buffer once and keep it mapped
	m_mappedBuffer = (GLfloat*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 1024 * sizeof(GLfloat),
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	// Track offset
	m_VBOoffset = 0;

	GLuint i_attributeIndex = 0;
	size_t i_stride = 0;

	// Compute stride
	if (m_vertexBufferStruct.hasAttribute(POSITION))  i_stride += 3 * sizeof(GLfloat);
	if (m_vertexBufferStruct.hasAttribute(NORMAL))    i_stride += 3 * sizeof(GLfloat);
	if (m_vertexBufferStruct.hasAttribute(TEXCOORD))  i_stride += 2 * sizeof(GLfloat);

	// set attributes
	size_t offset = 0;
	if (m_vertexBufferStruct.hasAttribute(POSITION)) {
		glVertexAttribPointer(i_attributeIndex, 3, GL_FLOAT, GL_FALSE, i_stride, (void*)0);
		glEnableVertexAttribArray(i_attributeIndex);
		i_attributeIndex++;
		offset += 3;
	}
	if (m_vertexBufferStruct.hasAttribute(NORMAL)) {
		glVertexAttribPointer(i_attributeIndex, 3, GL_FLOAT, GL_FALSE, i_stride, (void*)(offset * sizeof(GLfloat)));
		glEnableVertexAttribArray(i_attributeIndex);
		i_attributeIndex++;
		offset += 3;
	}
	if (m_vertexBufferStruct.hasAttribute(TEXCOORD)) {
		glVertexAttribPointer(i_attributeIndex, 2, GL_FLOAT, GL_FALSE, i_stride, (void*)(offset * sizeof(GLfloat)));
		glEnableVertexAttribArray(i_attributeIndex);
		i_attributeIndex++;
		offset += 2;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

cMesh* cVertexShaderProgram::UploadMesh(const char* i_meshObjPath, sTextureUsage i_textureUsage)
{
	cMesh* i_mesh = new cMesh();
	i_mesh->m_cyMesh = new cy::TriMesh();
	i_mesh->m_cyMesh->LoadFromFileObj(i_meshObjPath);

	std::vector<GLfloat> i_vertices;

	for (int i = 0; i < (int)i_mesh->m_cyMesh->NF(); i++) {
		for (int j = 0; j < 3; j++) {
			if (m_vertexBufferStruct.hasAttribute(POSITION)) {
				unsigned int i_vertexInd = i_mesh->m_cyMesh->F(i).v[j];
				i_vertices.push_back(i_mesh->m_cyMesh->V(i_vertexInd).x);
				i_vertices.push_back(i_mesh->m_cyMesh->V(i_vertexInd).y);
				i_vertices.push_back(i_mesh->m_cyMesh->V(i_vertexInd).z);
			}

			if (m_vertexBufferStruct.hasAttribute(NORMAL)) {
				unsigned int i_normalInd = i_mesh->m_cyMesh->FN(i).v[j];
				i_vertices.push_back(i_mesh->m_cyMesh->VN(i_normalInd).x);
				i_vertices.push_back(i_mesh->m_cyMesh->VN(i_normalInd).y);
				i_vertices.push_back(i_mesh->m_cyMesh->VN(i_normalInd).z);
			}

			if (m_vertexBufferStruct.hasAttribute(TEXCOORD)) {
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

	memcpy(m_mappedBuffer + m_VBOoffset, i_vertices.data(), i_dataSize);
	i_mesh->m_bufferOffset = m_VBOoffset;
	m_VBOoffset += i_dataSize;

	// upload textures
	if (i_textureUsage.useAnyTexture()) {
		// get and decode texture png
		size_t i_lastSlash = std::string(i_meshObjPath).find_last_of("/\\");
		std::string directory = (i_lastSlash == std::string::npos) ? "" : std::string(i_meshObjPath).substr(0, i_lastSlash + 1);

		// ambient
		if (i_textureUsage.useTexture(AMBIENT)) {
			i_mesh->UploadTexture(AMBIENT, directory);
		}
		// diffuse
		if (i_textureUsage.useTexture(DIFFUSE)) {
			i_mesh->UploadTexture(DIFFUSE, directory);
		}
		// specular
		if (i_textureUsage.useTexture(SPECULAR)) {
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

	delete i_vertexShader;
	delete i_fragmentShader;
}

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
