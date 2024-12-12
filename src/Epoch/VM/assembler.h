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

};

void vm_assemble(char* program, unsigned short* image, unsigned short& imageSize, std::vector<Line>& programLines);