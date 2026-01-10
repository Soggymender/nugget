#include "Scene.h"

#include "Entity.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

void NScene::Add(NEntity* pEntity)
{
	if (!pEntity)
		return;

	entities.push_back(pEntity);
}

void NScene::Update(float deltaTime)
{

}

void NScene::Draw(Shader& shader)
{
	for (NEntity* entity : entities)
		entity->Draw(shader);
}

NEntity* NScene::FindEntityByName(const string& name)
{
	for (NEntity* entity : entities)
	{
		if (entity->name == name)
			return entity;
	}

	return nullptr;
}
