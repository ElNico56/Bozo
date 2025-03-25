// bozorun.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MEMORY_SIZE 65536  // 16-bit memory space

typedef uint16_t Word;  // 16-bit integers

typedef enum {
	NOOP, HALT, MOV, OUT, INP, SHOW,
	ADD, SUB, MUL, DIV, AND, XOR,
	JEQ, JNE, JLT, JGT,
} Code;

typedef enum { I, M, P } Mode;

typedef struct { Code code; Mode mode1; Mode mode2; Mode mode3; } Opcode;

typedef struct { Word mem[MEMORY_SIZE]; int halted; } VM;

Opcode OPCODES[] = {
	#include "opcodes.txt"
};

// MOV Src Dst Len
// OUT Src Len
// INP Dst Len
// SHOW Src Len
// ADD Src Dst Len
// SUB Src Dst Len
// MUL Src Dst Len
// DIV Src Dst Len
// AND Src Dst Len
// XOR Src Dst Len
// JEQ OpA OpB Loc
// JNE OpA OpB Loc
// JLT OpA OpB Loc
// JGT OpA OpB Loc


inline Word Get(Mode mode, Word src, Word* mem, int offset) {
	switch (mode) {
	case I: return src;
	case M: return mem[src + offset];
	case P: return mem[mem[src] + offset];
	}
}


inline void Set(Mode mode, Word dest, Word value, Word* mem, int offset) {
	switch (mode) {
	case I: printf("Error: Cannot write to immediate values!\n"); break;
	case M: mem[dest + offset] = value; break;
	case P: mem[mem[dest] + offset] = value; break;
	}
}


inline void HandleJump(Mode mode, Word location, int condition, Word* mem) {
	if (condition) {
		switch (mode) {
		case I: mem[MEMORY_SIZE - 1] += 4 * location; break;
		case M: mem[MEMORY_SIZE - 1] = location; break;
		case P: mem[MEMORY_SIZE - 1] = mem[location]; break;
		}
	} else {
		mem[MEMORY_SIZE - 1] += 4;
	}
}


void Step(VM* vm) {
	Word* mem = vm->mem;
	Word pc = mem[MEMORY_SIZE - 1];

	Opcode opcode = OPCODES[mem[pc]];
	Word op1 = mem[pc + 1];
	Word op2 = mem[pc + 2];
	Word op3 = mem[pc + 3];

	Word a = Get(opcode.mode1, op1, mem, 0);
	Word b = Get(opcode.mode2, op2, mem, 0);
	Word c = Get(opcode.mode3, op3, mem, 0);

	switch (opcode.code) {
	case NOOP: mem[MEMORY_SIZE - 1] += 4; break;
	case HALT: vm->halted = 1; break;
	case MOV:
		for (int i = 0; i < c + (c == 0); i++) {
			Word value = Get(opcode.mode1, op1, mem, i);
			Set(opcode.mode2, op2, value, mem, i);
		}
		mem[MEMORY_SIZE - 1] += 4;
		break;
	case OUT:
		for (int i = 0; i < b + (b == 0); i++)
			printf("%c", Get(opcode.mode1, op1, mem, i));
		printf("\n");
		mem[MEMORY_SIZE - 1] += 4;
		break;
	case INP:
		printf("> ");
		for (int i = 0; i < b + (b == 0); i++) {
			Word value = getchar();
			Set(opcode.mode1, op1, value, mem, i);
		}
		mem[MEMORY_SIZE - 1] += 4;
		break;
	case SHOW:
		for (int i = 0; i < b + (b == 0); i++)
			printf("%5d ", Get(opcode.mode1, op1, mem, i));
		printf("\n");
		mem[MEMORY_SIZE - 1] += 4;
		break;
	case ADD:
		for (int i = 0; i < c + (c == 0); i++) {
			Word a = Get(opcode.mode1, op1, mem, i);
			Word b = Get(opcode.mode2, op2, mem, i);
			Set(opcode.mode2, op2, b + a, mem, i);
		}
		mem[MEMORY_SIZE - 1] += 4;
		break;
	case SUB:
		for (int i = 0; i < c + (c == 0); i++) {
			Word a = Get(opcode.mode1, op1, mem, i);
			Word b = Get(opcode.mode2, op2, mem, i);
			Set(opcode.mode2, op2, b - a, mem, i);
		}
		mem[MEMORY_SIZE - 1] += 4;
		break;
	case MUL:
		for (int i = 0; i < c + (c == 0); i++) {
			Word a = Get(opcode.mode1, op1, mem, i);
			Word b = Get(opcode.mode2, op2, mem, i);
			Set(opcode.mode2, op2, b * a, mem, i);
		}
		mem[MEMORY_SIZE - 1] += 4;
		break;
	case DIV:
		for (int i = 0; i < c + (c == 0); i++) {
			Word a = Get(opcode.mode1, op1, mem, i);
			Word b = Get(opcode.mode2, op2, mem, i);
			if (a != 0) Set(opcode.mode2, op2, b / a, mem, i);
		}
		mem[MEMORY_SIZE - 1] += 4;
		break;
	case AND:
		for (int i = 0; i < c + (c == 0); i++) {
			Word a = Get(opcode.mode1, op1, mem, i);
			Word b = Get(opcode.mode2, op2, mem, i);
			if (a != 0) Set(opcode.mode2, op2, b & a, mem, i);
		}
		mem[MEMORY_SIZE - 1] += 4;
		break;
	case XOR:
		for (int i = 0; i < c + (c == 0); i++) {
			Word a = Get(opcode.mode1, op1, mem, i);
			Word b = Get(opcode.mode2, op2, mem, i);
			if (a != 0) Set(opcode.mode2, op2, b ^ a, mem, i);
		}
		mem[MEMORY_SIZE - 1] += 4;
		break;
	case JEQ: HandleJump(opcode.mode3, op3, a == b, mem); break;
	case JNE: HandleJump(opcode.mode3, op3, a != b, mem); break;
	case JLT: HandleJump(opcode.mode3, op3, a < b, mem); break;
	case JGT: HandleJump(opcode.mode3, op3, a > b, mem); break;
	}
	if (pc == mem[MEMORY_SIZE - 1]) { vm->halted = 1; }
}


int LoadProgram(VM* vm, const char* filename) {
	FILE* file = fopen(filename, "rb");
	if (!file) {
		perror("Error opening file");
		return -1;
	}

	fread(vm->mem, sizeof(Word), MEMORY_SIZE, file);
	fclose(file);
	return 0;
}


void PrintMemory(VM* vm, int from, int to, int columns) {
	printf("Memory from %d to %d:\n", from, to);
	for (int i = from; i < to; i++) {
		if (i % columns == 0) printf("%04X: ", i);
		printf("%4X ", vm->mem[i]);
		if (i % columns == columns - 1) printf("\n");
	}
}


int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
		return 1;
	}

	VM vm = { 0 };
	if (LoadProgram(&vm, argv[1]) != 0) return 1;

	while (!vm.halted) Step(&vm);
	// PrintMemory(&vm, 0, 256, 4);

	return 0;
}
