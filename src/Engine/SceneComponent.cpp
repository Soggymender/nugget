#include "SceneComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

void NSceneComponent::Update(float deltaTime)
{

	//glm::vec3 m_position;
	//glm::vec3 m_rotation;

	//glm::mat4 transformWS;
}

void NSceneComponent::UpdateTransform()
{
    m_transformWS = glm::mat4(1.0f);

	// Move.
    m_transformWS = glm::translate(m_transformWS, m_position);

	// Scale.
    m_transformWS = glm::scale(m_transformWS, glm::vec3(1.0f, 1.0f, 1.0f));
	
	// Rotate.
	m_transformWS = glm::rotate(m_transformWS, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
	m_transformWS = glm::rotate(m_transformWS, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
	m_transformWS = glm::rotate(m_transformWS, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
}

void NSceneComponent::SetPositionLS(glm::vec3& position)
{
	m_position = position;
}