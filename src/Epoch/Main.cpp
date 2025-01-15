#ifdef GAME_EPOCH

#include "glad/glad.h"

#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_img.h"

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

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    blarg.Scroll(xoffset, yoffset);
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

    std::vector<Line> programLines;
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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

        processInput(window);
        blarg.ProcessInput(deltaTime);

        blarg.Update(deltaTime);

        // Imgui
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;
            static int counter = 0;

            ImVec2 size(SCR_WIDTH * 1.0f, SCR_HEIGHT * 1.0f);  // Example: 80% width, 60% height

            // Set up ImGui layout
            ImGui::SetNextWindowSize(size, ImGuiCond_Always);  // Always set the size
                        
            ImGui::SetNextWindowPos(ImVec2((SCR_WIDTH - size.x) * 0.5f, (SCR_HEIGHT - size.y) * 0.5f), ImGuiCond_Always);  // Center the window
            ImGui::SetNextWindowBgAlpha(0.66f);

            RenderUI();
            /*
            // Begin ImGui window
            ImGui::Begin("Epoch IDE", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            ImVec2 textSize = ImGui::GetContentRegionAvail();

            static char textBuffer[1024 * 16] = "";
            ImGui::InputTextMultiline("##InputText", textBuffer, IM_ARRAYSIZE(textBuffer), textSize);

            ImGui::End();
            */
        }

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

void RenderUI() {
    ImGui::Begin("Main Window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();

    vMin.x += ImGui::GetWindowPos().x;
    vMin.y += ImGui::GetWindowPos().y;
    vMax.x += ImGui::GetWindowPos().x;
    vMax.y += ImGui::GetWindowPos().y;

    ImGuiStyle& style = ImGui::GetStyle();
    float borderSize = style.WindowPadding.x;// .WindowPadding;// .FrameBorderSize;

//    ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));

    float width = vMax.x - vMin.x;
    float height = vMax.y - vMin.y;

    // Define sizes for the panes
    float large_pane_width = width * 0.70f;  // Width of the large pane
    float space_between_panes = width * 0.05f; // Space between the large pane and the small panes
    float small_pane_height = (height - space_between_panes) * 0.5f; // Height of each small pane

    // Create a large pane (left)
    ImVec2 large_pane_size = ImVec2(large_pane_width, height);
    ImGui::BeginChild("LargePane", large_pane_size, true);
    ImGui::Text("Large Pane");
    if (ImGui::Button("Large Pane Button")) {
        // Handle button click
    }

    ImGui::EndChild();

    // Move to the right side of the large pane
    ImGui::SameLine();
    ImGui::BeginGroup();

    // Create a container to hold the small panes vertically
    ImVec2 small_pane_size = ImVec2(ImGui::GetWindowSize().x - large_pane_width - (borderSize * 3.0f), small_pane_height);

    // First small pane
    ImGui::BeginChild("SmallPane1", small_pane_size, true);
    ImGui::Text("Small Pane 1");
    if (ImGui::Button("Button 1")) {
        // Handle button click
    }
    ImGui::EndChild();

    // Space between small panes
    ImGui::Dummy(ImVec2(0, space_between_panes - borderSize));

    // Second small pane
    ImGui::BeginChild("SmallPane2", small_pane_size, true);
    ImGui::Text("Small Pane 2");
    if (ImGui::Button("Button 2")) {
        // Handle button click
    }
    ImGui::EndChild();
    ImGui::EndGroup();

    ImGui::End();
}


void ShowIconGrid() {
    // Example list of image paths (replace with actual paths)
    const char* iconPaths[] = {
        "icon1.png",
        "icon2.png",
        "icon3.png",
        "icon4.png",
        "icon5.png",
        "icon6.png",
    };

    // Load textures for the icons
    GLuint iconTextures[6];
    for (int i = 0; i < 6; ++i) {
        iconTextures[i] = LoadTextureFromFile(iconPaths[i]);
    }

    // Set up grid parameters
    int iconsPerRow = 3;  // How many icons per row
    ImVec2 iconSize(64.0f, 64.0f);  // Set the size of each icon

    // Start a new column for layout
    ImGui::Columns(iconsPerRow, NULL, false);  // false means no border between columns

    for (int i = 0; i < 6; ++i) {
        if (ImGui::ImageButton((void*)(intptr_t)iconTextures[i], iconSize)) {
            // Handle icon click (e.g., perform an action)
            std::cout << "Icon " << i + 1 << " clicked!" << std::endl;
        }
        ImGui::NextColumn();  // Move to the next column
    }

    // End the columns layout
    ImGui::Columns(1);  // Reset columns to a single column after the grid
}

GLuint LoadTextureFromFile(const char* filename) {
    // Load the image using stb_image
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (!image) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return 0;  // Return 0 if the texture loading failed
    }

    // Generate an OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload the image to the GPU as a texture
    if (channels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    else if (channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    else {
        std::cerr << "Unsupported image format" << std::endl;
        stbi_image_free(image);
        return 0;  // Return 0 if the image format is unsupported
    }

    // Generate mipmaps for the texture
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free the image data as it's no longer needed
    stbi_image_free(image);

    return textureID;
}


#endif