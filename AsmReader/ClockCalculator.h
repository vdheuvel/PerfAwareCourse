#pragma once
#include "Instruction.h"

class ClockCalculator
{
public:
	int Calculate(const Instruction &instruction) const;
private:
	int EffectiveAddressCost(const Instruction& instruction) const;
};

