#pragma once
#include "Include.h"
#include <list>

enum eTextureUsageFlags {
	AMBIENT = 1 << 0, // 0001
	DIFFUSE = 1 << 1, // 0010
	SPECULAR = 1 << 2  // 0100
};

struct sTextureUsage {
private:
	uint8_t m_textureMask;

public:
	sTextureUsage(bool b_ambient, bool b_diffuse, bool b_specular) {
		m_textureMask = 0;
		if (b_ambient) m_textureMask |= AMBIENT;
		if (b_diffuse)   m_textureMask |= DIFFUSE;
		if (b_specular)  m_textureMask |= SPECULAR;
	}

	// Check if a texture is used
	bool useTexture(eTextureUsageFlags tex) const {
		return (m_textureMask & tex) != 0;
	}

	// Check if any texture is used
	bool useAnyTexture() {
		return m_textureMask != 0;
	}

	sTextureUsage() = default;
};

class cMesh
{
public:
	cy::TriMesh* m_cyMesh;
	std::vector<std::pair<GLuint, eTextureUsageFlags>> m_textureBinding;
	size_t m_bufferOffset;

public:
	cMesh() :m_cyMesh(nullptr), m_bufferOffset(0) {
		m_textureBinding.clear();
	}
	void UploadTexture(eTextureUsageFlags i_textureUsage, std::string i_textureDir);
};

// Bitmask Flags for Attributes
enum eVertexAttributeFlags {
	POSITION = 1 << 0, // 0001
	NORMAL = 1 << 1, // 0010
	TEXCOORD = 1 << 2  // 0100
};

struct sVertexBufferStruct {

private:
	uint8_t m_attributeMask;

public:
	sVertexBufferStruct(bool b_position, bool b_normal, bool b_uvCoord) {
		m_attributeMask = 0;
		if (b_position) m_attributeMask |= POSITION;
		if (b_normal)   m_attributeMask |= NORMAL;
		if (b_uvCoord)  m_attributeMask |= TEXCOORD;
	}

	// Check if an attribute is enabled
	bool hasAttribute(eVertexAttributeFlags attr) const {
		return (m_attributeMask & attr) != 0;
	}

	sVertexBufferStruct() = default;
};

enum eMVPMatrixFlags {
	MODEL = 1 << 0, // 0001
	VIEW = 1 << 1, // 0010
	PROJECTION = 1 << 2  // 0100
};

class cVertexShaderProgram {

public:
	GLuint m_shaderProgram, m_VAO, m_VBO;
	std::vector<cMesh*> m_meshes;
	sVertexBufferStruct* m_vertexBufferStruct;

protected:
	GLuint m_shaderModelMat, m_shaderViewMat, m_shaderProjectionMat;

private:
	size_t m_VBOoffset;
	GLfloat* m_mappedBuffer = nullptr;

	GLuint m_textureKa, m_textureKd, m_textureKs;
	GLuint m_shaderCameraPosition, m_shaderLightingPosition;

	size_t m_bufferSize;

public:
	cVertexShaderProgram(sVertexBufferStruct* i_vertexBufferStruct);

	virtual cMesh* UploadMesh(const char* i_meshObjPath, sTextureUsage* i_textureUsage);

	virtual void LinkShaders(char const* i_vertexShaderFilename, char const* i_fragmentShaderFilename);

	void SetMVPMatrix(glm::mat4 i_matrix, eMVPMatrixFlags i_matrixName);

	void SetCameraPosition(glm::vec3 i_cameraPos);
	void SetLightingPosition(glm::vec3 i_lightPos);

	virtual void DrawCall();
};

class cEnvironmentShaderProgram: cVertexShaderProgram {

private:
	cy::TriMesh* m_environmentCube;
	GLuint m_environmentTexture;

public:
	cEnvironmentShaderProgram();

	bool UploadEnvironmentTexture(std::vector<std::string> i_fileNames);

	void DrawCall() override;
};



