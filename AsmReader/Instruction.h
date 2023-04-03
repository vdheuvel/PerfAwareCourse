#pragma once

enum OperandType {
	operand_none,
	operand_register,
	operand_memory,
	operand_immediate
};

struct Operand {
	OperandType type;
	
};

struct Instruction {
	int opcode;
	int opMid; // for add, sub, cmp
	int size;
	int address;
	bool s;
	bool w;
	unsigned char mod;
	unsigned char rm;
	unsigned char reg;
	int displacement;
	bool accumulator;
	bool d;
	signed char jumpOffset;
	int immediate;
};