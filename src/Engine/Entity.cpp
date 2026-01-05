#include "Entity.h"

#include "Shader.h"

using namespace std;

NEntity::NEntity()
{
	AttachComponent(&m_sceneComponent);
}

void NEntity::Draw(Shader& shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}