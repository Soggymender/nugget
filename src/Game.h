#include "GLFW/glfw3.h"

#include "camera.h"
#include "shader.h"

enum GameState {
    GAME_PLAY,
    GAME_MENU
}; 

class Game
{
public:

    GameState    State;	
    bool         Keys[1024];
    unsigned int Width, Height;

    GLFWwindow *window = nullptr;

    Shader ourShader;

    Camera camera;

    Game();
   ~Game();
   
    void Create(GLFWwindow *window, unsigned int width, unsigned int height);
    void Destroy();

    void ProcessInput(float dt);
    void Scroll(double x, double y);

    void Update(float dt);
    void Render();

};