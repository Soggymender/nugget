#ifndef SPRITE_H
#define SPRITE_H

#include "engine/SimpleMesh.h"

#include "engine/shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Sprite
{
public:
    Sprite() { }

    bool Create(Shader* shader, const char* mapFilename, unsigned int width, unsigned int height, const char* tilesetFilename);
    void Destroy();

    void Update(float deltaTime);
    void Render();
    
    SimpleMesh mesh;

    short mapWidth = 0;
    short mapHeight = 0;

    short width = 16;
    short height = 16;

private:

    class TilesetPosition
    {
    public:
        float x;
        float y;
    };

    void GenerateMesh();

public:

    TilesetPosition* tilesetPos;

    float spriteUvWidth; 
    float textureTileHeight;

    unsigned int    tileIdCount = 0;
    unsigned short* tileIds = nullptr;

    glm::mat4 model;
    glm::vec3 pos;
    float depthOffset = 0.0f;
};

#endif