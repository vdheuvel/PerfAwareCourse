#pragma once
#include <string>
#include <fstream>
#include "Instruction.h"

using std::string;
using std::ostream;

class Printer {
public:
	Printer(ostream& outfile);
	void print(const Instruction &instruction);
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