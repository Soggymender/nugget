#include "tileMap.h"

#include "engine/shader.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_img.h"

#include "libnoise/noise.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>

using namespace noise;

#define TILE_ID_WATER_SHALLOW (129)
#define TILE_ID_WATER (225)
#define TILE_ID_WATER_DEEP (321)

#define TILE_ID_GRASS (236)
#define TILE_ID_SAND   (44)
#define TILE_ID_ROCK  (51)



static float EaseIn(float t)
{
    return t * t;
}

static float Flip(float x)
{
    return 1 - x;
}

static float Square(float x)
{
    return x * x;
}

static float Pow(float x, float e)
{
    return (powf(x, e));
}

static float EaseOut(float t)
{
    return Flip(Pow(Flip(t), 3.0f));
}




bool TileMap::Create(Shader* shader, const char* mapGroundFilename, const char* mapSurfaceFilename, unsigned int width, unsigned int height, const char* tilesetFilename)
{
    std::ifstream mapGroundInput{mapGroundFilename};
    if (!mapGroundInput.is_open()) {
        std::cerr << "Couldn't read file: " << mapGroundFilename << "\n";
        return false; 
    }

    std::ifstream mapSurfaceInput{mapSurfaceFilename};
    if (!mapSurfaceInput.is_open()) {
        std::cerr << "Couldn't read file: " << mapSurfaceFilename << "\n";
        return false; 
    }

    layers[0].tileIds = (short*)::operator new(width * height * sizeof(short));
    layers[1].tileIds = (short*)::operator new(width * height * sizeof(short));

    module::Perlin myModule;
    myModule.SetSeed(50);

    for (int y = 0; y < height; y++) {

        for (int x = 0; x < width; x++) {

            double value = myModule.GetValue((float)x / (float)width, (float)y / (float)height, 0);
            
            // distance from center
            glm::vec2 center((float)width / 2.0f, (float)height / 2.0f);
            glm::vec2 self((float)x, (float)y);

            glm::vec2 dir = self - center;
            dir = glm::normalize(dir);

            float d = glm::distance(self, center);
            float max_d;

            if (d != 0.0f) {
                double angle = atan2(dir.y, dir.x);
                double magnitude;
                double abs_cos_angle = fabs(cos(angle));
                double abs_sin_angle = fabs(sin(angle));
                if ((float)width / 2.0f * abs_sin_angle <= (float)height / 2.0f * abs_cos_angle)
                {
                    magnitude = (float)width / 2.0f / abs_cos_angle;
                }
                else
                {
                    magnitude = (float)height / 2.0f / abs_sin_angle;
                }

                double check_x = x + cos(angle) * magnitude;
                double check_y = y + sin(angle) * magnitude;

                if (d <= 0.1f)
                    magnitude = 0.0f;

                max_d = magnitude;
            }
            else
                max_d = width / 2.0f;



            // % of diameter.
            d = d / max_d;

            if (d > 1.0f)
                d = 1.0f;

            d = 1.0f - d;

            /*
            d *= 5.0f;
            if (d > 1.0f)
                d = 1.0f;
            */
            
            d = EaseOut(d);

            value = value * d;// (value + (1.0f - d)) / 2.0f;

            
            int tileId = -1;

            static float seaLevel = 0.25f;

            if (value > seaLevel) {

                if (value > seaLevel + 0.25f)
                    tileId = TILE_ID_ROCK;
                else if (value > seaLevel + 0.075f)
                    tileId = TILE_ID_GRASS;
                else
                    tileId = TILE_ID_SAND;
            }
            else {

                if (value > seaLevel - 0.075f)
                    tileId = TILE_ID_WATER_SHALLOW;
                else if (value > seaLevel - 0.185f)
                    tileId = TILE_ID_WATER;
                else
                    tileId = TILE_ID_WATER_DEEP;
            }




            layers[0].tileIds[layers[0].tileIdCount++] = tileId;
            layers[1].tileIds[layers[1].tileIdCount++] = -1;

        }
    }

    /*
    for (std::string line; std::getline(mapGroundInput, line);) {

        std::istringstream ss(std::move(line));
        
        for (std::string value; std::getline(ss, value, ',');) {
            layers[0].tileIds[layers[0].tileIdCount++] = std::stoi(value);
        }
    }

    for (std::string line; std::getline(mapSurfaceInput, line);) {

        std::istringstream ss(std::move(line));
        
        for (std::string value; std::getline(ss, value, ',');) {
            layers[1].tileIds[layers[1].tileIdCount++] = std::stoi(value);
        }
    }
    */

    int tileWidth = 16;
    int tileHeight = 16;

    int textureWidth, textureHeight, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    stbi_set_flip_vertically_on_load(true);     
    unsigned char *data = stbi_load(tilesetFilename, &textureWidth, &textureHeight, &nrChannels, 0);

    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //glEnable(GL_TEXTURE_2D) ;//glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    textureTileWidth = float(tileWidth) / float(textureWidth);
    textureTileHeight = float(tileHeight) / float(textureHeight);

    int tilesPerRow = textureWidth / tileWidth;
    int tilesPerColumn = textureHeight / tileHeight;
    mapWidth = width;
    mapHeight = height;

    int tileId = 0;

    tilesetPos = new TilesetPosition[tilesPerRow * tilesPerColumn];

    for (int y = 0; y < tilesPerColumn; y++) {

        for (int x = 0; x < tilesPerRow; x++) {

            tilesetPos[tileId].x = (float)x * textureTileWidth;
            tilesetPos[tileId].y = (tilesPerColumn - y - 1) * textureTileHeight;

            tileId++;
        }
    }

    layers[0].GenerateMesh(*this, 0.0f);
    layers[0].mesh.Create(textureId, shader);

    layers[1].GenerateMesh(*this, 0.01f);
    layers[1].mesh.Create(textureId, shader);

    model = glm::mat4(1.0f);

    return true;
}

