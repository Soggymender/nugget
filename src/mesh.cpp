#include "mesh.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

bool Mesh::Create(unsigned int textureId, Shader* shader)
{
    this->textureId = textureId;
    this->shader = shader;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);    

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return true;
}

void Mesh::Destroy()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Mesh::Render()
{
    shader->use();

    if (textureId > 0) {
        glBindTexture(GL_TEXTURE_2D, textureId);
    }
    
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::CalculateBoundingBox()
{
    boundingBox.min.x = 9999.99f;
    boundingBox.min.y = 9999.99f;
    boundingBox.min.z = 9999.99f;

    boundingBox.max.x = -9999.99f;
    boundingBox.max.y = -9999.99f;
    boundingBox.max.z = -9999.99f;

    for (int i = 0; i < vertCount * 8; i += 8)
    {
        // x
        if (vertices[i] > boundingBox.max.x)
            boundingBox.max.x = vertices[i];
        else if (vertices[i] < boundingBox.min.x)
            boundingBox.min.x = vertices[i];

        // y
        if (vertices[i + 1] > boundingBox.max.y)
            boundingBox.max.y = vertices[i + 1];
        else if (vertices[i + 1] < boundingBox.min.y)
            boundingBox.min.y = vertices[i + 1];

        // z
        if (vertices[i + 2] > boundingBox.max.z)
            boundingBox.max.z = vertices[i + 2];
        else if (vertices[i + 2] < boundingBox.min.z)
            boundingBox.min.z = vertices[i + 2];
    }






}