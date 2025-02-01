#include "vm.h"

#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <Windows.h>
#include <conio.h>
#include <thread>

#define MEMORY_MAX (1 << 16)
unsigned short memory[MEMORY_MAX];

enum
{
	R_R0 = 0,
	R_R1,
	R_R2,
	R_R3,
	R_R4,
	R_R5,
	R_R6,
	R_R7,
	R_PC,
	R_COND,
	R_COUNT
};

unsigned short reg[R_COUNT];

enum
{
	OP_BR = 0, //	0000 BR
	OP_ADD, // 1	0001 AND
	OP_LD,  // 2	0010 LD
	OP_ST,  // 3	0011 ST
	OP_JSR, // 4	0100 JSR, JSRR
	OP_AND, // 5	0101 AND
	OP_LDR, // 6	0110 LDR
	OP_STR, // 7	0111 STR

	OP_RTI, // 8	1000 RTI
	OP_NOT, // 9	1001 NOT
	OP_LDI, // 10	1010 LDI
	OP_STI, // 11	1011 STI

	OP_JMP, // 12	1100 JMP, RET
	OP_RES, // 13	1101 reserved
	OP_LEA, // 14	1110 LEA

	OP_TRAP // 15	1111 TRAP
};

enum
{
	FL_POS = 1 << 0,
	FL_ZRO = 1 << 1,
	FL_NEG = 1 << 2,
};

enum
{
	TRAP_GETC = 0x20,
	TRAP_OUT = 0x21,
	TRAP_PUTS = 0x22,
	TRAP_IN = 0x23,
	TRAP_PUTSP = 0x24,
	TRAP_HALT = 0x25
};

enum
{
	MR_KBSR = 0xFE00, /* keyboard status */
	MR_KBDR = 0xFE02  /* keyboard data */
};

unsigned short instr;
int running = 1;

bool showLogs = false;
void log(const char* message)
{
	if (showLogs)
		printf(message);
}

unsigned short swap16(unsigned short x)
{
	return (x << 8) | (x >> 8);
}

void read_image(unsigned short* image, bool useOrigin, unsigned short* progMem)
{
	/* the origin tells us where in memory to place the image */
	unsigned short origin = image[0];

	if (!useOrigin) {
		progMem[0] = origin;
		origin = 1;
	}

	/* we know the maximum file size so we only need one fread */
	unsigned short max_read = (1 << 16) - origin - 1;

	unsigned short* p = progMem + origin;
	memcpy(p, &image[1], max_read);
}

void read_image_file(FILE* file, bool useOrigin, unsigned short* progMem)
{
	/* the origin tells us where in memory to place the image */
	unsigned short origin = 0;

	fread(&origin, sizeof(origin), 1, file);
	origin = swap16(origin);
	if (!useOrigin) {
		progMem[0] = origin;
		origin = 1;
	}

	/* we know the maximum file size so we only need one fread */
	unsigned short max_read = (1 << 16) - origin - 1;

	unsigned short* p = progMem + origin;
	size_t read = fread(p, sizeof(unsigned short), max_read, file);

	/* swap to little endian */
	while (read-- > 0)
	{
		*p = swap16(*p);
		++p;
	}
}

int read_image(const char* image_path, bool useOrigin, unsigned short* progMem)
{
#define _CRT_SECURE_NO_WARNINGS
	FILE* file = fopen(image_path, "rb");
	if (!file) { return 0; };
	read_image_file(file, useOrigin, progMem);
	fclose(file);
	return 1;
}

HANDLE hStdin = INVALID_HANDLE_VALUE;
DWORD fdwMode, fdwOldMode;

void disable_input_buffering()
{
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hStdin, &fdwOldMode); /* save old mode */
	fdwMode = fdwOldMode
		^ ENABLE_ECHO_INPUT  /* no input echo */
		^ ENABLE_LINE_INPUT; /* return when one or
								more characters are available */
	SetConsoleMode(hStdin, fdwMode); /* set new mode */
	FlushConsoleInputBuffer(hStdin); /* clear buffer */
}

void restore_input_buffering()
{
	SetConsoleMode(hStdin, fdwOldMode);
}

unsigned short check_key()
{
	return WaitForSingleObject(hStdin, 1000) == WAIT_OBJECT_0 && _kbhit();
}

void mem_write(unsigned short address, unsigned short val)
{
	memory[address] = val;
}