void TileMap::Destroy()
{
    layers[0].mesh.Destroy();
    layers[1].mesh.Destroy();

}

void TileMap::Render()
{

    layers[0].mesh.shader->setMatrix("model", model);
    layers[0].mesh.Render();

    layers[1].mesh.shader->setMatrix("model", model);
    layers[1].mesh.Render();

}

void TileMapLayer::GenerateMesh(const TileMap& tileMap, float zOffset)
{
    int verticesCount = tileIdCount * 4 * 32;
    mesh.vertices = new float[verticesCount];
    mesh.verticesSize = verticesCount * sizeof(float);
    
    int indicesCount = tileIdCount * 2 * 3;
    mesh.indices = new unsigned int[indicesCount];
    mesh.indicesSize = indicesCount * sizeof(unsigned int);
  
    const unsigned int SCR_WIDTH = 1280;
    const unsigned int SCR_HEIGHT = 1024;

    int tileWidth = 16;
    int tileHeight = 16;
    //int screenWidth = SCR_WIDTH;
    int screenHeight = SCR_HEIGHT;
    float screenTileWidth = 2 * float(tileWidth) / float(screenHeight);
    float screenTileHeight = 2 * float(tileHeight) / float(screenHeight);
    //int screenSize = (screenWidth, screenHeight);

 
    int vi = 0;
    int ii = 0;

    for (int y = 0; y < tileMap.mapHeight; y++) {

        for (int x = 0; x < tileMap.mapWidth; x++) {

            int index = y * tileMap.mapWidth + x;

            int tileId = tileIds[index];
            if (tileId == -1) {
                continue;
            }

            float px = 1.0f / tileWidth;       // # one pixel in texture space
            float half_px = 0.0f;//(textureTileWidth / tileWidth) * 0.5f;//   px * 0.5f;      //# 1/2 pixel in texture space
            float py = 1.0f / tileHeight;       // # one pixel height in texture space
            float half_py = 0.0f;//(textureTileHeight / tileHeight) * 0.5f;      //# 1/2 pixel in texture space

            float spriteScreenX1 = (float)x * screenTileWidth;
            float spriteScreenY1 = -(float)y * screenTileHeight;
            float spriteScreenX2 = spriteScreenX1 + screenTileWidth;
            float spriteScreenY2 = spriteScreenY1 + screenTileHeight;



            float spriteTextureX1 = tileMap.tilesetPos[tileId].x + half_px;// * textureTileWidth;
            float spriteTextureY1 = tileMap.tilesetPos[tileId].y + half_py;// * textureTileHeight;
            float spriteTextureX2 = spriteTextureX1 + tileMap.textureTileWidth - half_px;
            float spriteTextureY2 = spriteTextureY1 + tileMap.textureTileHeight - half_py;

            // 
            mesh.vertices[vi + 0] = spriteScreenX2;
            mesh.vertices[vi + 1] = spriteScreenY1;
            mesh.vertices[vi + 2] = zOffset;

            mesh.vertices[vi + 3] = 1.0f;
            mesh.vertices[vi + 4] = 1.0f;
            mesh.vertices[vi + 5] = 1.0f;

            mesh.vertices[vi + 6] = spriteTextureX2;
            mesh.vertices[vi + 7] = spriteTextureY1;

            // 
            mesh.vertices[vi + 8] = spriteScreenX2;
            mesh.vertices[vi + 9] = spriteScreenY2;
            mesh.vertices[vi + 10] = zOffset;

            mesh.vertices[vi + 11] = 1.0f;
            mesh.vertices[vi + 12] = 1.0f;
            mesh.vertices[vi + 13] = 1.0f;

            mesh.vertices[vi + 14] = spriteTextureX2;
            mesh.vertices[vi + 15] = spriteTextureY2;

            //
            mesh.vertices[vi + 16] = spriteScreenX1;
            mesh.vertices[vi + 17] = spriteScreenY2;
            mesh.vertices[vi + 18] = zOffset;

            mesh.vertices[vi + 19] = 1.0f;
            mesh.vertices[vi + 20] = 1.0f;
            mesh.vertices[vi + 21] = 1.0f;

            mesh.vertices[vi + 22] = spriteTextureX1;
            mesh.vertices[vi + 23] = spriteTextureY2;

            //
            mesh.vertices[vi + 24] = spriteScreenX1;
            mesh.vertices[vi + 25] = spriteScreenY1;
            mesh.vertices[vi + 26] = zOffset;

            mesh.vertices[vi + 27] = 1.0f;
            mesh.vertices[vi + 28] = 1.0f;
            mesh.vertices[vi + 29] = 1.0f;

            mesh.vertices[vi + 30] = spriteTextureX1;
            mesh.vertices[vi + 31] = spriteTextureY1;        

            vi += 32;

            mesh.indices[ii + 0 ] = mesh.vertCount + 0;
            mesh.indices[ii + 1 ] = mesh.vertCount + 1;
            mesh.indices[ii + 2 ] = mesh.vertCount + 3;

            mesh.indices[ii + 3 ] = mesh.vertCount + 1;
            mesh.indices[ii + 4 ] = mesh.vertCount + 2;
            mesh.indices[ii + 5 ] = mesh.vertCount + 3;

            ii += 6;

            mesh.vertCount += 4;
            mesh.indexCount += 6;
        }
    }

    mesh.CalculateBoundingBox();
}

glm::vec3 TileMap::GetCenter()
{
    glm::vec3 distance = (layers[0].mesh.boundingBox.max - layers[0].mesh.boundingBox.min);
    return layers[0].mesh.boundingBox.min + (distance * 0.5f);
}
