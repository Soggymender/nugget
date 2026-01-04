#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "Singleton.h"

#include "glad/glad.h"

#include <string>
#include <unordered_map>

using namespace std;

struct aiScene;
struct aiNode;

struct NEntity;

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

    void ProcessNode(aiNode* node, const aiScene* scene, ICustomProcessor* pCustomProcessor = nullptr);
};

#endif