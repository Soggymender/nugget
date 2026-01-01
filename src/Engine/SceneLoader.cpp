#include "SceneLoader.h"
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

void NSceneLoader::LoadScene(string const& path, ICustomProcessor* pCustomProcessor)
{
    // Read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | /*aiProcess_FlipUVs |*/ aiProcess_CalcTangentSpace);

    // Check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }

    // Retrieve the directory path of the filepath
    string directory = path.substr(0, path.find_last_of('/'));

    // Process ASSIMP's root node recursively
    ProcessNode(scene->mRootNode, scene, pCustomProcessor);
}

void NSceneLoader::ProcessNode(aiNode* node, const aiScene* scene, ICustomProcessor* pCustomProcessor)
{
    if (node->mMetaData != nullptr)
    {
        unordered_map<string, void*> properties;

        string name = node->mName.C_Str();

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

        auto metaString = static_cast<const aiString*>(properties["p_type"]);
        std::string p_type = metaString->C_Str();
 
        // Blender makes similar names unique by adding .001 etc. Strip it.
        //string entityName = toLowerCase().split("\\.")[0];

        if (pCustomProcessor != nullptr)
        {
            NEntity* pEntity = pCustomProcessor->PreProcessEntity(name, properties);

            // Do our stuff.

            pCustomProcessor->PostProcessEntity(pEntity, "none", properties);
        }
     }   











    // Process meshes in this node.
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        //meshes.push_back(processMesh(mesh, scene));
    }

    // Process children.
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}
