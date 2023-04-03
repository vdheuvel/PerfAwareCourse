#pragma once
#include<array>
#include"Instruction.h"
#include"Constants.h"

using std::array;

class Simulator
{
public:
	Simulator();
	void Execute(const Instruction &instruction);
	void PrintRegisters() const;
private:
	const static int registerCount = 8;
	array<unsigned short, registerCount> registers;
	bool zeroFlag = false;
	bool signFlag = false;
	void ExecuteImmediateToReg(const Instruction &instruction);
	void ExecuteMove(const Instruction &instruction);
	void ExecuteAddSubComp(const Instruction& instruction);
	void ExecuteImmediateFromReg(const Instruction& instruction);
	void SetFlags(unsigned short result);
};

