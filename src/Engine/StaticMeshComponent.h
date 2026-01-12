#ifndef STATIC_MESH_COMPONENT_H
#define STATIC_MESH_COMPONENT_H

#include "engine/SceneComponent.h"

#include <glm/glm.hpp>
#include "glm/gtx/quaternion.hpp"

#include "Engine/Mesh.h"

using namespace std;

class NStaticMeshComponent : public NSceneComponent
{
public:
	
	Mesh* pMesh = nullptr;

	NStaticMeshComponent(Mesh* pMesh) { this->pMesh = pMesh; }
};

#endif