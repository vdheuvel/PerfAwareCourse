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
	array<char, registerCount> registers;
	void ExecuteImmediateToReg(const Instruction &instruction);
	void ExecuteMove(const Instruction &instruction);
};

