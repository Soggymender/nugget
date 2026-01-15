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
    m_transformDirty = true;

    // Walk the object hierarchy and update world transforms.

//  m_sceneComponent.Update(deltaTime);
}

void NEntity::UpdateTransform()
{
    if (m_rootComponent == nullptr)
        return;

    if (!m_transformDirty)
        return;

    m_rootComponent->UpdateTransform();

    m_transformDirty = false;
}

void NEntity::Draw(Shader& shader)
{
    if (m_rootComponent == nullptr)
        return;

    // Catch-all in case this entity isn't part of a scene or an update didn't occur for some reason.
    UpdateTransform();

    for (NObjectComponent* pComponent : m_components)
    {
        if (NStaticMeshComponent* pMeshComp = dynamic_cast<NStaticMeshComponent*>(pComponent))
        {
            shader.setMatrix("model", pMeshComp->m_transformWs);

            pMeshComp->pMesh->Draw(shader);
        }
    }
}

void NEntity::SetPositionLS(glm::vec3& position)
{
    if (m_rootComponent == nullptr)
        return;

    m_rootComponent->m_position = position;
}
