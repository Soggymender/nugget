#include "sprite.h"

#include "engine/shader.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_img.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>

bool Sprite::Create(Shader* shader, const char* spriteFilename, unsigned int width, unsigned int height, const char* textureFilename)
{
    std::ifstream mapInput{spriteFilename};
    if (!mapInput.is_open()) {
        std::cerr << "Couldn't read file: " << spriteFilename << "\n";
        return false; 
    }

    std::ifstream tilesetInput{textureFilename};
    if (!tilesetInput.is_open()) {
        std::cerr << "Couldn't read file: " << textureFilename << "\n";
        return false; 
    }

    tileIds = (unsigned short*)::operator new(width * height * sizeof(unsigned short));
/*
    for (std::string line; std::getline(mapInput, line);) {

        std::istringstream ss(std::move(line));
        
        for (std::string value; std::getline(ss, value, ',');) {
            tileIds[tileIdCount++] = std::stoi(value);
        }
    }
*/
 
    this->width = width;
    this->height = height;

    int textureWidth, textureHeight, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    stbi_set_flip_vertically_on_load(true);     
    unsigned char *data = stbi_load(textureFilename, &textureWidth, &textureHeight, &nrChannels, STBI_rgb_alpha);

    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    spriteUvWidth = float(width) / float(textureWidth);
    textureTileHeight = float(height) / float(textureHeight);

    int framesPerRow = textureWidth / width;
    int framesPerColumn = textureHeight / height;
    mapWidth = 30;
    mapHeight = 20;

    int frameId = 0;

    tilesetPos = new TilesetPosition[framesPerRow * framesPerColumn];
    
    for (int y = 0; y < framesPerColumn; y++) {

        for (int x = 0; x < framesPerRow; x++) {

            int index = y * framesPerRow + x;
            tilesetPos[index].x = frameId % framesPerRow;
            tilesetPos[index].y = frameId / framesPerRow;

            frameId++;
        }
    }

    GenerateMesh();

    mesh.Create(textureId, shader);

    model = glm::mat4(1.0f);

    return true;
}

void Sprite::Destroy()
{
    mesh.Destroy();
}

void Sprite::Update(float deltaTime)
{
    static glm::mat4 identity(1.0f);

    glm::vec3 adjustedPos(pos);
    adjustedPos.z += depthOffset;

    model = glm::translate(identity, adjustedPos);
}

void Sprite::Render()
{
    mesh.shader->setMatrix("model", model);
    mesh.Render();
}

void Sprite::GenerateMesh()
{
    int verticesCount = 4 * 32;
    mesh.vertices = new float[verticesCount];
    mesh.verticesSize = verticesCount * sizeof(float);
    
    int indicesCount = 2 * 3;
    mesh.indices = new unsigned int[indicesCount];
    mesh.indicesSize = indicesCount * sizeof(unsigned int);
  
    int screenWidth = 1280;
    int screenHeight = 1024;
    float screenTileWidth = 2 * float(width) / float(screenHeight);
    float screenTileHeight = 2 * float(height) / float(screenHeight);
    int screenSize = (screenWidth, screenHeight);


    float spriteScreenX1 = 0.0f;
    float spriteScreenY1 = 0.0f;
    float spriteScreenX2 = screenTileWidth;
    float spriteScreenY2 = screenTileHeight;
    
    int index = 35;
    float spriteTextureX1 = tilesetPos[index].x * spriteUvWidth;
    float spriteTextureY1 = tilesetPos[index].y * textureTileHeight;
    float spriteTextureX2 = spriteTextureX1 + spriteUvWidth;
    float spriteTextureY2 = spriteTextureY1 + textureTileHeight;

    // bottom right
    mesh.vertices[0] = spriteScreenX2;
    mesh.vertices[1] = 0;
    mesh.vertices[2] = 0.0f;

    mesh.vertices[3] = 1.0f;
    mesh.vertices[4] = 1.0f;
    mesh.vertices[5] = 1.0f;

    mesh.vertices[6] = spriteTextureX2;
    mesh.vertices[7] = spriteTextureY1;

    // top right
    mesh.vertices[8] = spriteScreenX2;
    mesh.vertices[9] = spriteScreenY2;
    mesh.vertices[10] = 0.0f;

    mesh.vertices[11] = 1.0f;
    mesh.vertices[12] = 1.0f;
    mesh.vertices[13] = 1.0f;

    mesh.vertices[14] = spriteTextureX2;
    mesh.vertices[15] = spriteTextureY2;

    // top left
    mesh.vertices[16] = 0.0f;
    mesh.vertices[17] = spriteScreenY2;
    mesh.vertices[18] = 0.0f;

    mesh.vertices[19] = 1.0f;
    mesh.vertices[20] = 1.0f;
    mesh.vertices[21] = 1.0f;

    mesh.vertices[22] = spriteTextureX1;
    mesh.vertices[23] = spriteTextureY2;

    // bottom left
    mesh.vertices[24] = 0;
    mesh.vertices[25] = 0;
    mesh.vertices[26] = 0.0f;

    mesh.vertices[27] = 1.0f;
    mesh.vertices[28] = 1.0f;
    mesh.vertices[29] = 1.0f;

    mesh.vertices[30] = spriteTextureX1;
    mesh.vertices[31] = spriteTextureY1;        

    mesh.indices[0] = 0;
    mesh.indices[1] = 1;
    mesh.indices[2] = 3;

    mesh.indices[3] = 1;
    mesh.indices[4] = 2;
    mesh.indices[5] = 3;

    mesh.vertCount += 4;
    mesh.indexCount += 6;
}
