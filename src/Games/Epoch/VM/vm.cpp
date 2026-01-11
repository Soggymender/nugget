/*
Learning resources and references:
https://www.jmeiners.com/lc3-vm/

Note that this is a learning project. My intention was to use reference material to learn
(someone's version of) correct process for writing and processing a VM before branching
out on my own and creating a VM for a custom language.
*/

#include "vm.h"

#include <stdint.h>
#include <signal.h>
#include <Windows.h>
#include <conio.h>
#include <thread>

typedef unsigned short ushort;

ushort memory[(1 << 16)];

// Registers enum.
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

// Registers.
ushort reg[R_COUNT];

// Opcode enum.
enum
{
	OP_BR = 0,	//		0000 BR
	OP_ADD,		// 1	0001 AND
	OP_LD,		// 2	0010 LD
	OP_ST,		// 3	0011 ST
	OP_JSR,		// 4	0100 JSR, JSRR
	OP_AND,		// 5	0101 AND
	OP_LDR,		// 6	0110 LDR
	OP_STR,		// 7	0111 STR
	OP_RTI,		// 8	1000 RTI
	OP_NOT,		// 9	1001 NOT
	OP_LDI,		// 10	1010 LDI
	OP_STI,		// 11	1011 STI
	OP_JMP,		// 12	1100 JMP, RET
	OP_RES,		// 13	1101 reserved
	OP_LEA,		// 14	1110 LEA
	OP_TRAP		// 15	1111 TRAP
};

// Flag enum.
enum
{
	FL_POS = 1 << 0,
	FL_ZRO = 1 << 1,
	FL_NEG = 1 << 2,
};

// Trap enum.
enum
{
	TRAP_GETC	= 0x20,
	TRAP_OUT	= 0x21,
	TRAP_PUTS	= 0x22,
	TRAP_IN		= 0x23,
	TRAP_PUTSP	= 0x24,
	TRAP_HALT	= 0x25
};

enum
{
	MR_KBSR = 0xFE00, // Keyboard status
	MR_KBDR = 0xFE02  // Keyboard data
};

ushort inst; // Currently executing instruction.
bool running = true; // Running / Not Running.

bool showLogs = false;
void log(const char* message)
{
	if (showLogs)
		printf(message);
}

ushort to_little_endian(ushort x)
{
	return (x << 8) | (x >> 8);
}

void read_image(ushort* image, bool useOrigin, ushort* progMem)
{
	ushort origin = image[0];

	if (!useOrigin) {
		progMem[0] = origin;
		origin = 1;
	}

	ushort max_read = (1 << 16) - origin - 1;

	ushort* p = progMem + origin;
	memcpy(p, &image[1], max_read);
}

void read_image_file(FILE* file, bool useOrigin, ushort* progMem)
{
	ushort origin = 0;

	fread(&origin, sizeof(origin), 1, file);
	origin = to_little_endian(origin);
	if (!useOrigin) {
		progMem[0] = origin;
		origin = 1;
	}

	ushort max_read = (1 << 16) - origin - 1;

	ushort* p = progMem + origin;
	size_t read = fread(p, sizeof(ushort), max_read, file);

	while (read-- > 0)
	{
		*p = to_little_endian(*p);
		++p;
	}
}

