#ifndef WORKSTATION_H
#define WORKSTATION_H

#include <string>
#include <vector>

#include "source_file.h"

using namespace std;

class Workstation
{
public:

    // Source files

    int maxSourceFiles = 20;
    vector<vector<SourceFile>> sourceFiles;

    SourceFile* currentSourceFile = nullptr;

    // Builds
    unsigned short imageSize = 0;
    unsigned short image[0xFFFF];

    Workstation();

    void Create();

    bool BuildCurrentSourceFile();
};

#endif