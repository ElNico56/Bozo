// bozorun.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 256

typedef unsigned char Byte;

typedef enum {
	MOV = 0x0, // MOV Src Dest Len
	ADD = 0x1, // ADD opA opB Dest
	SUB = 0x2, // SUB opA opB Dest
	JEQ = 0x3, // JEQ opA opB Loc
	JGT = 0x4, // JGT opA opB Loc
	JLT = 0x5, // JLT opA opB Loc
	JNE = 0x6, // JNE opA opB Loc
	AND = 0x7, // AND opA opB Dest
} Code;

typedef enum {
	I = 0x0,
	M = 0x1,
	P = 0x2,
} Mode;

typedef struct {
	Code code;
	Mode mode1;
	Mode mode2;
	Mode mode3;
} Opcode;

typedef struct {
	Byte mem[MEMORY_SIZE];
	int halted;
} VM;

Opcode GetOpcode(Byte byte) {
	Opcode opcode = { 0 };
	opcode.code = (byte >> 5) & 0x7;
	opcode.mode3 = ((byte & 0x1F) / 1) % 3;
	opcode.mode2 = ((byte & 0x1F) / 3) % 3;
	opcode.mode1 = ((byte & 0x1F) / 9) % 3;
	return opcode;
}

Byte GetOprand(Mode mode, Byte src, Byte* mem) {
	switch (mode) {
	case I: return src & 0xFF;
	case M: return mem[src] & 0xFF;
	case P: return mem[mem[src]] & 0xFF;
	}
}

void SetOprand(Mode mode, Byte dest, Byte value, Byte* mem) {
	switch (mode) {
	case I: break;
	case M: mem[dest] = value & 0xFF; break;
	case P: mem[mem[dest]] = value & 0xFF; break;
	}
}

void HandleJump(Mode mode, Byte location, int condition, Byte* mem) {
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
	Opcode opcode = GetOpcode(mem[pc]);
	Byte op1 = mem[pc + 1];
	Byte op2 = mem[pc + 2];
	Byte op3 = mem[pc + 3];

	Byte a = GetOprand(opcode.mode1, op1, mem);
	Byte b = GetOprand(opcode.mode2, op2, mem);
	Byte c = GetOprand(opcode.mode3, op3, mem);

	switch (opcode.code) {
	case MOV: {
		Byte length = c == 0 ? 1 : c;
		if (opcode.mode1 == I) {
			for (int i = 0; i < length; i++) {
				SetOprand(opcode.mode2, op2 + i, op1, mem);
			}
		} else {
			for (int i = 0; i < length; i++) {
				Byte value = GetOprand(opcode.mode1, op1 + i, mem);
				SetOprand(opcode.mode2, op2 + i, value, mem);
			}
		}
		if (opcode.mode2 == I) {
			// usually useless, destination can't be immediate
			// these "invalid" opcodes will be used for something else
			switch (op2) {
			case 0x0: { // OUT
				printf("%d\n", a);
				break;
			}
			case 0x1: { // IN
				scanf("%2X", &a);
				SetOprand(opcode.mode1, op1, a, mem);
				break;
			}
			case 0x2: { // SHOW
				PrintMemory(vm, a);
				break;
			}
			default:
				printf("Invalid opcode\n");
				break;
			}

		}
		mem[255] += 4;
		break;
	}
	case ADD: {
		SetOprand(opcode.mode3, op3, a + b, mem);
		mem[255] += 4;
		break;
	}
	case SUB: {
		SetOprand(opcode.mode3, op3, a - b, mem);
		mem[255] += 4;
		break;
	}
	case JEQ: {
		HandleJump(opcode.mode3, op3, a == b, mem);
		break;
	}
	case JGT: {
		HandleJump(opcode.mode3, op3, a > b, mem);
		break;
	}
	case JLT: {
		HandleJump(opcode.mode3, op3, a < b, mem);
		break;
	}
	case JNE: {
		HandleJump(opcode.mode3, op3, a != b, mem);
		break;
	}
	case AND: {
		SetOprand(opcode.mode3, op3, a & b, mem);
		mem[255] += 4;
		break;
	}
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
