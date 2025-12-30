#pragma once

#include <stdio.h>

// Run from raw image.
void vm_run(unsigned short* image);

// Run from image file.
int vm_run_from_file(char* imageFilename);

// Run after image is loaded into vm memory.
int vm_main();

bool vm_read_memory(unsigned short status_addr, unsigned short addr, unsigned short& value);
void vm_write_memory(unsigned short status_addr, unsigned short addr, unsigned short value);