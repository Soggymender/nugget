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

class NScene;
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

        virtual NEntity* PreProcessEntity(string name, unordered_map<string, void*>const& properties, NScene* scene) = 0;
        virtual void PostProcessEntity(NEntity* pEntity, string parentName, unordered_map<string, void*>const& properties, NScene* scene) = 0;
    };

    void LoadScene(string const& path, ICustomProcessor* pCustomProcessor = nullptr, NScene* scene = nullptr, NEntity* pEntity = nullptr);

private:

    // Can't make your own.
    NSceneLoader() = default;

    void ProcessNode(aiNode* node, int depth, const aiScene* pImportScene, ICustomProcessor* pCustomProcessor, const string& workingDir, NScene* pScene, NEntity* curEntity);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* pImportScene, const string& workingDir);
    vector<Texture*> ProcessMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const string& workingDir);
};

#endif