#include "game.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "sprite.h"

#include "engine/shader.h"
#include "engine/Model.h"

Sprite sprite;
   
glm::mat4 projection;

Model* ourModel;

float zoom = 0.0f;

glm::vec3 mapCenter(0.0f, 0.0f, 0.0f);

Game::Game() 
    : State(GAME_PLAY), 
      Keys(),
      camera(0.0f, 0.0f, 1.0f)
{ 

}

Game::~Game()
{
    
}

void Game::Create(GLFWwindow *window, unsigned int width, unsigned int height)
{
    this->window = window;
    this->Width = width;
    this->Height = height;

    // Tile Map
    ourShader.Create("shaders/shader.vs", "shaders/shader.fs");   

    sprite.Create(&ourShader, "assets/sprites/test.csv", 16, 16, "assets/sprites/arthax.png");
//    sprite.depthOffset = 0.1f;

    ourModel = new Model("assets/backpack/backpack.obj");



    projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);   

    camera.pos.x = mapCenter.x;
    camera.pos.y = mapCenter.y;
}

void Game::Destroy()
{
    sprite.Destroy();
}

void Game::Update(float dt)
{
    camera.Update();

    zoom = 0;
}

void Game::ProcessInput(float dt)
{
    glm::vec3 dir{};

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        dir += camera.up;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        dir += -camera.up;
    
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        dir += -camera.right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        dir += camera.right;   

    if (glm::length(dir) > 0.0f) {
        dir = glm::normalize(dir);
    }

    camera.ProcessInput(dir, zoom, dt);
}

void Game::Scroll(double x, double y)
{
    zoom = y;
}


void Game::Render()
{
    ourShader.setMatrix("view", camera.view);
    ourShader.setMatrix("projection", projection);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(mapCenter.x, mapCenter.y, -10.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    ourShader.setMatrix("model", model);

    ourModel->Draw(ourShader);
}