int read_image(const char* image_path, bool useOrigin, ushort* progMem)
{
#define _CRT_SECURE_NO_WARNINGS
	
	FILE* file;
	fopen_s(&file, image_path, "rb");
	if (!file) {
		return 0;
	};

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

ushort check_key()
{
	return WaitForSingleObject(hStdin, 1000) == WAIT_OBJECT_0 && _kbhit();
}

void mem_write(ushort address, ushort val)
{
	memory[address] = val;
}

ushort mem_read(ushort address)
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
ushort sext(ushort x, int bit_count = 5) {

	if ((x >> (bit_count - 1)) && 1)
		x |= (0xFFFF << bit_count);

	return x;
}

void setcc(ushort r) {

	if (reg[r] == 0)
		reg[R_COND] = FL_ZRO;
	else if (reg[r] >> 15)
		reg[R_COND] = FL_NEG;
	else
		reg[R_COND] = FL_POS;
}

void op_add() {

	ushort dr = (inst >> 9) & 0x7;
	ushort sr1 = (inst >> 6) & 0x7;

	if ((inst >> 5) & 0x1) {
		reg[dr] = reg[sr1] + sext(inst & 0x1F);
	}
	else {
		ushort sr2 = inst & 0x7;
		reg[dr] = reg[sr1] + reg[sr2];
	}

	setcc(dr);
}

void op_and() {

	ushort dr = (inst >> 9) & 0x7;
	ushort sr1 = (inst >> 6) & 0x7;

	if ((inst >> 5) & 0x1) {
		reg[dr] = reg[sr1] & sext(inst & 0x1F, 5);
	}
	else {
		ushort sr2 = inst & 0x7;
		reg[dr] = reg[sr1] & reg[sr2];
	}

	setcc(dr);
}

void op_not() {

	ushort dr = (inst >> 9) & 0x7;
	ushort sr = (inst >> 6) & 0x7;
	reg[dr] = ~reg[sr];
	setcc(dr);
}

void op_br() {

	ushort n = (inst >> 9) & 0x4;
	ushort z = (inst >> 9) & 0x2;
	ushort p = (inst >> 9) & 0x1;

	ushort N = reg[R_COND] & FL_NEG;
	ushort Z = reg[R_COND] & FL_ZRO;
	ushort P = reg[R_COND] & FL_POS;

	if ((n & reg[R_COND]) || (z & reg[R_COND]) || (p & reg[R_COND]))
		reg[R_PC] = reg[R_PC] + sext(inst & 0x1FF, 9);
}

void op_jmp() {

	ushort baseR = (inst >> 6) & 0x7;
	reg[R_PC] = reg[baseR];
}

void op_jsr() {

	reg[R_R7] = reg[R_PC];

	if ((inst >> 11) & 0x1) {
		reg[R_PC] = reg[R_PC] + sext(inst & 0x7FF, 11);
	}
	else {
		ushort baseR = (inst >> 6) & 0x7;
		reg[R_PC] = reg[baseR];
	}
}

void op_ld() {

	ushort dr = (inst >> 9) & 0x7;
	ushort pcOffset9 = (inst & 0x1FF);
	reg[dr] = mem_read(reg[R_PC] + sext(pcOffset9, 9));
	
	setcc(dr);
}

void op_ldi() {

	ushort dr = (inst >> 9) & 0x7;
	ushort pcOffset9 = (inst & 0x1FF);
	reg[dr] = mem_read(mem_read(reg[R_PC] + sext(pcOffset9, 9)));
	setcc(dr);  
}

void op_ldr() {

	ushort dr = (inst >> 9) & 0x7;
	ushort baseR = (inst >> 6) & 0x7;
	ushort offset6 = (inst & 0x3F);
	reg[dr] = mem_read(reg[baseR] + sext(offset6, 6));
	setcc(dr);
}

void op_lea() {

	ushort dr = (inst >> 9) & 0x7;
	ushort pcOffset = inst & 0x1FF;
	reg[dr] = reg[R_PC] + sext(pcOffset, 9);
	setcc(dr);
}

void op_st() {

	ushort pcOffset = inst & 0x1FF;
	
	ushort addr = reg[R_PC] + sext(pcOffset, 9);
	ushort val = (inst >> 9) & 0x7;
	
	mem_write(addr, reg[val]);
}

void op_sti() {

	ushort pcOffset = inst & 0x1FF;

	ushort addr = reg[R_PC] + sext(pcOffset, 9);
	ushort val = (inst >> 9) & 0x7;

	mem_write(mem_read(addr), reg[val]);
}

void op_str() {

	ushort baseR = (inst >> 6) & 0x7;
	ushort offset6 = inst & 0x3F;

	ushort addr = reg[baseR] + sext(offset6, 6);
	ushort val  = (inst >> 9) & 0x7;

	mem_write(addr, reg[val]);
}

void op_trap() {

	/*
	unsigned int trapvect8 = instr & 0xFF;

	reg[R_R7] = reg[R_PC];
	reg[R_PC] = memory[zext(trapvect8, 8)];
	*/

	reg[R_R7] = reg[R_PC];

	switch (inst & 0xFF) {

	case TRAP_GETC: {

		reg[R_R0] = (ushort)getchar();
		setcc(R_R0);

		break;
	}

	case TRAP_OUT: {

		putc((char)reg[R_R0], stdout);
		fflush(stdout);
		break;
	}

	case TRAP_PUTS: {

		ushort* c = memory + reg[R_R0];
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
		reg[R_R0] = (ushort)c;
		setcc(R_R0);
		break;
	}

	case TRAP_PUTSP: {
		
		ushort* c = memory + reg[R_R0];
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

void exec_instr(ushort cur_instr) {

	inst = cur_instr;

	ushort op = inst >> 12;

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

void vm_run(ushort* image)
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

		ushort cur_instr = mem_read(reg[R_PC]++);

		if (cur_instr == 0)
			break;

		exec_instr(cur_instr);

		log("\n");
	}

	restore_input_buffering();

	return 0;
}

bool vm_read_memory(ushort status_addr, ushort addr, ushort& value)
{
	if (memory[status_addr] == 1) {
		memory[status_addr] = 0;

		value = memory[addr];

		return true;
	}

	value = 0;
	return false;
}

void vm_write_memory(ushort status_addr, ushort addr, ushort value)
{
	memory[status_addr] = 1;

	memory[addr] = value;
}