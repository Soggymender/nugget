#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include "stb/stb_img.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "shader.h"

#include <string>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory);

class Model
{
public:
    vector<Mesh> meshes;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path);

    // draws the model, and thus all its meshes
    void Draw(Shader& shader);

private:
    void loadModel(string const& path);
    void processNode(aiNode* node, const aiScene* scene, const string& workingDir);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, const string& workingDir);
    vector<Texture*> processMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const string& workingDir);
};

#endif