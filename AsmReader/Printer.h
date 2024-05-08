#pragma once
#include <string>
#include <fstream>
#include <array>
#include "Instruction.h"
#include "Constants.h"

using std::string;
using std::ostream;
using std::array;

class Printer {
public:
	Printer(ostream& outfile);
	void print(const Instruction &instruction);
	void printMemory(const array<unsigned short, memorySize> memory);
private:
	string getOpString(unsigned char opcode, unsigned char opMid);
	string getSubOpString(unsigned char opcode);
	void writeDwInstruction(const Instruction &instruction);
	void writeAddSubCmpInstruction(const Instruction &instruction);
	void writeImmediateFromRegInstruction(const Instruction &instruction);
	void writeImmediateToRegInstruction(const Instruction &instruction);
	void writeJumpInstruction(int offset);
	ostream &outfile;
};