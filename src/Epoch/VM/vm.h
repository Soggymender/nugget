#pragma once

#include <stdio.h>

void exec_instr(unsigned short instr);

// Run from raw image.
void vm_run(unsigned short* image);

// Run from image file.
int vm_run_from_file(char* imageFilename);

// Run after image is loaded into vm memory.
int vm_main();

int read_image(const char* image_path, bool useOrigin, unsigned short* memory);
unsigned short swap16(unsigned short x);

bool vm_read_memory(unsigned short status_addr, unsigned short addr, unsigned short& value);
void vm_write_memory(unsigned short status_addr, unsigned short addr, unsigned short value);