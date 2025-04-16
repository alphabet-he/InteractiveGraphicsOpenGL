#pragma once
#include "Include.h"

struct sMeshInstance {
private:
	std::shared_ptr<cMesh> m_mesh;

public:
	glm::vec3 m_boundingBoxMin;
	glm::vec3 m_boundingBoxMax;

	inline void SetMesh(std::shared_ptr<cMesh> i) {
		m_mesh = i;
	}
	inline std::weak_ptr<cMesh> GetMesh() {
		std::weak_ptr<cMesh> i_weakptr = m_mesh;
		return i_weakptr;
	}
};

class cMeshSystem {

private:
	std::vector<std::unique_ptr<sMeshInstance>> m_meshList;

public:
	sMeshInstance* RegisterMesh(const char* i_meshObjPath);

	sMeshInstance* SelectMesh(const glm::vec3& rayOrigin, const glm::vec3& rayDir);

	void DeleteMesh(sMeshInstance* i_mesh);

	void ForEachMesh(const std::function<void(sMeshInstance*)>& func) const {
		for (const auto& mesh : m_meshList) {
			func(mesh.get());
		}
	}

private:
	bool IntersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
		const glm::vec3& boxMin, const glm::vec3& boxMax,
		float& outDistance);
};
