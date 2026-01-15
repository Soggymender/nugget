#ifndef ENTITY_H
#define ENTITY_H

#include "engine/Mesh.h"
#include "engine/Object.h"
#include "engine/SceneComponent.h"

#include <string>
#include <vector>

using namespace std;

class Shader;

class NEntity : public NObject
{
public:

	string name = "";

	NSceneComponent m_defaultSceneComponent = {};
	NSceneComponent* m_rootComponent = &m_defaultSceneComponent;

	bool m_transformDirty = true;

	NEntity();
	NEntity(string& name) { this->name = name; }

	void Update(float deltaTime);
	void UpdateTransform();
	void Draw(Shader& shader);

	void SetPositionLS(glm::vec3& position);
};

#endif