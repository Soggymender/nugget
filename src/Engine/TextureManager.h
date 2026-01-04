#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "Engine/Singleton.h"

#include "glad/glad.h"

#include <string>
#include <unordered_map>

using namespace std;

class Texture;

class NTextureManager : public NSingleton<NTextureManager>
{
    friend class NSingleton<NTextureManager>;

public:

    Texture* Load(string const& filename, const string& workingDir);

private:

    // Can't make your own.
    NTextureManager() = default;

    unordered_map<string, Texture*> textures;
};

#endif