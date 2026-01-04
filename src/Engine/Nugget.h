#ifndef NUGGET_H
#define NUGGET_H

#include "IGameModule.h"
#include "Singleton.h"

#include <unordered_map>

struct GLFWwindow;

enum class NUpdatePhase
{
    None = 0,

    PreUpdate,
    Update,
    PostUpdate
};

enum class NRenderPhase
{
    None = 0,
    PreRender,
    PostRender
};

struct NPointer
{
    // Previous absolute location.
    double prevX = 0.0f;
    double prevY = 0.0f;

    // Current absolute position.
    double curX = 0.0f;
    double curY = 0.0f;

    // Current relative position. Only valid for the current frame.
    double relativeX = 0.0f;
    double relativeY = 0.0f;

    void Clear()
    {
        relativeX = 0.0f;
        relativeY = 0.0f;
    }
};

struct NScrollWheel
{
    double deltaX = 0.0f;
    double deltaY = 0.0f;
};

struct NKeyState
{
    bool pressed;
    bool wasPressed;
};

struct NKeyboard
{
    std::unordered_map<int, NKeyState> keyStates;

    bool JustPressed(int key) const
    {
        if (keyStates.find(key) == keyStates.end())
            return false;

        const auto& keyState = keyStates.at(key);
        
        return keyState.pressed && !keyState.wasPressed;
    }

    bool JustReleased(int key) const 
    {
        if (keyStates.find(key) == keyStates.end())
            return false;

        const auto& keyState = keyStates.at(key);
        return !keyState.pressed && keyState.wasPressed;
    }
};

class Nugget : public NSingleton<Nugget> {

    friend class NSingleton<Nugget>;

public:

    void Run(IGameModule* pGameModule);

    GLFWwindow* GetWindow() { return m_pWindow; }

    const NPointer* GetPointer() { return &m_pointer; }
    const NScrollWheel* GetScrollWheel() { return &m_scroll; }
    const NKeyboard* GetKeyboard() { return &m_keyboard; }

private:

    // Can't make your own.
    Nugget() = default;

    bool m_Running = true;
    GLFWwindow* m_pWindow = nullptr;
    unsigned int m_frameBuffer;

    NPointer m_pointer;
    NScrollWheel m_scroll;
    NKeyboard m_keyboard;

    void Create();
    void Destroy();

    void Update(float deltaTime, NUpdatePhase updatePhase);
    void Render(NRenderPhase renderPhase);

    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif