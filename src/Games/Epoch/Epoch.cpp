#include "Epoch.h"

#include "Engine/Nugget.h"
#include "Engine/Entity.h"
#include "Engine/Scene.h"
#include "Engine/SceneLoader.h"

#include "engine/core.h"

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

// Workspace camera.
bool camControl = false;

glm::vec3 workspaceCamOrient;
float workspaceCamZoom = 0.0f;

// Space camera.
glm::vec3 spaceCamOrient;
float spaceCamZoom = 0.0f;

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

GLuint toolIconTextures[6];
GLuint fileIconTextures[2];

char textBuffer[256 * 256] = "";

GLuint noiseTexture;

NEntity computer;
NScene  officeScene;

class EntityCustomProcessor : public NSceneLoader::ICustomProcessor
{
    NEntity* PreProcessEntity(string name, unordered_map<string, void*>const& properties, NScene* scene)
    {
        // Check the type.
        // Allocate the correct type derived from NEntity.

        NEntity* entity = new NEntity(name);
        scene->Add(entity);

        return entity;
    }

    void PostProcessEntity(NEntity* pEntity, string parentName, unordered_map<string, void*>const& properties, NScene* scene)
    {
        // Cast pEntity to the correct type.
        // Post process.
    }
};

GLuint createTexture(const int width, const int height)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    unsigned char noiseData[256 * 256 * 4];

    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Fill the array with random values for RGBA channels (0-255)
    for (int i = 0; i < 256 * 256 * 4; i += 4) {

        int val = std::rand() % 256;
        noiseData[i] = std::rand() % 256;
        noiseData[i + 1] = std::rand() % 256;
        noiseData[i + 2] = std::rand() % 256;
        noiseData[i + 3] = std::rand() % 256;
    }

    // Upload data to OpenGL texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, noiseData);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return textureID;
}

EpochGame::EpochGame()
  : State(GameState::GAME_PLAY),
    Keys(),
    workspaceCamera(0.0f, 0.0f, 1.0f),
    spaceCamera(0.0f, 0.0f, 1.0f)
{
    workstation.Create();
}

EpochGame::~EpochGame()
{

}

void EpochGame::Create()
{
    // Tile Map
    ourShader.Create("shaders/shader.vs", "shaders/shader.fs");

    sprite.Create(&ourShader, "assets/sprites/test.csv", 16, 16, "assets/sprites/arthax.png");
    //    sprite.depthOffset = 0.1f;

    probe.Create(&ourShader);
    skybox.Create(&ourShader);

    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    workspaceCamera.pos.x = 0.0f;
    workspaceCamera.pos.y = 0.0f;
    workspaceCamera.pos.z -= 9.0f;

    spaceCamera.pos.x = 0.0f;
    spaceCamera.pos.y = 0.0f;
    spaceCamera.pos.z -= 9.0f;

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

    noiseTexture = createTexture(256, 256);

    EntityCustomProcessor entityCustomProcessor;
    NSceneLoader::Instance().LoadScene("assets/office/Models/office.fbx", &entityCustomProcessor, &officeScene);

    NEntity* computer = officeScene.FindEntityByName("Monitor");
    if (computer)
    {
        computer->m_sceneComponent.m_position = glm::vec3(0.0f, 0.0f, -10.0f);
    }

 //   computer.m_sceneComponent.m_position = glm::vec3(0.0f, 0.0f, -10.0f);
 //   computer.m_sceneComponent.m_rotation = glm::vec3(0.0f, 45.0f, 0.0f);
    //NSceneLoader::Instance().LoadScene("assets/computer/Models/PC.obj", nullptr, &computer);
}

void EpochGame::Destroy()
{
    sprite.Destroy();
}

void EpochGame::SetScreenTexture(unsigned int textureId)
{
    // Dig up the location of the current texture.

    // TODO: Find the right texture dynamically.
    //computer->meshes[6].textures[0]->id = textureId;
}

void EpochGame::Update(float deltaTime)
{
    ProcessInput(Nugget::Instance().GetWindow(), deltaTime);

    probe.Update(deltaTime);

    workspaceCamera.Update();
    spaceCamera.Update();

    skybox.Update(&spaceCamera, deltaTime);

    //   UpdateGUI();

    workspaceCamZoom = 0;
    spaceCamZoom = 0;
}

void ShowToolIcons(int small_pane_size, int height);
void ShowIconGrid();
GLuint LoadTextureFromFile(const char* filename);

