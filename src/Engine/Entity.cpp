#include "Entity.h"

#include "Shader.h"

using namespace std;

NEntity::NEntity()
{
	AttachComponent(&m_sceneComponent);
}

void NEntity::Update(float deltaTime)
{
//  m_sceneComponent.Update(deltaTime);
}

void NEntity::Draw(Shader& shader)
{
    if (meshes.size() == 0)
        return;

    m_sceneComponent.UpdateTransform();

    shader.setMatrix("model", m_sceneComponent.m_transformWS);

    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}