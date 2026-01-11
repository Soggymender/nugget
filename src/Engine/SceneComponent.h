#ifndef SCENE_COMPONENT_H
#define SCENE_COMPONENT_H

#include "engine/ObjectComponent.h"

#include <glm/glm.hpp>
#include "glm/gtx/quaternion.hpp"

using namespace std;

class NSceneComponent : public NObjectComponent
{
public:

    glm::vec3 m_position = {};
    glm::vec3 m_rotation = {};

    glm::mat4 m_transformWS = {};

    void Update(float deltaTime);
    void UpdateTransform();
};

#endif