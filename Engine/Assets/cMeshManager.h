#pragma once
#include "Include.h"

struct sMeshInstance {
	std::shared_ptr<cMesh> m_mesh;
	glm::vec3 m_boundingBoxMin;
	glm::vec3 m_boundingBoxMax;
};

class cMeshSystem {

private:
	std::vector<std::unique_ptr<sMeshInstance>> m_meshList;

public:
	void RegisterMesh(std::shared_ptr<cMesh> i_mesh);

	sMeshInstance* SelectMesh(const glm::vec3& rayOrigin, const glm::vec3& rayDir);

	void DeleteMesh(sMeshInstance* i_mesh);

private:
	bool IntersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
		const glm::vec3& boxMin, const glm::vec3& boxMax,
		float& outDistance);
};
