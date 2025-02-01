#ifdef GAME_EPOCH

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

#include "game.h"

#include "vm/vm.h"
#include "vm/assembler.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

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

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    blarg.Create(window, SCR_WIDTH, SCR_HEIGHT);


    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Imgui
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        processInput(window);
        blarg.ProcessInput(deltaTime);

        blarg.Update(deltaTime);

        // IMGUI Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // Game Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        blarg.Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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