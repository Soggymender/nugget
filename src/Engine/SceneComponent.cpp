#include "SceneComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

void NSceneComponent::SetParent(NSceneComponent* pParent)
{
	m_parent = pParent;

	if (m_parent != nullptr)
		m_parent->AddChild(this);
}

void NSceneComponent::AddChild(NSceneComponent* pChild)
{
	if (pChild != nullptr)
		m_children.push_back(pChild);
}

void NSceneComponent::Update(float deltaTime)
{

	//glm::vec3 m_position;
	//glm::vec3 m_rotation;

	//glm::mat4 transformWS;
}

void NSceneComponent::UpdateTransform()
{
	// Get the parent transform.
	glm::mat4 parentTxWs = glm::mat4(1.0f);

	if (m_parent != nullptr)
		parentTxWs = m_parent->m_transformWs;

	// Transform local position into world position.

    glm::mat4 transformLs = glm::mat4(1.0f);

	// Move.
    transformLs = glm::translate(transformLs, m_position);

	// Scale.
    transformLs = glm::scale(transformLs, glm::vec3(1.0f, 1.0f, 1.0f));
	
	// Rotate.
	transformLs = glm::rotate(transformLs, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
	transformLs = glm::rotate(transformLs, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
	transformLs = glm::rotate(transformLs, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));

	m_transformWs = parentTxWs * transformLs;

	// If we had children here is where we'd iterate them.
	for (NSceneComponent* pChild : m_children)
		pChild->UpdateTransform();
}

void NSceneComponent::SetPositionLS(glm::vec3& position)
{
	m_position = position;
}