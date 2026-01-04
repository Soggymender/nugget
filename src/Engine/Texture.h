#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

using namespace std;

typedef unsigned int GLuint;
GLuint LoadTextureFromFile(const char* filename);

struct Texture
{
    unsigned int id;
 
    string type;
    string path;
};

#endif