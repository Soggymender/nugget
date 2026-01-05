#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "Singleton.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "glad/glad.h"

#include <string>
#include <unordered_map>

using namespace std;

struct aiScene;
struct aiNode;

struct NEntity;
class Mesh;
struct Texture;

class NSceneLoader : public NSingleton<NSceneLoader>
{
    friend class NSingleton<NSceneLoader>;

public:

    class ICustomProcessor
    {
    public:

        virtual NEntity* PreProcessEntity(string name, unordered_map<string, void*>const& properties) = 0;
        virtual void PostProcessEntity(NEntity* pEntity, string parentName, unordered_map<string, void*>const& properties) = 0;
    };

    void LoadScene(string const& path, ICustomProcessor* pCustomProcessor = nullptr);

private:

    // Can't make your own.
    NSceneLoader() = default;

    void ProcessNode(aiNode* node, int depth, const aiScene* scene, ICustomProcessor* pCustomProcessor, const string& workingDir, NEntity* curEntity);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const string& workingDir);
    vector<Texture*> ProcessMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const string& workingDir);
};

#endif