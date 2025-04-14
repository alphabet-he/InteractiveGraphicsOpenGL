#include "cMeshManager.h"

sMeshInstance* cMeshSystem::RegisterMesh(const char* i_meshObjPath)
{
    auto i_mesh = std::make_shared<cMesh>();
    i_mesh->m_cyMesh = new cy::TriMesh();
    i_mesh->m_cyMesh->LoadFromFileObj(i_meshObjPath);
    i_mesh->m_filePath = i_meshObjPath;

	auto i = std::make_unique<sMeshInstance>();
    i->SetMesh(i_mesh);

	i_mesh->m_cyMesh->ComputeBoundingBox();
    glm::vec3 i_boundMax = glm::vec3(i_mesh->m_cyMesh->GetBoundMax().x,
		i_mesh->m_cyMesh->GetBoundMax().y,
		i_mesh->m_cyMesh->GetBoundMax().z);
    glm::vec3 i_boundMin = glm::vec3(i_mesh->m_cyMesh->GetBoundMin().x,
		i_mesh->m_cyMesh->GetBoundMin().y,
		i_mesh->m_cyMesh->GetBoundMin().z);

    i->m_boundingBoxMax = i_boundMax;
    i->m_boundingBoxMin = i_boundMin;

    sMeshInstance* ret = i.get();
	m_meshList.push_back(std::move(i));
    return ret;
}

sMeshInstance* cMeshSystem::SelectMesh(const glm::vec3& rayOrigin, const glm::vec3& rayDir)
{

    sMeshInstance* closestMesh = nullptr;
    float closestDistance = std::numeric_limits<float>::max();

    for (const auto& mesh : m_meshList) {
        float hitDist;
        glm::mat4 invModel = glm::inverse(mesh->GetMesh().lock()->m_modelMat);
        glm::vec3 localRayOrigin = glm::vec3(invModel * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 localRayDir = glm::normalize(glm::vec3(invModel * glm::vec4(rayDir, 0.0f)));
        if (IntersectRayAABB(localRayOrigin, localRayDir,
            mesh->m_boundingBoxMin, mesh->m_boundingBoxMax,
            hitDist)) {
            if (hitDist < closestDistance) {
                closestDistance = hitDist;
                closestMesh = mesh.get();
            }
        }
    }

    return closestMesh;
}

void cMeshSystem::DeleteMesh(sMeshInstance* i_mesh)
{
	for (auto it = m_meshList.begin(); it != m_meshList.end(); ++it) {
		if (it->get() == i_mesh) {
			m_meshList.erase(it);
			break;
		}
	}
}

bool cMeshSystem::IntersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& boxMin, const glm::vec3& boxMax, float& outDistance)
{
    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; ++i) {
        if (std::abs(rayDir[i]) < 1e-8f) {
            // Ray is parallel to slab. No hit if origin not within slab
            if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i]) {
                return false;
            }
        }
        else {
            float ood = 1.0f / rayDir[i];
            float t1 = (boxMin[i] - rayOrigin[i]) * ood;
            float t2 = (boxMax[i] - rayOrigin[i]) * ood;

            if (t1 > t2) std::swap(t1, t2);

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax) {
                return false;
            }
        }
    }

    outDistance = tMin;
    return true;
}

