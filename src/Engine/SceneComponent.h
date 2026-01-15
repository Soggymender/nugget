#ifndef SCENE_COMPONENT_H
#define SCENE_COMPONENT_H

#include "engine/ObjectComponent.h"

#include <glm/glm.hpp>
#include "glm/gtx/quaternion.hpp"

#include <vector>

using namespace std;

class NSceneComponent : public NObjectComponent
{
public:

    // WS if no parent. LS if parent.
    glm::vec3 m_position = {};
    glm::vec3 m_rotation = {};

    glm::mat4 m_transformWs = {};

    NSceneComponent* m_parent = nullptr;
    vector<NSceneComponent*> m_children = {};

    void SetParent(NSceneComponent* pParent);
    void AddChild(NSceneComponent* pChild);

    void Update(float deltaTime);
    void UpdateTransform();

    void SetPositionLS(glm::vec3& position);
};

#endif