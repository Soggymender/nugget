#include "workstation.h"

//#include "vm/vm.h"
#include "vm/assembler.h"

Workstation::Workstation()
{

}

void Workstation::Create()
{
    int i = 0;

    // 5x4 file grid.
    for (int y = 0; y < 4; y++) {

        vector<SourceFile> row;

        for (int x = 0; x < 5; x++) {

            string filename = "file" + std::to_string(i + 1) + ".asm";
            row.push_back(SourceFile(filename, x, y));

            i++;
        }

        sourceFiles.push_back(row);
    }
}

bool Workstation::BuildCurrentSourceFile()
{
    if (currentSourceFile == nullptr)
        return false;

    memset(image, 0, 0xFFFF);
    imageSize = 0;

    vm_assemble(currentSourceFile->contents.c_str(), image, imageSize);

    return imageSize > 0;
}
