#include "source_file.h"

#include <fstream>
#include <sstream>

SourceFile::SourceFile(string filename, int x, int y)
{
    this->filename = filename;
    this->x = x;
    this->y = y;

    std::ifstream file("files/" + filename);

    if (file.good()) {

        std::stringstream buffer;
        buffer << file.rdbuf();
        uneditedContents = buffer.str();

        file.close();
    }
    else {
        this->uneditedContents = ";Header\\n.ORIG x3000\\nMAIN";
    }

    // Copy to edit buffer.
    this->contents = this->uneditedContents;
}

void SourceFile::RevertContents()
{
    contents = uneditedContents;
}

void SourceFile::SaveContents()
{
    uneditedContents = contents;

    std::ofstream file("files/" + filename);

    if (file.is_open()) {

        file.write(contents.c_str(), contents.length());

        //file.write();
        file.close();
    }
}
