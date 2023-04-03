#pragma once
#include<vector>
#include<string>
#include"Instruction.h"

using std::string;

class AsmReader {
public:
	std::vector<Instruction> read(string filename);
private:
	unsigned char getOperation(unsigned char byte);
};