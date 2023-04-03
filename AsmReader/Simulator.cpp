#include <iostream>
#include <algorithm>
#include <bitset>
#include <iomanip>
#include "Simulator.h"

using std::cout;
using std::bitset;

Simulator::Simulator(vector<Instruction> instructions) : instructions(instructions)
{
	for (auto &r: registers) {
		r = 0;
	}
	for (int i = 0; i < instructions.size(); ++i) {
		instructionIdByAddress.insert({ instructions[i].address, i });
	}
}

int Simulator::Execute()
{
	auto before = registers;
	auto beforeZeroFlag = zeroFlag;
	auto beforeSignFlag = signFlag;
	auto beforeIp = instructionPointer;

	int instructionIndex = instructionIdByAddress[instructionPointer];
	const Instruction& instruction = instructions[instructionIndex];

	instructionPointer += instruction.size;

	switch (instruction.opcode)
	{
	case immediateToRegOp:
		ExecuteImmediateToReg(instruction);
		break;
	case immediateFromRegOp:
		ExecuteImmediateFromReg(instruction);
		break;
	case addSubCmpOp:
		ExecuteAddSubComp(instruction);
		break;
	case moveOp:
		ExecuteMove(instruction);
	case jnzOp:
		ExecuteJnz(instruction);
	default:
		break;
	}

	cout << std::hex;
	for (int i = 0; i < registerCount; i++) {
		if (registers[i] != before[i]) {
			cout << "changed register " << w1mod11Registers[i] << " from " << (unsigned int) before[i] << " to " << (unsigned int) registers[i] << std::endl;
		}
	}
	cout << std::dec;
	if (zeroFlag != beforeZeroFlag) {
		cout << "changed zero flag from " << beforeZeroFlag << " to " << zeroFlag << std::endl;
	}
	if (signFlag != beforeSignFlag) {
		cout << "changed sign flag from " << beforeSignFlag << " to " << signFlag << std::endl;
	}
	if (instructionPointer != beforeIp) {
		cout << "changed instruction pointer " << beforeIp << " to " << instructionPointer << std::endl;
	}

	return instructionIndex;
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

void Simulator::ExecuteAddSubComp(const Instruction& instruction)
{
	auto dest = instruction.d ? instruction.reg : instruction.rm;
	auto source = instruction.d ? instruction.rm : instruction.reg;
	switch (instruction.opMid) {
	case addOp:
		registers[dest] += registers[source];
		SetFlags(registers[dest]);
		break;
	case subOp:
		registers[dest] -= registers[source];
		SetFlags(registers[dest]);
		break;
	case cmpOp:
	{
		auto temp = registers[dest] - registers[source];
		SetFlags(temp);
		break;
	}
	}
}

void Simulator::ExecuteImmediateFromReg(const Instruction& instruction)
{
	switch (instruction.opMid) {
	case addOp:
		registers[instruction.rm] += instruction.immediate;
		SetFlags(registers[instruction.rm]);
		break;
	case subOp:
		registers[instruction.rm] -= instruction.immediate;
		SetFlags(registers[instruction.rm]);
		break;
	case cmpOp:
	{
		auto temp = registers[instruction.rm] - instruction.immediate;
		SetFlags(temp);
		break;
	}
	}
}

void Simulator::ExecuteJnz(const Instruction& instruction)
{
	if (!zeroFlag) {
		instructionPointer += instruction.jumpOffset;
	}
}

void Simulator::SetFlags(unsigned short result)
{
	zeroFlag = result == 0;
	signFlag = result & 0b1000000000000000;
}

void Simulator::PrintRegisters() const
{
	for (int i = 0; i < registerCount; i++) {
		cout << i << " " << w1mod11Registers[i] << " : "
			<< (unsigned int)registers[i] << "   (" << bitset<16>(registers[i]).to_string() << ")" << std::endl;
	}
}
