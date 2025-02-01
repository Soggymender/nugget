#include "game.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui/imgui.h"

#include "sprite.h"

#include "workstation.h"
#include "probe/probe.h"
#include "skybox.h"

#include "engine/shader.h"
#include "Engine/Texture.h"

Workstation workstation;
Probe probe;
Skybox skybox;

Sprite sprite;
   
glm::mat4 projection;

float zoom = 0.0f;

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

GLuint toolIconTextures[6];
GLuint fileIconTextures[2];

char textBuffer[256 * 256] = "";

Game::Game() 
    : State(GAME_PLAY), 
      Keys(),
      camera(0.0f, 0.0f, 1.0f)
{ 
    workstation.Create();
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

    probe.Create(&ourShader);
    skybox.Create(&ourShader);

    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);   

    camera.pos.x = 0.0f;
    camera.pos.y = 0.0f;
    camera.pos.z -= 9.0f;

    int numToolIcons = 6;
    const char* toolIconPaths[] = {
        "assets/textures/icon_forward.png",
        "assets/textures/icon_stop.png",
        "assets/textures/icon_arrow_up.png",
        "assets/textures/icon_arrow_down.png",
        "assets/textures/icon_return.png",
        "assets/textures/icon_save.png"
    };

    int numFileIcons = 2;
    const char* fileIconPaths[] = {
        "assets/textures/icon_folder.png",
        "assets/textures/icon_file_asm.png",
    };

    for (int i = 0; i < numToolIcons; ++i) {
        toolIconTextures[i] = LoadTextureFromFile(toolIconPaths[i]);
    }

    for (int i = 0; i < numFileIcons; ++i) {
        fileIconTextures[i] = LoadTextureFromFile(fileIconPaths[i]);
    }
}

void Game::Destroy()
{
    sprite.Destroy();
}

void Game::Update(float dt)
{
    probe.Update(dt);
    camera.Update();
    skybox.Update(&camera, dt);

    UpdateGUI();

    zoom = 0;
}

void ShowToolIcons(int small_pane_size, int height);
void ShowIconGrid();
GLuint LoadTextureFromFile(const char* filename);

void Game::UpdateGUI() {

    ImGui::NewFrame();
    {
        static float f = 0.0f;
        static int counter = 0;

        ImVec2 size(SCR_WIDTH * 1.0f, SCR_HEIGHT * 1.0f);  // Example: 80% width, 60% height

        // Set up ImGui layout
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);  // Always set the size

        ImGui::SetNextWindowPos(ImVec2((SCR_WIDTH - size.x) * 0.5f, (SCR_HEIGHT - size.y) * 0.5f), ImGuiCond_Always);  // Center the window
        ImGui::SetNextWindowBgAlpha(0.33f);

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
        float space_between_panes = width * 0.04f; // Space between the large pane and the small panes
        float small_pane_height = (height - space_between_panes) * 0.5f; // Height of each small pane

        // Create a large pane (left)
        ImVec2 large_pane_size = ImVec2(large_pane_width, height);
        ImGui::BeginChild("IDE", large_pane_size, true);

        ImGui::Text("Gain: %.3f", probe.attitudeControlModule.signalStrength);

        // Begin ImGui window
//        ImGui::Begin("Epoch IDE", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImVec2 textSize = ImGui::GetContentRegionAvail();

        ImGui::InputTextMultiline("##InputText", textBuffer, IM_ARRAYSIZE(textBuffer), textSize, ImGuiInputTextFlags_AllowTabInput);

        //       ImGui::End();


        ImGui::EndChild();

        // Move to the right side of the large pane
        ImGui::SameLine();
        ImGui::BeginGroup();

        // Create a container to hold the small panes vertically
        ImVec2 small_pane_size = ImVec2(ImGui::GetWindowSize().x - large_pane_width - (borderSize * 3.0f), small_pane_height);

        ImGui::SetNextWindowBgAlpha(0.75f);

        // First small pane
        ImGui::BeginChild("SmallPane1", small_pane_size, true);
        ImGui::EndChild();

        // Space between small panes
        ShowToolIcons(small_pane_size.x, space_between_panes - (borderSize * 2));

        // Second small pane
        ImGui::SetNextWindowBgAlpha(0.75f);
        ImGui::BeginChild("SmallPane2", small_pane_size, true);

        ShowIconGrid();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 5.0f));

        char buf1[64] = "";
        //ImGui::InputText("##", buf1, 64);

        if (workstation.currentSourceFile) {
            // TODO: This doesn't need to happen every frame...
            strcpy(buf1, workstation.currentSourceFile->filename.c_str());
        }

        ImGui::InputText("##", buf1, 64);

        ImGui::PopStyleVar();

        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::End();
    }
}

