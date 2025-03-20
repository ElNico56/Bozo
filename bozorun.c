// bozorun.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 256

typedef unsigned char Byte;

typedef enum {
	NOOP, HALT, MOV, OUT, INP, SHOW,
	ADD, SUB, MUL, DIV, AND, XOR,
	JEQ, JNE, JLT, JGT,
} Code;

typedef enum { I, M, P } Mode;

typedef struct { Code code; Mode mode1; Mode mode2; Mode mode3; } Opcode;

typedef struct { Byte mem[MEMORY_SIZE]; int halted; } VM;

Opcode OPCODES[] = {
	{ NOOP, I, I, I },{ HALT, I, I, I },
	{ MOV, I, M, I },{ MOV, M, M, I },{ MOV, P, M, I },
	{ MOV, I, P, I },{ MOV, M, P, I },{ MOV, P, P, I },
	{ MOV, I, M, M },{ MOV, M, M, M },{ MOV, P, M, M },
	{ MOV, I, P, M },{ MOV, M, P, M },{ MOV, P, P, M },
	{ MOV, I, M, P },{ MOV, M, M, P },{ MOV, P, M, P },
	{ MOV, I, P, P },{ MOV, M, P, P },{ MOV, P, P, P },

	{ OUT, I, I, I },{ OUT, M, I, I },{ OUT, P, I, I },
	{ OUT, I, M, I },{ OUT, M, M, I },{ OUT, P, M, I },
	{ OUT, I, P, I },{ OUT, M, P, I },{ OUT, P, P, I },

	/*------------*/ { INP, M, I, I },{ INP, P, I, I },
	/*--ElNico56--*/ { INP, M, M, I },{ INP, P, M, I },
	/*------------*/ { INP, M, P, I },{ INP, P, P, I },

	{ SHOW, I, I, I },{ SHOW, M, I, I },{ SHOW, P, I, I },
	{ SHOW, I, M, I },{ SHOW, M, M, I },{ SHOW, P, M, I },
	{ SHOW, I, P, I },{ SHOW, M, P, I },{ SHOW, P, P, I },

	{ ADD, I, M, I },{ ADD, M, M, I },{ ADD, P, M, I },
	{ ADD, I, P, I },{ ADD, M, P, I },{ ADD, P, P, I },
	{ ADD, I, M, M },{ ADD, M, M, M },{ ADD, P, M, M },
	{ ADD, I, P, M },{ ADD, M, P, M },{ ADD, P, P, M },
	{ ADD, I, M, P },{ ADD, M, M, P },{ ADD, P, M, P },
	{ ADD, I, P, P },{ ADD, M, P, P },{ ADD, P, P, P },

	{ SUB, I, M, I },{ SUB, M, M, I },{ SUB, P, M, I },
	{ SUB, I, P, I },{ SUB, M, P, I },{ SUB, P, P, I },
	{ SUB, I, M, M },{ SUB, M, M, M },{ SUB, P, M, M },
	{ SUB, I, P, M },{ SUB, M, P, M },{ SUB, P, P, M },
	{ SUB, I, M, P },{ SUB, M, M, P },{ SUB, P, M, P },
	{ SUB, I, P, P },{ SUB, M, P, P },{ SUB, P, P, P },

	{ MUL, I, M, I },{ MUL, M, M, I },{ MUL, P, M, I },
	{ MUL, I, P, I },{ MUL, M, P, I },{ MUL, P, P, I },
	{ MUL, I, M, M },{ MUL, M, M, M },{ MUL, P, M, M },
	{ MUL, I, P, M },{ MUL, M, P, M },{ MUL, P, P, M },
	{ MUL, I, M, P },{ MUL, M, M, P },{ MUL, P, M, P },
	{ MUL, I, P, P },{ MUL, M, P, P },{ MUL, P, P, P },

	{ DIV, I, M, I },{ DIV, M, M, I },{ DIV, P, M, I },
	{ DIV, I, P, I },{ DIV, M, P, I },{ DIV, P, P, I },
	{ DIV, I, M, M },{ DIV, M, M, M },{ DIV, P, M, M },
	{ DIV, I, P, M },{ DIV, M, P, M },{ DIV, P, P, M },
	{ DIV, I, M, P },{ DIV, M, M, P },{ DIV, P, M, P },
	{ DIV, I, P, P },{ DIV, M, P, P },{ DIV, P, P, P },

	{ AND, I, M, I },{ AND, M, M, I },{ AND, P, M, I },
	{ AND, I, P, I },{ AND, M, P, I },{ AND, P, P, I },
	{ AND, I, M, M },{ AND, M, M, M },{ AND, P, M, M },
	{ AND, I, P, M },{ AND, M, P, M },{ AND, P, P, M },
	{ AND, I, M, P },{ AND, M, M, P },{ AND, P, M, P },
	{ AND, I, P, P },{ AND, M, P, P },{ AND, P, P, P },

	{ XOR, I, M, I },{ XOR, M, M, I },{ XOR, P, M, I },
	{ XOR, I, P, I },{ XOR, M, P, I },{ XOR, P, P, I },
	{ XOR, I, M, M },{ XOR, M, M, M },{ XOR, P, M, M },
	{ XOR, I, P, M },{ XOR, M, P, M },{ XOR, P, P, M },
	{ XOR, I, M, P },{ XOR, M, M, P },{ XOR, P, M, P },
	{ XOR, I, P, P },{ XOR, M, P, P },{ XOR, P, P, P },

	/*------------*/ { JEQ, M, I, I },{ JEQ, P, I, I },
	{ JEQ, I, M, I },{ JEQ, M, M, I },{ JEQ, P, M, I },
	{ JEQ, I, P, I },{ JEQ, M, P, I },{ JEQ, P, P, I },
	/*------------*/ { JEQ, M, I, M },{ JEQ, P, I, M },
	{ JEQ, I, M, M },{ JEQ, M, M, M },{ JEQ, P, M, M },
	{ JEQ, I, P, M },{ JEQ, M, P, M },{ JEQ, P, P, M },
	/*------------*/ { JEQ, M, I, P },{ JEQ, P, I, P },
	{ JEQ, I, M, P },{ JEQ, M, M, P },{ JEQ, P, M, P },
	{ JEQ, I, P, P },{ JEQ, M, P, P },{ JEQ, P, P, P },

	/*------------*/ { JNE, M, I, I },{ JNE, P, I, I },
	{ JNE, I, M, I },{ JNE, M, M, I },{ JNE, P, M, I },
	{ JNE, I, P, I },{ JNE, M, P, I },{ JNE, P, P, I },
	/*------------*/ { JNE, M, I, M },{ JNE, P, I, M },
	{ JNE, I, M, M },{ JNE, M, M, M },{ JNE, P, M, M },
	{ JNE, I, P, M },{ JNE, M, P, M },{ JNE, P, P, M },
	/*------------*/ { JNE, M, I, P },{ JNE, P, I, P },
	{ JNE, I, M, P },{ JNE, M, M, P },{ JNE, P, M, P },
	{ JNE, I, P, P },{ JNE, M, P, P },{ JNE, P, P, P },

	/*------------*/ { JLT, M, I, I },{ JLT, P, I, I },
	{ JLT, I, M, I },{ JLT, M, M, I },{ JLT, P, M, I },
	{ JLT, I, P, I },{ JLT, M, P, I },{ JLT, P, P, I },
	/*------------*/ { JLT, M, I, M },{ JLT, P, I, M },
	{ JLT, I, M, M },{ JLT, M, M, M },{ JLT, P, M, M },
	{ JLT, I, P, M },{ JLT, M, P, M },{ JLT, P, P, M },
	/*------------*/ { JLT, M, I, P },{ JLT, P, I, P },
	{ JLT, I, M, P },{ JLT, M, M, P },{ JLT, P, M, P },
	{ JLT, I, P, P },{ JLT, M, P, P },{ JLT, P, P, P },

	/*------------*/ { JGT, M, I, I },{ JGT, P, I, I },
	{ JGT, I, M, I },{ JGT, M, M, I },{ JGT, P, M, I },
	{ JGT, I, P, I },{ JGT, M, P, I },{ JGT, P, P, I },
	/*------------*/ { JGT, M, I, M },{ JGT, P, I, M },
	{ JGT, I, M, M },{ JGT, M, M, M },{ JGT, P, M, M },
	{ JGT, I, P, M },{ JGT, M, P, M },{ JGT, P, P, M },
	/*------------*/ { JGT, M, I, P },{ JGT, P, I, P },
	{ JGT, I, M, P },{ JGT, M, M, P },{ JGT, P, M, P },
	{ JGT, I, P, P },{ JGT, M, P, P },{ JGT, P, P, P },
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

inline Byte Get(Mode mode, Byte src, Byte* mem, int offset) {
	switch (mode) {
	case I: return src & 0xFF;
	case M: return mem[src + offset] & 0xFF;
	case P: return mem[mem[src] + offset] & 0xFF;
	}
}

inline void Set(Mode mode, Byte dest, Byte value, Byte* mem, int offset) {
	switch (mode) {
	case I: printf("Uh-oh, stinky!\n"); break;
	case M: mem[dest + offset] = value & 0xFF; break;
	case P: mem[mem[dest] + offset] = value & 0xFF; break;
	}
}

inline void HandleJump(Mode mode, Byte location, int condition, Byte* mem) {
	if (condition) {
		switch (mode) {
		case I: mem[255] += 4 * location; break;
		case M: mem[255] = location; break;
		case P: mem[255] = mem[location]; break;
		}
		mem[255] &= 0xFF;
	} else {
		mem[255] += 4;
	}
}

void PrintMemory(VM* vm, int columns) {
	printf("\n");
	for (int y = 0; y < 256 / columns; y++) {
		printf("%02X: ", y * columns);
		for (int x = 0; x < columns; x++) {
			printf("%2X ", vm->mem[y * columns + x]);
		}
		printf("\n");
	}
	printf("\n");
}

void Step(VM* vm) {
	Byte* mem = vm->mem;
	Byte pc = mem[255];

	Opcode opcode = OPCODES[mem[pc]];
	Byte op1 = mem[pc + 1];
	Byte op2 = mem[pc + 2];
	Byte op3 = mem[pc + 3];

	Byte a = Get(opcode.mode1, op1, mem, 0);
	Byte b = Get(opcode.mode2, op2, mem, 0);
	Byte c = Get(opcode.mode3, op3, mem, 0);

	switch (opcode.code) {
	case NOOP: mem[255] += 4; break;
	case HALT: break;
	case MOV:
		for (int i = 0; i < (c == 0 ? 1 : c); i++) {
			Byte value = Get(opcode.mode1, op1, mem, i);
			Set(opcode.mode2, op2, value, mem, i);
		}
		mem[255] += 4; break;
	case OUT:
		for (int i = 0; i < (b == 0 ? 1 : b); i++)
			printf("%c", Get(opcode.mode1, op1, mem, i));
		printf("\n");
		mem[255] += 4; break;
	case INP:
		printf("> ");
		for (int i = 0; i < (b == 0 ? 1 : b); i++) {
			Byte value = getchar();
			Set(opcode.mode1, op1, value, mem, i);
		}
		mem[255] += 4; break;
	case SHOW:
		for (int i = 0; i < (b == 0 ? 1 : b); i++)
			printf("%3d ", Get(opcode.mode1, op1, mem, i));
		printf("\n");
		mem[255] += 4; break;
	case ADD:
		for (int i = 0; i < (c == 0 ? 1 : c); i++) {
			Byte a = Get(opcode.mode1, op1, mem, i);
			Byte b = Get(opcode.mode2, op2, mem, i);
			Set(opcode.mode2, op2, b + a, mem, i);
		}
		mem[255] += 4; break;
	case SUB:
		for (int i = 0; i < (c == 0 ? 1 : c); i++) {
			Byte a = Get(opcode.mode1, op1, mem, i);
			Byte b = Get(opcode.mode2, op2, mem, i);
			Set(opcode.mode2, op2, b - a, mem, i);
		}
		mem[255] += 4; break;
	case MUL:
		for (int i = 0; i < (c == 0 ? 1 : c); i++) {
			Byte a = Get(opcode.mode1, op1, mem, i);
			Byte b = Get(opcode.mode2, op2, mem, i);
			Set(opcode.mode2, op2, b * a, mem, i);
		}
		mem[255] += 4; break;
	case DIV:
		for (int i = 0; i < (c == 0 ? 1 : c); i++) {
			Byte a = Get(opcode.mode1, op1, mem, i);
			Byte b = Get(opcode.mode2, op2, mem, i);
			Set(opcode.mode2, op2, b / a, mem, i);
		}
		mem[255] += 4; break;
	case AND:
		for (int i = 0; i < (c == 0 ? 1 : c); i++) {
			Byte a = Get(opcode.mode1, op1, mem, i);
			Byte b = Get(opcode.mode2, op2, mem, i);
			Set(opcode.mode2, op2, b & a, mem, i);
		}
		mem[255] += 4; break;
	case XOR:
		for (int i = 0; i < (c == 0 ? 1 : c); i++) {
			Byte a = Get(opcode.mode1, op1, mem, i);
			Byte b = Get(opcode.mode2, op2, mem, i);
			Set(opcode.mode2, op2, b ^ a, mem, i);
		}
		mem[255] += 4; break;
	case JEQ: HandleJump(opcode.mode3, op3, a == b, mem); break;
	case JNE: HandleJump(opcode.mode3, op3, a != b, mem); break;
	case JLT: HandleJump(opcode.mode3, op3, a < b, mem); break;
	case JGT: HandleJump(opcode.mode3, op3, a > b, mem); break;
	}

	if (pc == mem[255]) vm->halted = 1;
}

int LoadProgram(VM* vm, const char* filename) {
	FILE* file = fopen(filename, "rb");
	if (!file) {
		perror("Error opening file");
		return -1;
	}

	Byte buffer[MEMORY_SIZE];
	size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
	fclose(file);

	if (bytesRead != sizeof(buffer)) {
		fprintf(stderr, "Error reading file\n");
		return -1;
	}

	memcpy(&vm->mem, buffer, MEMORY_SIZE);

	return 0;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
		return 1;
	}

	VM vm = { 0 };
	if (LoadProgram(&vm, argv[1]) != 0) {
		return 1;
	}

	while (!vm.halted) {
		Step(&vm);
	}
	// PrintMemory(&vm, 4);
	return 0;
}
