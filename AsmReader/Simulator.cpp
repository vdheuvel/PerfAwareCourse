#include <iostream>
#include <algorithm>
#include <bitset>
#include "Simulator.h"

using std::cout;
using std::bitset;

Simulator::Simulator()
{
	for (auto &r: registers) {
		r = 0;
	}
}

void Simulator::Execute(const Instruction &instruction)
{
	auto before = registers;

	switch (instruction.opcode)
	{
	case immediateToRegOp:
		ExecuteImmediateToReg(instruction);
		break;
	case moveOp:
		ExecuteMove(instruction);
	default:
		break;
	}

	for (int i = 0; i < registerCount; i++) {
		if (registers[i] != before[i]) {
			cout << "changed register " << w1mod11Registers[i] << " from " << (unsigned int) before[i] << " to " << (unsigned int) registers[i] << std::endl;
		}
	}
}

void Simulator::ExecuteImmediateToReg(const Instruction &instruction)
{
	if (instruction.w) {
		registers[instruction.reg] = instruction.immediate;
	}
	else {
		// not implemented
	}
}

void Simulator::ExecuteMove(const Instruction& instruction)
{
	if (instruction.w) {
		if (instruction.d) {
			registers[instruction.reg] = registers[instruction.rm];
		}
		else {
			registers[instruction.rm] = registers[instruction.reg];
		}
	}
	else {
		// not implemented
	}
}

void Simulator::PrintRegisters() const
{
	for (int i = 0; i < registerCount; i++) {
		cout << i << " " << w1mod11Registers[i] << " : "
			<< (unsigned int)registers[i] << "   (" << bitset<8>(registers[i]).to_string() << ")" << std::endl;
	}
}
