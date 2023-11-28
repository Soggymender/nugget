#include "tileset.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_img.h"

#include <iostream>

bool Tileset::Create(const char* filename)
{
    int textureWidth, textureHeight, nrChannels;

    stbi_set_flip_vertically_on_load(true);     
    unsigned char *data = stbi_load(filename, &textureWidth, &textureHeight, &nrChannels, 0);

    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    textureTileWidth = float(16) / float(textureWidth);
    textureTileHeight = float(16) / float(textureHeight);

    int tilesPerRow = textureWidth / 16;
    int tilesPerColumn = textureHeight / 16;

    int tileId = 0;

    tiles = new Tile[tilesPerRow * tilesPerColumn];

    for (int y = 0; y < tilesPerColumn; y++) {

        for (int x = 0; x < tilesPerRow; x++) {

            tiles[tileId].u = (float)x * textureTileWidth;
            tiles[tileId].v = (tilesPerColumn - y - 1) * textureTileHeight;

            tileId++;
        }
    }

    return true;
}

void Tileset::Destroy()
{

}

