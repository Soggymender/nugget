#include "probe.h"

#include "Engine/SceneLoader.h"

#include "../vm/vm.h"

#include "glm/gtx/rotate_vector.hpp"

float timeSinceStart = 0.0f;

Probe::Probe()
{

}

bool Probe::Receive(const unsigned short* image, unsigned short imageSize)
{
    if (this->image == nullptr || image == nullptr) {
        return false;
    }

    memcpy(this->image, image, 0xFFFF);
    
    Run();

    return true;
}

void Probe::Run()
{
    if (image == nullptr)
        return;

    vm_run(image);
}

void Probe::Create(Shader* shader)
{
    this->shader = shader;

    NSceneLoader::Instance().LoadScene("assets/probe/probe.obj", nullptr, &entity);

    attitudeControlModule.Initialize(this);
}

void Probe::Update(float deltaTime) {

    heading += headingRate * deltaTime;

    attitudeControlModule.Update(deltaTime);

    timeSinceStart += deltaTime;
}

void Probe::Render()
{
    // render the loaded model
    glm::mat4 modelSpace = glm::mat4(1.0f);
    modelSpace = glm::translate(modelSpace, translation); // translate it down so it's at the center of the scene
    modelSpace = glm::scale(modelSpace, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

    // 2. Rotation: Rotate the mesh around the Y-axis by 45 degrees
    float angle = glm::radians(-45.0f);  // Convert angle to radians
    glm::vec3 axis(1.0f, 0.0f, 0.0f);  // Y-axis
    modelSpace = glm::rotate(modelSpace, angle, axis);

    angle = glm::radians(heading);  // Convert angle to radians
    axis = glm::vec3(0.0f, 1.0f, 0.0f);  // Y-axis
    modelSpace = glm::rotate(modelSpace, angle, axis);

//    uniform float pixelationIntensity;  // Controls the intensity of the pixelation (higher = more pixelated)
//    uniform float noiseAmount;

    shader->use();

    shader->setMatrix("model", modelSpace);

    entity.Draw(*shader);
}