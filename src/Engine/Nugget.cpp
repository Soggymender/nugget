#include "Nugget.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include <iostream>
#include <bitset>
#include <unordered_map>


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "engine/shader.h"

//#include "vm/vm.h"
//#include "vm/assembler.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//const unsigned int SCR_WIDTH = 1600;
//const unsigned int SCR_HEIGHT = 900;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Nugget::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    NPointer* pPointer = &Instance().m_pointer;

    pPointer->prevX = pPointer->curX;
    pPointer->prevY = pPointer->curY;

    pPointer->curX = xpos;
    pPointer->curY = ypos;

    pPointer->relativeX = pPointer->curX - pPointer->prevX;
    pPointer->relativeY = pPointer->curY - pPointer->prevY;
}

void Nugget::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    NScrollWheel* pScroll = &Instance().m_scroll;

    pScroll->deltaX = xoffset;
    pScroll->deltaY = yoffset;
}

void Nugget::SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    NKeyboard* pKeyboard = &Instance().m_keyboard; 

    if (key == GLFW_KEY_UNKNOWN)
        return;

    if (action == GLFW_PRESS)
    {
        pKeyboard->keyStates[key].pressed = true;
        pKeyboard->keyStates[key].wasPressed = false;
    }
    else if (action == GLFW_RELEASE)
    {
        pKeyboard->keyStates[key].pressed = false;
        pKeyboard->keyStates[key].wasPressed = true;
    }
}

extern unordered_map<string, unsigned short> registerIndices;

void Nugget::Run(IGameModule* gameModule) {

    Create();
    gameModule->Create();

    while (m_Running) {

        Update(deltaTime, NUpdatePhase::PreUpdate);
        gameModule->Update(deltaTime);
        Update(deltaTime, NUpdatePhase::PostUpdate);

        Render(NRenderPhase::PreRender);
        gameModule->Render();
        Render(NRenderPhase::PostRender);

        // Timer
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }

    gameModule->Destroy();
    Destroy();
}

void Nugget::Create()
{
    glfwInit();

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_pWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "nug", NULL, NULL);
    if (m_pWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return; // -1;
    }

    glfwMakeContextCurrent(m_pWindow);
    glfwSetFramebufferSizeCallback(m_pWindow, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return; // -1;
    }

    // Input callbacks.

    glfwGetCursorPos(m_pWindow, &m_pointer.prevX, &m_pointer.prevY);
    glfwSetCursorPosCallback(m_pWindow, CursorPosCallback);
    glfwSetScrollCallback(m_pWindow, ScrollCallback);

    glfwSetKeyCallback(m_pWindow, SetKeyCallback);



    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();


    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); ;// (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    io.IniFilename = nullptr;
    io.FontGlobalScale = 2.0f;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Moved to ::Run. Alternate sequence. May break game init.
    //blarg.Create(m_pWindow, SCR_WIDTH, SCR_HEIGHT);



    // Screen shader.
    Shader screenShader;
    screenShader.Create("shaders/screen.vs", "shaders/bktGlitch/frag.fs");

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    // Setup frame buffer
    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

    // Setup render texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Change the PC monitor's texture.
    //blarg.SetScreenTexture(textureColorbuffer);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // Setup render buffer
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Validate
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create a full screen quad and VAO
    GLuint screenVao, screenVbo;

    // Define the vertices for a full-screen quad (in NDC coordinates)
    float vertices[] = {
        // Position         // UV Coordinates
        -1.0f, -1.0f,      0.0f, 0.0f,  // Bottom-left
         1.0f, -1.0f,      1.0f, 0.0f,  // Bottom-right
        -1.0f,  1.0f,      0.0f, 1.0f,  // Top-left
         1.0f,  1.0f,      1.0f, 1.0f   // Top-right
    };

    // Generate and bind the VAO
    glGenVertexArrays(1, &screenVao);
    glBindVertexArray(screenVao);

    // Generate and bind the VBO
    glGenBuffers(1, &screenVbo);
    glBindBuffer(GL_ARRAY_BUFFER, screenVbo);

    // Load the vertex data into the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Specify the vertex attribute for position (2 floats)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Specify the vertex attribute for UV (2 floats)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind the VAO (optional, for safety)
    glBindVertexArray(0);

    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Nugget::Destroy()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

void Nugget::Update(float deltaTime, NUpdatePhase updatePhase)
{
    if (updatePhase == NUpdatePhase::PreUpdate)
    {
        // Update the full set of keyboard keys.
        m_pointer.Clear();

        m_scroll.deltaX = 0.0f;
        m_scroll.deltaY = 0.0f;

        m_keyboard.keyStates.clear();

        glfwPollEvents();

        // Imgui
        // Start the Dear ImGui frame
    //    ImGui_ImplOpenGL3_NewFrame();
      //  ImGui_ImplGlfw_NewFrame();

        if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            m_Running = false;
            glfwSetWindowShouldClose(m_pWindow, true);
        }
    }
    else if (updatePhase == NUpdatePhase::PostUpdate)
    {

    }
}

void Nugget::Render(NRenderPhase renderPhase)
{
    if (renderPhase == NRenderPhase::PreRender)
    {
        // Bind the game framebuffer.
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

        // IMGUI Rendering
    //      ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_pWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
    }

    else if (renderPhase == NRenderPhase::PostRender)
    {
        glfwSwapBuffers(m_pWindow);
    }
}
