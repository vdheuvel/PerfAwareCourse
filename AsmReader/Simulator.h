#pragma once
#include<array>
#include<vector>
#include<unordered_map>
#include"Instruction.h"
#include"Constants.h"

using std::array;
using std::vector;
using std::unordered_map;

class Simulator
{
public:
	Simulator(vector<Instruction> instructions);
	int Execute();
	void PrintRegisters() const;
	int instructionPointer = 0;
private:
	const static int registerCount = 8;
	const static unsigned int memorySize = 1 << 16;
	array<unsigned short, registerCount> registers;
	array<unsigned short, memorySize> memory;
	bool zeroFlag = false;
	bool signFlag = false;
	void ExecuteImmediateToReg(const Instruction &instruction);
	void ExecuteImmediateToRegMem(const Instruction &instruction);
	void ExecuteMove(const Instruction &instruction);
	void ExecuteAddSubComp(const Instruction& instruction);
	void ExecuteImmediateFromReg(const Instruction& instruction);
	void ExecuteJnz(const Instruction &instruction);
	void SetFlags(unsigned short result);
	const vector<Instruction> instructions;
	unordered_map<int, int> instructionIdByAddress;
	int getAddressCalc(unsigned short rm);
};

