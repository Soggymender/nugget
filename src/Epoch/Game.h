#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "engine/camera.h"
#include "engine/shader.h"

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

    Camera workspaceCamera;
    Camera spaceCamera;

    Game();
   ~Game();
   
    void Create(GLFWwindow *window, unsigned int width, unsigned int height);
    void Destroy();

    void SetScreenTexture(unsigned int textureId);

    void ProcessInput(float dt);
    void Cursor(double x, double y);
    void Scroll(double x, double y);
    void KeyDown(int key, int scancode, int action);

    void Update(float dt);
    void UpdateGUI();

    void SetScreenUniforms(Shader* screenShader);
    void RenderSpace();
    void RenderWorkstation();
    void RenderHUD();

};