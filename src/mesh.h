#ifndef MESH_H
#define MESH_H

#include "shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

struct BoundingBox
{
    glm::vec3 min;
    glm::vec3 max;
};


class Mesh
{
public:
    Mesh() { }
 
    bool Create(unsigned int textureId, Shader* shader);
    void Destroy();

    void Render();

    void CalculateBoundingBox();

    unsigned int textureId;
    Shader* shader = nullptr;

    float* vertices = nullptr;
    int verticesSize = 0;

    unsigned int* indices = nullptr;
    int indicesSize = 0;

    int vertCount = 0;
    int indexCount = 0;

    unsigned int VBO, VAO, EBO;

    BoundingBox boundingBox;
};

#endif