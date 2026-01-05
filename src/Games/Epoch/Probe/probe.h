#ifndef PROBE_H
#define PROBE_H

#include <string>

#include "engine/shader.h"
#include "engine/entity.h"

#include "glm/glm.hpp"

#include "AttitudeControlModule.h"

using namespace std;

class Probe
{
public:

    Shader* shader;
    NEntity entity;

    glm::vec3 translation = glm::vec3(0.0f, 0.0f, -10.0f);

    float heading = 0.0f;
    float headingRate = 5.0f;

    unsigned short imageSize = 0;
    unsigned short image[0xFFFF];

    // Modules
    AttitudeControlModule attitudeControlModule;

    Probe();

    bool Receive(const unsigned short* image, unsigned short imageSize);
    void Run();

    void Create(Shader* shader);
    void Update(float deltaTime);
    void Render();
};

#endif