#ifndef SOURCE_FILE_H
#define SOURCE_FILE_H

#include <string>

using namespace std;

class SourceFile
{
public:

    string filename;

    int x, y;

    string uneditedContents;
    string contents;

    SourceFile(string filename, int x, int y);

    void RevertContents();
    void SaveContents();
};

#endif