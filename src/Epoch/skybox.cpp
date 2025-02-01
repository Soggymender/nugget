#include "skybox.h"

#include "vm/vm.h"

Skybox::Skybox()
{

}

void Skybox::Create(Shader* shader)
{
    this->shader = shader;

    model = new Model("assets/skybox/skybox.obj");
}
float heading = 0.0f;
float headingRate = 5.0f;

void Skybox::Update(Camera* camera, float deltaTime) {

    heading += headingRate * deltaTime;
}

void Skybox::Render()
{
    // render the loaded model
    glm::mat4 modelSpace = glm::mat4(1.0f);
    modelSpace = glm::translate(modelSpace, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    modelSpace = glm::scale(modelSpace, glm::vec3(50.0f, 50.0f, 50.0f));	// it's a bit too big for our scene, so scale it down

    float angle = 0.0f;// glm::radians(-heading);  // Convert angle to radians
    glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f);  // Y-axis
    modelSpace = glm::rotate(modelSpace, angle, axis);


    shader->setMatrix("model", modelSpace);

 //   model->Draw(*shader);
}