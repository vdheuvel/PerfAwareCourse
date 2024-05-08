#include "ClockCalculator.h"
#include "Constants.h"

int ClockCalculator::Calculate(const Instruction &instruction) const
{
	switch (instruction.opcode) {
		case immediateFromRegOp:
		case immediateToRegOp:
			return 4;
		case moveOp:
			if (instruction.mod == 3) { // reg - reg
				return 2;
			}
			else{
				if (instruction.d) { // from mem to reg
					return 8 + EffectiveAddressCost(instruction);
				}
				else { // from reg to mem
					return 9 + EffectiveAddressCost(instruction);
				}

			}
		case addOp:
			if (instruction.mod == 3) { // reg - reg
				return 3;
			}
			else{
				if (instruction.d) { // from mem to reg
					return 9 + EffectiveAddressCost(instruction);
				}
				else { // from reg to mem
					return 16 + EffectiveAddressCost(instruction);
				}

			}
	}
	return -1;
}

int ClockCalculator::EffectiveAddressCost(const Instruction& instruction) const
{
	if (instruction.mod == 0 && instruction.rm == 6) {
		return 6; // special case direct address
	}
	bool hasDisplacement = instruction.mod > 0 && instruction.displacement > 0;
	bool hasBaseOrIndex = instruction.rm >= 4;
	bool hasBaseAndIndex = instruction.rm < 4;

	if (!hasDisplacement && hasBaseOrIndex) {
		return 5;
	}
	if (!hasDisplacement && hasBaseAndIndex) {
		return 7; // or 8 depending on regs
	}
	if (hasDisplacement && hasBaseOrIndex) {
		return 9;
	}
	if (hasDisplacement && hasBaseAndIndex) {
		return 11; // or 12 depending on regs
	}
	return 0;
}
