#include "SceneLoader.h"

#include "Entity.h"
#include "Mesh.h"
#include "Scene.h"
#include "Texture.h"
#include "TextureManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb/stb_img.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>

using namespace std;

void NSceneLoader::LoadScene(string const& path, ICustomProcessor* pCustomProcessor, NScene* pScene, NEntity* pEntity)
{
    // Read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* pImportScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | /*aiProcess_FlipUVs |*/ aiProcess_CalcTangentSpace);

    // Check for errors
    if (!pImportScene || pImportScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pImportScene->mRootNode)
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }

    // Retrieve the directory path of the filepath
    string workingDir = path.substr(0, path.find_last_of('/'));

    // Process ASSIMP's root node recursively
    ProcessNode(pImportScene->mRootNode, 0, pImportScene, pCustomProcessor, workingDir, pScene, pEntity);
}

void GetNodeMetadata(aiNode* node, unordered_map<string, void*>& properties)
{
    if (node->mMetaData != nullptr)
    {
        for (int i = 0; i < node->mMetaData->mNumProperties; i++)
        {
            aiMetadata* meta = node->mMetaData;

            string key = meta->mKeys[i].C_Str();
            aiMetadataEntry* metaValue = &meta->mValues[i];

            if (key.rfind("p_", 0) == 0)
            {
                properties[key] = metaValue->mData;
            }
        }

        std::string p_type;

        if (properties.find("p_type") != properties.end())
        {
            auto metaString = static_cast<const aiString*>(properties["p_type"]);
            p_type = metaString->C_Str();
        }
    }
}

void NSceneLoader::ProcessNode(aiNode* node, int depth, const aiScene* pImportScene, ICustomProcessor* pCustomProcessor, const string& workingDir, NScene* pScene, NEntity* curEntity)
{
    string name = node->mName.C_Str();

    unordered_map<string, void*> properties;
    GetNodeMetadata(node, properties);

    // Blender makes similar names unique by adding .001 etc. Strip it.
    //string entityName = toLowerCase().split("\\.")[0];

    if (depth == 0)
    {
        // Process children.
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], depth + 1, pImportScene, pCustomProcessor, workingDir, pScene, curEntity);
        }
    }
    else
    {
        // The call can pass in a single entity, or we can call back to request them one at a time.
        if (pCustomProcessor != nullptr && curEntity == nullptr)
            curEntity = pCustomProcessor->PreProcessEntity(name, properties, pScene);

        if (curEntity != nullptr)
        {
            // Process meshes in this node.
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* aiMesh = pImportScene->mMeshes[node->mMeshes[i]];

                curEntity->meshes.push_back(ProcessMesh(aiMesh, pImportScene, workingDir));
            }

            // Process children.
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                ProcessNode(node->mChildren[i], depth + 1, pImportScene, pCustomProcessor, workingDir, pScene, curEntity);
            }

            if (pCustomProcessor != nullptr)
                pCustomProcessor->PostProcessEntity(curEntity, "none", properties, pScene);
        }
    }
}   

Mesh NSceneLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const string& workingDir)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture*> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // 1. diffuse maps
    vector<Texture*> diffuseMaps = ProcessMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", workingDir);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture*> specularMaps = ProcessMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", workingDir);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture*> normalMaps = ProcessMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", workingDir);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture*> heightMaps = ProcessMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", workingDir);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}

vector<Texture*> NSceneLoader::ProcessMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const string& workingDir)
{
    vector<Texture*> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        Texture* texture = NTextureManager::Instance().Load(str.C_Str(), workingDir);
        if (texture != nullptr)
            textures.push_back(texture);
    }

    return textures;
}
