#ifndef SIMPLE_MESH_H
#define SIMPLE_MESH_H

#include "shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

struct BoundingBox
{
    glm::vec3 min = {};
    glm::vec3 max = {};
};

class SimpleMesh
{
public:
    SimpleMesh() { }
 
    bool Create(unsigned int textureId, Shader* shader);
    void Destroy();

    void Render();

    void CalculateBoundingBox();

    unsigned int textureId = 0;
    Shader* shader = nullptr;

    float* vertices = nullptr;
    int verticesSize = 0;

    unsigned int* indices = nullptr;
    int indicesSize = 0;

    int vertCount = 0;
    int indexCount = 0;

    unsigned int VBO = 0, VAO = 0, EBO = 0;

    BoundingBox boundingBox = {};
};

#endif