unsigned short mem_read(unsigned short address)
{
	if (address == MR_KBSR)
	{
		if (check_key())
		{
			memory[MR_KBSR] = (1 << 15);
			memory[MR_KBDR] = getchar();
		}
		else
		{
			memory[MR_KBSR] = 0;
		}
	}
	return memory[address];
}

void handle_interrupt(int signal)
{
	restore_input_buffering();
	log("\n");
	exit(-2);
}
unsigned short sext(unsigned short x, int bit_count = 5) {

	if ((x >> (bit_count - 1)) && 1)
		x |= (0xFFFF << bit_count);

	return x;
}

void setcc(unsigned short r) {

	if (reg[r] == 0)
		reg[R_COND] = FL_ZRO;
	else if (reg[r] >> 15)
		reg[R_COND] = FL_NEG;
	else
		reg[R_COND] = FL_POS;
}

void op_add() {

	unsigned short dr = (instr >> 9) & 0x7;
	unsigned short sr1 = (instr >> 6) & 0x7;

	if ((instr >> 5) & 0x1) {
		reg[dr] = reg[sr1] + sext(instr & 0x1F);
	}
	else {
		unsigned short sr2 = instr & 0x7;
		reg[dr] = reg[sr1] + reg[sr2];
	}

	setcc(dr);
}

void op_and() {

	unsigned short dr = (instr >> 9) & 0x7;
	unsigned short sr1 = (instr >> 6) & 0x7;

	if ((instr >> 5) & 0x1) {
		reg[dr] = reg[sr1] & sext(instr & 0x1F, 5);
	}
	else {
		unsigned short sr2 = instr & 0x7;
		reg[dr] = reg[sr1] & reg[sr2];
	}

	setcc(dr);
}

void op_not() {

	unsigned short dr = (instr >> 9) & 0x7;
	unsigned short sr = (instr >> 6) & 0x7;
	reg[dr] = ~reg[sr];
	setcc(dr);
}

void op_br() {

	unsigned short n = (instr >> 9) & 0x4;
	unsigned short z = (instr >> 9) & 0x2;
	unsigned short p = (instr >> 9) & 0x1;

	unsigned short N = reg[R_COND] & FL_NEG;
	unsigned short Z = reg[R_COND] & FL_ZRO;
	unsigned short P = reg[R_COND] & FL_POS;

	if ((n & reg[R_COND]) || (z & reg[R_COND]) || (p & reg[R_COND]))
		reg[R_PC] = reg[R_PC] + sext(instr & 0x1FF, 9);
}

void op_jmp() {

	unsigned short baseR = (instr >> 6) & 0x7;
	reg[R_PC] = reg[baseR];
}

void op_jsr() {

	reg[R_R7] = reg[R_PC];

	if ((instr >> 11) & 0x1) {
		reg[R_PC] = reg[R_PC] + sext(instr & 0x7FF, 11);
	}
	else {
		unsigned short baseR = (instr >> 6) & 0x7;
		reg[R_PC] = reg[baseR];
	}
}

void op_ld() {

	unsigned short dr = (instr >> 9) & 0x7;
	unsigned short pcOffset9 = (instr & 0x1FF);
	reg[dr] = mem_read(reg[R_PC] + sext(pcOffset9, 9));
	
	setcc(dr);
}

void op_ldi() {

	unsigned short dr = (instr >> 9) & 0x7;
	unsigned short pcOffset9 = (instr & 0x1FF);
	reg[dr] = mem_read(mem_read(reg[R_PC] + sext(pcOffset9, 9)));
	setcc(dr);  
}

void op_ldr() {

	unsigned short dr = (instr >> 9) & 0x7;
	unsigned short baseR = (instr >> 6) & 0x7;
	unsigned short offset6 = (instr & 0x3F);
	reg[dr] = mem_read(reg[baseR] + sext(offset6, 6));
	setcc(dr);
}

void op_lea() {

	unsigned short dr = (instr >> 9) & 0x7;
	unsigned short pcOffset = instr & 0x1FF;
	reg[dr] = reg[R_PC] + sext(pcOffset, 9);
	setcc(dr);
}

void op_st() {

	unsigned short pcOffset = instr & 0x1FF;
	
	unsigned short addr = reg[R_PC] + sext(pcOffset, 9);
	unsigned short val = (instr >> 9) & 0x7;
	
	mem_write(addr, reg[val]);
}

