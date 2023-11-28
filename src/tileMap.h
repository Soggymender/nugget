#include "mesh.h"

#include "shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class TileMap;

class TileMapLayer
{
    friend class TileMap;

protected:

    void GenerateMesh(const TileMap& tileMap, float zOffset);

public:

    unsigned int    tileIdCount = 0;
    short* tileIds = nullptr;

    Mesh mesh;
};

class TileMap
{

public:
    TileMap() { }

    bool Create(Shader* shader, const char* mapGroundFilename, const char* mapSurfaceFilename, unsigned int width, unsigned int height, const char* tilesetFilename);
    void Destroy();

    void Render();

    glm::vec3 GetCenter();

    class TilesetPosition
    {
    public:
        float x;
        float y;
    };

    TilesetPosition* tilesetPos; 

    TileMapLayer layers[2];

    short mapWidth = 0;
    short mapHeight = 0;

    float textureTileWidth; 
    float textureTileHeight;

    glm::mat4 model;
};