void LoadCurrentSourceFile() {

    // Load the source file's text.
    strcpy(textBuffer, workstation.currentSourceFile->contents.c_str());
}

void RestoreCurrentSourceFile() {

    if (workstation.currentSourceFile == nullptr)
        return;

    workstation.currentSourceFile->RevertContents();
    LoadCurrentSourceFile();
}

void SaveCurrentSourceFile() {

    if (workstation.currentSourceFile == nullptr)
        return;

    // Load the source file's text.
    workstation.currentSourceFile->contents = textBuffer;

    workstation.currentSourceFile->SaveContents();
}

void SelectSourceFile(int x, int y) {

    // Save the current file's text.
    if (workstation.currentSourceFile != nullptr) {
        workstation.currentSourceFile->contents = textBuffer;
    }

    // Select the new file.
    workstation.currentSourceFile = &workstation.sourceFiles[y][x];

    LoadCurrentSourceFile();
}

void DiscardChanges() {


}

void BuildAndTransmit() {

    if (workstation.BuildCurrentSourceFile()) {

        // Build succesful, begin transmit.
        probe.Receive(workstation.image, workstation.imageSize);
    }
}

void ShowToolIcons(int small_pane_size, int height) {

    ImGuiStyle& style = ImGui::GetStyle();
    float borderSize = style.WindowPadding.x;

    float areaWidth = small_pane_size;

    int offsetX = (areaWidth - ((height + borderSize) * 2)) / 2;

    // Set up grid parameters
    ImVec2 iconSize(height, height);

    //ImGui::SameLine(offsetX);

    // Transmit
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

    ImGui::PushID(0);
    if (ImGui::ImageButton((void*)(intptr_t)toolIconTextures[2], iconSize)) {
        BuildAndTransmit();
    }
    ImGui::PopID();

    // Receive
    ImGui::SameLine(1 * (height + borderSize) + offsetX);

    ImGui::PushID(1);
    ImGui::ImageButton((void*)(intptr_t)toolIconTextures[3], iconSize);
    ImGui::PopID();

    // Save
    ImGui::SameLine(3 * (height + borderSize) + offsetX);

    ImGui::PushID(2);
    if (ImGui::ImageButton((void*)(intptr_t)toolIconTextures[5], iconSize)) {
        SaveCurrentSourceFile();
    }
    ImGui::PopID();

    // Reload
    ImGui::SameLine(4 * (height + borderSize) + offsetX);

    ImGui::PushID(3);
    if (ImGui::ImageButton((void*)(intptr_t)toolIconTextures[4], iconSize)) {
        RestoreCurrentSourceFile();
    }
    ImGui::PopID();
}

void ShowIconGrid() {

    // Set up grid parameters
    int iconsPerRow = 5;  // How many icons per row
    ImVec2 iconSize(64.0f, 64.0f);  // Set the size of each icon

    // Start a new column for layout
    ImGui::Columns(iconsPerRow, NULL, false);

    //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 20.0f));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.2f, 0.0f));

    int i = 0;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 5; x++) {

            ImGui::PushID(i);

            bool isSelected = false;
            if (workstation.currentSourceFile != nullptr && workstation.currentSourceFile->x == x && workstation.currentSourceFile->y == y) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 1.0f, 1.0f));
                isSelected = true;
            }

            if (ImGui::ImageButton((void*)(intptr_t)fileIconTextures[1], iconSize)) {
                
                SelectSourceFile(x, y);
            }

            if (isSelected) {
                ImGui::PopStyleColor();
            }

            ImGui::PopID();

            i++;
 
            ImGui::NextColumn();  // Move to the next column
        }
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    // End the columns layout
    ImGui::Columns(1);  // Reset columns to a single column after the grid
}

void Game::ProcessInput(float dt)
{
    /*
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
    */
}

void Game::Scroll(double x, double y)
{
    zoom = y;
}

void Game::Render()
{
    ourShader.setMatrix("view", camera.view);
    ourShader.setMatrix("projection", projection);

    probe.Render();
    skybox.Render();
}