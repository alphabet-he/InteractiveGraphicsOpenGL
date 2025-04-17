#pragma once
#include "Include.h"
#include <list>

enum eTextureUsageFlags {
	AMBIENT = 1 << 0, // 0001
	DIFFUSE = 1 << 1, // 0010
	SPECULAR = 1 << 2,  // 0100
	SKYBOX_REFLECTION = 1 << 3, // 1000
	SCREEN_TEXTURE = 1 << 4,
	SHADOW_MAP = 1 << 5,
	NORMAL_MAP = 1 << 6,
	DISPLACEMENT_MAP = 1 << 7,
	FLAT_SPRITE = 1 << 8
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
	std::map<eTextureUsageFlags, GLuint> m_textureBinding;
	size_t m_bufferOffset;
	glm::mat4 m_modelMat; //TODO: Change accessibility
	std::string m_filePath;
	std::map<eTextureUsageFlags, std::string> m_textureFiles;

public:
	cMesh() :m_cyMesh(nullptr), m_bufferOffset(0) {
		m_textureBinding.clear();
		m_modelMat = glm::mat4(1.0f);
		m_filePath = "";
	}
	void UploadPNGTexture(eTextureUsageFlags i_textureUsage, std::string i_textureFilePath);
	inline void UploadTexture(GLuint i_tex, eTextureUsageFlags i_flag) {
		m_textureBinding[i_flag] = i_tex;
		m_textureFiles[i_flag] = "";
	};
	inline void SetModelMat(const glm::mat4& i_model) { m_modelMat = i_model; };
};

struct sLight {
	glm::vec3 m_pos;
	glm::vec3 m_color;

	sLight(glm::vec3 i_pos, glm::vec3 i_color) :
		m_pos(i_pos), m_color(i_color) {}
};

// Bitmask Flags for Attributes
enum eVertexAttributeFlags {
	POSITION = 1 << 0, // 0001
	NORMAL = 1 << 1, // 0010
	TEXCOORD = 1 << 2,  // 0100
	TANGENT = 1 << 3
};