void EpochGame::UpdateGUI() {

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

void EpochGame::ProcessInput(GLFWwindow* window, float deltaTime)
{
    glm::vec3 dir{};

    // First poll mouse and keyboard.
    //

    
    const NPointer* pPointer = Nugget::Instance().GetPointer();
    Cursor(pPointer->relativeX, pPointer->relativeY, deltaTime);

    const NScrollWheel* pScroll = Nugget::Instance().GetScrollWheel();
    Scroll(pScroll->deltaX, pScroll->deltaY, deltaTime);
    
    const NKeyboard* pKeyboard = Nugget::Instance().GetKeyboard();
    KeyDown(pKeyboard);
    

    if (camControl)
    {
        float speed = 5.0f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            dir += workspaceCamera.GetForward() * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            dir += -workspaceCamera.GetForward() * speed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            dir += -workspaceCamera.GetRight() * speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            dir += workspaceCamera.GetRight() * speed * deltaTime;
    }

    workspaceCamera.ProcessInput(dir, workspaceCamOrient, workspaceCamZoom, deltaTime);

    // TODO: Need a way to input-target this camera. Or not since in game it only needs to be controlled by coding.
    //spaceCamera.ProcessInput(dir, workspaceCamOrient, workspaceCamZoom, dt);
}

void EpochGame::Cursor(double x, double y, float deltaTime)
{
    if (camControl) {

        workspaceCamOrient.y += x * 0.1f * deltaTime; // Cursor X rotates around Y axis.
        workspaceCamOrient.x += y * 0.1f * deltaTime; // Cursor Y rotates around X axis;
    }
}

void EpochGame::Scroll(double x, double y, float deltaTime)
{
    workspaceCamZoom += y;
}

void EpochGame::KeyDown(const NKeyboard* pKeyboard)
{
    if (pKeyboard->JustReleased(GLFW_KEY_TAB))
    {
        camControl = !camControl;

        if (camControl)
            glfwSetInputMode(Nugget::Instance().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(Nugget::Instance().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

class Range2f {
public:
    float min, max;

    Range2f(float min, float max) { this->min = min; this->max = max; }
    float Map(float weight) { return min + (weight * (max - min)); }
};

void EpochGame::SetScreenUniforms(Shader* screenShader)
{
    // 0.0f = no signal.
    // 1.0f = perfect signal.
    float signalStrength = probe.attitudeControlModule.signalStrength;

    /*
    define min and max for each
    scale current min and max relative to signal strength.



    */



    static float s = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float a = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float c = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    // Base
    static float GlitchIntensity = 0.5f;
    static float GlitchNoiseLevel = 1.0f;

    screenShader->setFloat("intensity", GlitchIntensity);
    screenShader->setFloat("time", (float)glfwGetTime() * 1000.0f);
    screenShader->setVec2("resolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
    screenShader->setFloat("rngSeed", s);
    screenShader->setFloat("noiseLevel", GlitchNoiseLevel);
    screenShader->setTexture("texture2", noiseTexture, 1);
    screenShader->setVec3("randomValues", glm::vec3(a, b, c));

    // Jumble
    static float GlitchJumbleSpeed = 0.15f;
    static float GlitchJumbleShift = 0.25f;
    static float GlitchJumbleResolution = 0.15f;
    static float GlitchJumbleness = 0.25f;

    Range2f GlitchJumblenessRange(0.0f, 0.25f);

    screenShader->setFloat("jumbleSpeed", GlitchJumbleSpeed);
    screenShader->setFloat("jumbleShift", GlitchJumbleShift);
    screenShader->setFloat("jumbleResolution", GlitchJumbleResolution);
    //    screenShader->setFloat("jumbleness", GlitchJumbleness);
    screenShader->setFloat("jumbleness", GlitchJumblenessRange.Map(1.0f - signalStrength));

    // Color
    static float GlitchDispersion = 0.05f;
    Range2f GlitchDispersionRange(0.0f, 0.05f);

    screenShader->setFloat("dispersion", GlitchDispersionRange.Map(1.0f - signalStrength));

    // Line
    static float GlitchLineSpeed = 0.0f;// 0.03f;
    static float GlitchLineShift = 0.1f;

    static float GlitchLineResolution = 0.425f;
    static float GlitchLineDrift = 0.0f;
    static float GlitchLineVertShift = 0.0f;

    //   Range2f GlitchLineSpeedRange(0.0f, 0.001f);
    Range2f GlitchLineShiftRange(0.0f, 0.05f);

    // screenShader->setFloat("lineSpeed", GlitchLineSpeedRange.Map(1.0f - signalStrength));
    screenShader->setFloat("lineShift", GlitchLineShiftRange.Map(1.0f - signalStrength));
    screenShader->setFloat("lineSpeed", GlitchLineSpeed);
    //  screenShader->setFloat("lineShift", GlitchLineShift);


    screenShader->setFloat("lineResolution", GlitchLineResolution);
    screenShader->setFloat("lineDrift", GlitchLineDrift);
    screenShader->setFloat("lineVertShift", GlitchLineVertShift);
}

void EpochGame::Render()
{
    // Render Space.
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    RenderSpace();

    // Bind the primary framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

    // Render Workstation.
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //        glClear(GL_COLOR_BUFFER_BIT);

    RenderWorkstation();

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
}

void EpochGame::RenderSpace()
{
    ourShader.use();

    ourShader.setMatrix("view", spaceCamera.view);
    ourShader.setMatrix("projection", projection);

    probe.Render();

    skybox.Render();
}

void EpochGame::RenderWorkstation()
{
    /*
    static glm::vec3 translation = glm::vec3(10.0f, 0.0f, -10.0f);

    // render the loaded model
    glm::mat4 modelSpace = glm::mat4(1.0f);
    modelSpace = glm::translate(modelSpace, translation); // translate it down so it's at the center of the scene
    modelSpace = glm::scale(modelSpace, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

    // 2. Rotation: Rotate the mesh around the Y-axis by 45 degrees
    float angle = glm::radians(0.0f);  // Convert angle to radians
    glm::vec3 axis(1.0f, 0.0f, 0.0f);  // Y-axis
    modelSpace = glm::rotate(modelSpace, angle, axis);
    */
    ourShader.use();

    ourShader.setMatrix("view", workspaceCamera.view);
    ourShader.setMatrix("projection", projection);

    //ourShader.setMatrix("model", modelSpace);

//    computer.Draw(ourShader);

    officeScene.Draw(ourShader);
}
 
void EpochGame::RenderHUD()
{


}