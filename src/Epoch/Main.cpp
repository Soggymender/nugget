#ifdef GAME_EPOCH

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

#include "game.h"

#include "vm/vm.h"
#include "vm/assembler.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//const unsigned int SCR_WIDTH = 1600;
//const unsigned int SCR_HEIGHT = 900;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

double cursorLastX;
double cursorLastY;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

Game blarg;
void RenderUI();

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    blarg.Cursor(xpos - cursorLastX, ypos - cursorLastY);
    cursorLastX = xpos;
    cursorLastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    blarg.Scroll(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
    blarg.KeyDown(key, scancode, action);
}

extern unordered_map<string, unsigned short> registerIndices;

int main(void)
{
/*
    char* writtenAsm = nullptr;

    unsigned short imageSize = 0;
    unsigned short image[0xFFFF];

//    unsigned short testImageSize = 0;
//    unsigned short testImage[0xFFFF];

    // Load the test image.
//    memset(testImage, 0, sizeof(testImage));
//    read_image("assets\\programs\\2048.obj", false, testImage);

    FILE* f = fopen("assets\\programs\\2048.asm", "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    writtenAsm = (char *)malloc(fsize + 1);
    fread(writtenAsm, fsize, 1, f);
    fclose(f);

    writtenAsm[fsize] = 0;

    // Clear the image buffer.
    memset(&image, 0, sizeof(image));

    vm_assemble(writtenAsm, image, imageSize, programLines);// testImage);
    */

    // Check encoding accuracy.

    /*
    cout << endl << endl;

    for (int i = 0; i < imageSize; i++)
    {
        cout << "\t" << bitset<16>(image[i]);

        if (image[i] != testImage[i]) {
            cout << " != " << bitset<16>(testImage[i]);
            exec_instr(testImage[i]);
            exec_instr(image[i]);
        }

        cout << endl;
    }

    cout << endl << endl;

    return 0;
    */

//    char img_file[] = "none";
//    vm_main(img_file);

//    if (testImage != nullptr) {
//        free(testImage);
//    }

//    return 0;

    glfwInit();

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "nug", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
	glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Input callbacks.

    glfwGetCursorPos(window, &cursorLastX, &cursorLastY);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetKeyCallback(window, key_callback);



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
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    blarg.Create(window, SCR_WIDTH, SCR_HEIGHT);



    // Screen shader.
    Shader screenShader;
    screenShader.Create("shaders/screen.vs", "shaders/bktGlitch/frag.fs");

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    // Setup frame buffer
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Setup render texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Change the PC monitor's texture.
    blarg.SetScreenTexture(textureColorbuffer);

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

    GLuint vao, vbo;

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
    
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Imgui
        // Start the Dear ImGui frame
    //    ImGui_ImplOpenGL3_NewFrame();
      //  ImGui_ImplGlfw_NewFrame();

        processInput(window);
        blarg.ProcessInput(deltaTime);

        blarg.Update(deltaTime);

        // Bind the game framebuffer.
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // IMGUI Rendering
  //      ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // Render Space.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        blarg.RenderSpace();

        // Bind the primary framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

        // Render Workstation.
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

//        glClear(GL_COLOR_BUFFER_BIT);

        blarg.RenderWorkstation();

        /*

        // Render game to primary.
        screenShader.use();

        blarg.SetScreenUniforms(&screenShader);

        glBindVertexArray(screenVao);
        glDisable(GL_DEPTH_TEST);

        screenShader.setTexture("texture1", textureColorbuffer, 0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Draw all UI elements here.
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        */

        glfwSwapBuffers(window);

        // Timer
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;          
    }

    blarg.Destroy();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

#endif