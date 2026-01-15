#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>

#include "engine/SceneComponent.h"

using namespace std;

class NEntity;
class Shader;

class NScene
{
public:

    vector<NEntity*> entities;

    void Add(NEntity* pEntity);

    void Update(float deltaTime);
    void UpdateTransforms();
    void Draw(Shader& shader);

    NEntity* FindEntityByName(const string& name);
};

#endif