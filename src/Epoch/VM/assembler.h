#pragma once

#include <vector>
#include <string>

using namespace std;

class Line {

public:

    int lineNumber;
    string sentence;

    std::vector<string> words;
    unsigned short address;
    unsigned short numWords = 0;
    unsigned short numInstWords = 0;
    unsigned short firstInstWord = 0;
};

void vm_assemble(const char* program, unsigned short* image, unsigned short& imageSize, unsigned short* testImage = nullptr);