#include "Entity.h"

#include "Shader.h"

using namespace std;

NEntity::NEntity()
{
	AttachComponent(&m_sceneComponent);
}

void NEntity::Update(float deltaTime)
{
    // Walk the object hierarchy and update world transforms.

//  m_sceneComponent.Update(deltaTime);
}

void NEntity::Draw(Shader& shader)
{
    if (meshes.size() == 0)
        return;

    if (m_rootComponent == nullptr)
        return;

    m_rootComponent->UpdateTransform();

    shader.setMatrix("model", m_sceneComponent.m_transformWS);

    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void NEntity::SetPositionLS(glm::vec3& position)
{
    if (m_rootComponent == nullptr)
        return;

    m_rootComponent->m_position = position;
}
