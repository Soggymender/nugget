#pragma once

#include <stdio.h>

int vm_main(char* imageFilename);

int read_image(const char* image_path, bool useOrigin, unsigned short* memory);
unsigned short swap16(unsigned short x);
