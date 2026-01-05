#ifndef ENTITY_H
#define ENTITY_H

#include "engine/Mesh.h"
#include "engine/Object.h"
#include "engine/SceneComponent.h"

#include <vector>

using namespace std;

class Shader;

class NEntity : public NObject
{
public:

	NSceneComponent m_sceneComponent;

	vector<Mesh> meshes;

	NEntity();

	void Draw(Shader& shader);
};

#endif