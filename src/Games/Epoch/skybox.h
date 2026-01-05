#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>

#include "engine/camera.h"
#include "engine/entity.h"
#include "engine/shader.h"

#include "glm/glm.hpp"

using namespace std;


class Skybox
{
public:

    Shader* shader;
    NEntity entity;

    Skybox();

    void Create(Shader* shader);
    void Update(Camera* camera, float deltaTime);
    void Render();
};

#endif