void op_sti() {

	unsigned short pcOffset = instr & 0x1FF;

	unsigned short addr = reg[R_PC] + sext(pcOffset, 9);
	unsigned short val = (instr >> 9) & 0x7;

	mem_write(mem_read(addr), reg[val]);
}

void op_str() {

	unsigned short baseR = (instr >> 6) & 0x7;
	unsigned short offset6 = instr & 0x3F;

	unsigned short addr = reg[baseR] + sext(offset6, 6);
	unsigned short val  = (instr >> 9) & 0x7;

	mem_write(addr, reg[val]);
}

void op_trap() {

	/*
	unsigned int trapvect8 = instr & 0xFF;

	reg[R_R7] = reg[R_PC];
	reg[R_PC] = memory[zext(trapvect8, 8)];
	*/

	reg[R_R7] = reg[R_PC];

	switch (instr & 0xFF) {

	case TRAP_GETC: {

		reg[R_R0] = (unsigned short)getchar();
		setcc(R_R0);

		break;
	}

	case TRAP_OUT: {

		putc((char)reg[R_R0], stdout);
		fflush(stdout);
		break;
	}

	case TRAP_PUTS: {

		unsigned short* c = memory + reg[R_R0];
		while (*c) {
			putc((char)*c, stdout);
			++c;
		}

		fflush(stdout);

		break;
	}

	case TRAP_IN: {

		log("Enter a character: ");
		char c = getchar();
		putc(c, stdout);
		fflush(stdout);
		reg[R_R0] = (unsigned short)c;
		setcc(R_R0);
		break;
	}

	case TRAP_PUTSP: {
		
		unsigned short* c = memory + reg[R_R0];
		while (*c) {

			char char1 = (*c) & 0xFF;
			putc(char1, stdout);
			char char2 = (*c) >> 8;
			if (char2)
				putc(char2, stdout);
			++c;
		}

		fflush(stdout);
		
		break;
	}

	case TRAP_HALT: {

		puts("HALT");
		fflush(stdout);
		running = 0;
		break;
	}


	}
}

void op_rti() {

}

void exec_instr(unsigned short cur_instr) {

	instr = cur_instr;

	unsigned short op = instr >> 12;

	switch (op) {

	case OP_ADD:
		log("add");
		op_add();
		break;

	case OP_AND:
		log("and");
		op_and();
		break;

	case OP_NOT:
		log("not");
		op_not();
		break;

	case OP_BR:
		log("br");
		op_br();
		break;

	case OP_JMP:
		log("jmp");
		op_jmp();
		break;

	case OP_JSR:
		log("jsr");
		op_jsr();
		break;

	case OP_LD:
		log("ld");
		op_ld();
		break;

	case OP_LDI:
		log("ldi");
		op_ldi();
		break;

	case OP_LDR:
		log("ldr");
		op_ldr();
		break;

	case OP_LEA:
		log("lea");
		op_lea();
		break;

	case OP_ST:
		log("st");
		op_st();
		break;

	case OP_STI:
		log("sti");
		op_sti();
		break;

	case OP_STR:
		log("str");
		op_str();
		break;

	case OP_TRAP:
		log("trap");
		op_trap();
		break;

	case OP_RTI:
		log("rti");
		op_rti();

	case OP_RES:
		log("res");
	default:
		break;
	}
}

void vm_run(unsigned short* image)
{
	signal(SIGINT, handle_interrupt);
	disable_input_buffering();

	read_image(image, true, memory);

	std::thread t(vm_main);
	t.detach();
}

int vm_run_from_file(char* imageFilename)
{
	signal(SIGINT, handle_interrupt);
	disable_input_buffering();

	read_image("assets\\programs\\2048.obj", true, memory);

	return vm_main();
}

int vm_main() {

	reg[R_COND] = FL_ZRO;

	enum { PC_START = 0x3000 };
	reg[R_PC] = PC_START;

	while (running) {

		// FETCH

		unsigned short cur_instr = mem_read(reg[R_PC]++);

		if (cur_instr == 0)
			break;

		exec_instr(cur_instr);

		log("\n");
	}

	restore_input_buffering();

	return 0;
}

bool vm_read_memory(unsigned short status_addr, unsigned short addr, unsigned short& value)
{
	if (memory[status_addr] == 1) {
		memory[status_addr] = 0;

		value = memory[addr];

		return true;
	}

	value = 0;
	return false;
}

void vm_write_memory(unsigned short status_addr, unsigned short addr, unsigned short value)
{
	memory[status_addr] = 1;

	memory[addr] = value;
}