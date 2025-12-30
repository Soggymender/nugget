#include "engine/IGameModule.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "engine/camera.h"
#include "engine/shader.h"

struct NKeyboard;

enum class GameState {
    GAME_PLAY,
    GAME_MENU
};

class EpochGame : public IGameModule
{
public:

    GameState    State;
    bool         Keys[1024];

    Shader ourShader;

    Camera workspaceCamera;
    Camera spaceCamera;

    EpochGame();
    ~EpochGame();

    void Create();
    void Destroy();

    void SetScreenTexture(unsigned int textureId);

    void ProcessInput(GLFWwindow* window, float deltaTime);
    void Cursor(double x, double y, float deltaTime);
    void Scroll(double x, double y, float deltaTime);
    void KeyDown(const NKeyboard* pKeyboard);

    void Update(float deltaTime);
    void UpdateGUI();

    void SetScreenUniforms(Shader* screenShader);

    void Render();

    void RenderSpace();
    void RenderWorkstation();
    void RenderHUD();
};