struct sVertexBufferStruct {

private:
	uint8_t m_attributeMask;

public:
	sVertexBufferStruct(bool b_position, bool b_normal, bool b_uvCoord, bool b_tangent) {
		m_attributeMask = 0;
		if (b_position) m_attributeMask |= POSITION;
		if (b_normal)   m_attributeMask |= NORMAL;
		if (b_uvCoord)  m_attributeMask |= TEXCOORD;
		if (b_tangent)  m_attributeMask |= TANGENT;
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

struct sScreenTextureInfo {
	GLuint m_FBO, m_RBO, m_texture;
	uint16_t m_textureWidth, m_textureHeight;
	GLuint m_screenTextureTexPosition;
};

struct sShadowMapInfo {
	GLuint m_FBO, m_texture;
	uint16_t m_textureWidth, m_textureHeight;
	GLuint m_shadowShaderProgram, m_shadowShaderViewMat, m_shadowShaderProjMat, m_shadowShaderModelMat;
	GLuint m_shadowMapTexPosition;
	std::vector<std::pair<glm::mat4, GLuint>> m_multiLightsDataArray;
};

struct sGBufferInfo {
	uint16_t m_windowWidth, m_windowHeight;

	GLuint m_gBuffer, m_RBO;
	GLuint m_gPosition, m_gNormal, m_gObjColor, m_gAlbedoSpec;

	GLuint m_gBufferShaderProgram;
	GLuint m_gPositionInt, m_gNormalInt, m_gObjColorInt, m_gAlbedoSpecInt;

	GLuint m_quadVAO, m_quadVBO;
};

struct sGeometryShaderProgram {
	GLuint m_shaderProgram;
	GLuint m_shaderModelMat, m_shaderViewMat, m_shaderProjectionMat;
	GLuint m_shaderTessLevelPosition;
};

class cVertexShaderProgram {

public:
	std::vector<std::weak_ptr<cMesh>> m_meshes;
	sVertexBufferStruct* m_vertexBufferStruct;

protected:
	GLuint m_shaderProgram, m_VAO, m_VBO;
	GLuint m_shaderModelMat, m_shaderViewMat, m_shaderProjectionMat;

	sScreenTextureInfo* m_screenTextureInfo = nullptr;
	sShadowMapInfo* m_shadowMapInfo = nullptr;
	sGBufferInfo* m_gBufferInfo = nullptr;
	sGeometryShaderProgram* m_geometryShaderProgram = nullptr;

	std::string m_vertexShaderPath, m_fragmentShaderPath;

private:
	size_t m_VBOOffset;
	size_t m_VBOVerticeOffset;
	GLfloat* m_mappedBuffer = nullptr;

	GLuint m_textureKa, m_textureKd, m_textureKs;
	GLuint m_textureSkyboxReflection, m_textureNormalMap, m_textureDisplacementMap;
	GLuint m_shaderCameraPosition, m_shaderLightingPosition;
	GLuint m_shaderLightNumPostion;
	GLuint m_shaderTessLevelPosition;

	size_t m_bufferSize;

	glm::mat4 m_viewMat, m_projMat;

	bool b_useTessellation = false;

public:
	cVertexShaderProgram();

	cVertexShaderProgram(sVertexBufferStruct* i_vertexBufferStruct);

	void UploadMesh(std::weak_ptr<cMesh> i_meshWeakPtr, sTextureUsage* i_textureUsage);

	void LinkShaders(char const* i_vertexShaderFilename, char const* i_fragmentShaderFilename);

	void SetTessellationShader(char const* i_vertexShaderFilename,
		char const* i_tessellationControlShaderFilename,
		char const* i_tessellationEvaluationShaderFilename,
		char const* i_fragmentShaderFilename);

	void SetMVPMatrix(glm::mat4 i_matrix, eMVPMatrixFlags i_matrixName);

	void SetCameraPosition(glm::vec3 i_cameraPos);
	void SetLightingPosition(glm::vec3 i_lightPos);
	void SetLights(std::vector<sLight*> i_lights);

	void SetTessellationLevel(int i_level);

	virtual void DrawCall();

	void InitializeGeometryShaderProgram();
	void GeometryDrawCall();

	void InitializeScreenTexture(uint16_t i_width, uint16_t i_height);
	GLuint RenderToScreenTexture(glm::vec3 i_cameraLocation, 
		glm::vec3 i_faceDirection, 
		std::vector<cVertexShaderProgram*> i_programsToDraw);

	void InitializeShadowMap(uint16_t i_width, uint16_t i_height);
	// spot light
	GLuint RenderSpotLightShadowMap(glm::vec3 i_lightLocation, glm::vec3 i_targetLocation,
		float i_lightConeAgnle, float i_lightingNearPlane, float i_lightingFarPlane);
	void RenderMultiSpotLightShadowMap(std::vector<sLight*> i_lights, glm::vec3 i_targetLocation,
		float i_lightConeAgnle, float i_lightingNearPlane, float i_lightingFarPlane);

	// directional light
	GLuint RenderDirectionalLightShadowMap(glm::vec3 i_lightDirection,
		float i_orthoSize, float i_lightingNearPlane, float i_lightingFarPlane);
	//TODO multiple directional light

	void InitializeGBuffer(uint16_t i_windowWidth, uint16_t i_windowHeight);

	void RenderGBuffer();

	void DrawCallWithGBuffer();

	inline std::string GetVertexShaderPath() {
		return m_vertexShaderPath;
	}

	inline std::string GetFragmentShaderPath() {
		return m_fragmentShaderPath;
	}

private:
	// Helper functions
	void ResetShaderProgram();
	void GetShaderUniforms();
	void ComputeLightViewProjMat(glm::vec3 i_lightLocation, glm::vec3 i_targetLocation,
		float i_lightConeAgnle, float i_lightingNearPlane, float i_lightingFarPlane,
		glm::mat4& o_viewMatrix, glm::mat4& o_projMatrix);
	void ComputeLightViewProjMat(glm::vec3 i_lightDirection,
		float i_orthoSize, float i_lightingNearPlane, float i_lightingFarPlane,
		glm::mat4& o_viewMatrix, glm::mat4& o_projMatrix);
	GLuint RenderShadowMapWithViewProjMat(glm::mat4 i_viewMatrix, glm::mat4 i_projMatrix);
};

class cEnvironmentShaderProgram: public cVertexShaderProgram {

private:
	cy::TriMesh* m_environmentCube;
	GLuint m_environmentTexture;

public:
	cEnvironmentShaderProgram();

	bool UploadEnvironmentTexture(std::vector<std::string> i_fileNames);

	void DrawCall() override;

	inline void SetMeshReflection(cMesh i_mesh) {
		i_mesh.UploadTexture(m_environmentTexture, SKYBOX_REFLECTION);
	}
};

namespace Graphics {
	GLuint GenerateTextureFromImage(std::string i_textureFilePath);
}