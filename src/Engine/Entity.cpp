#include "Entity.h"

#include "Shader.h"
#include "StaticMeshComponent.h"

using namespace std;

NEntity::NEntity()
{
	AttachComponent(&m_defaultSceneComponent);
}

void NEntity::Update(float deltaTime)
{
    // Walk the object hierarchy and update world transforms.

//  m_sceneComponent.Update(deltaTime);
}

void NEntity::Draw(Shader& shader)
{
    if (m_rootComponent == nullptr)
        return;

    m_rootComponent->UpdateTransform();

    shader.setMatrix("model", m_defaultSceneComponent.m_transformWS);

    for (NObjectComponent* pComponent : m_components)
    {
        if (NStaticMeshComponent* pMeshComp = dynamic_cast<NStaticMeshComponent*>(pComponent))
            pMeshComp->pMesh->Draw(shader);
    }
}

void NEntity::SetPositionLS(glm::vec3& position)
{
    if (m_rootComponent == nullptr)
        return;

    m_rootComponent->m_position = position;
}
