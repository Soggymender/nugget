#include "game.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "npc.h"
#include "sprite.h"
#include "tileMap.h"

#include "engine/shader.h"
#include "engine/Model.h"

TileMap tileMap;
Sprite sprite;
Npc npc;
   
glm::mat4 projection;

Model* ourModel;

glm::vec3 camOrient;
float zoom = 0.0f;

glm::vec3 mapCenter;

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
    sprite.depthOffset = 0.1f;

    tileMap.Create(&ourShader, "assets/tilemaps/test2_ground.csv", "assets/tilemaps/test2_surface.csv", 100, 100, "assets\\miniworld+\\terrain.png");



    projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);   

    mapCenter = tileMap.GetCenter();
    camera.pos.x = mapCenter.x;
    camera.pos.y = mapCenter.y;

    npc.Create(&sprite);
    npc.depthOffset = 0.01f;

    npc.pos.x = mapCenter.x;
    npc.pos.y = mapCenter.y;
}

void Game::Destroy()
{
    tileMap.Destroy();
    npc.Destroy();
    sprite.Destroy();
}

void Game::Update(float dt)
{
    camera.Update();

    npc.Update(dt);

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

    camera.ProcessInput(dir, camOrient, zoom, dt);
}

void Game::Cursor(double x, double y)
{
    camOrient.y += x;
    camOrient.x += y;
}

void Game::Scroll(double x, double y)
{
    zoom = y;
}

void Game::Render()
{
    ourShader.setMatrix("view", camera.view);
    ourShader.setMatrix("projection", projection);

    tileMap.Render();

    sprite.mesh.textureId = 0;
    sprite.Render();
    npc.Render();



    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(mapCenter.x, mapCenter.y, -10.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    ourShader.setMatrix("model", model);

}