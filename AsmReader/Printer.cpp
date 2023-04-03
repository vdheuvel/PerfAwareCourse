#include <iostream>
#include <bitset>
#include "Printer.h"
#include "Constants.h"

using std::cout;
using std::bitset;

Printer::Printer(ofstream& outfile) : outfile(outfile)
{
    outfile << "bits 16" << std::endl;
}

void Printer::print(const Instruction &instruction)
{
    outfile << std::endl;
    outfile << getOpString(instruction.opcode, instruction.opMid) << " ";

    switch (instruction.opcode)
    {
    case addSubCmpOp:
        writeAddSubCmpInstruction(instruction);
        break;
    case moveOp:
        writeDwInstruction(instruction);
        break;
    case immediateFromRegOp:
        writeImmediateFromRegInstruction(instruction);
        break;
    case immediateToRegOp:
        writeImmediateToRegInstruction(instruction);
        break;
    case jnzOp: // also jneOp
    case jeOp:
    case jlOp:
    case jleOp:
    case jbOp:
    case jbeOp:
    case jpOp:
    case joOp:
    case jsOp:
    case jnlOp:
    case jgOp:
    case jnbOp:
    case jaOp:
    case jnpOp:
    case jnoOp:
    case jnsOp:
    case loopOp:
    case loopzOp:
    case loopnzOp:
    case jcxzOp:
        writeJumpInstruction(instruction.jumpOffset);
    default:
        break;
    }
}

string Printer::getSubOpString(unsigned char opcode)
{
    switch (opcode)
    {
    case addOp:
        return "add";
    case subOp:
        return "sub";
    case cmpOp:
        return "cmp";
    default:
        return "error: unknown op encountered ";
    }
}

void Printer::writeAddSubCmpInstruction(const Instruction& instruction)
{
    if (instruction.accumulator) {
        if (instruction.w) {
            outfile << "ax, " << instruction.immediate;
        }
        else {
            outfile << "al, " << instruction.immediate;
        }
    }
    else {
        writeDwInstruction(instruction);
    }
}

void Printer::writeImmediateFromRegInstruction(const Instruction& instruction)
{
    if (instruction.mod != 3) { // memory mode
        outfile << (instruction.w ? "word" : "byte") << " [";
    }

    bool twoByteData = instruction.w && !instruction.s;
    int data = instruction.immediate;
    string addresscalcstr;
    if (instruction.mod == 3) {
        if (instruction.w) {
            addresscalcstr = w1mod11Registers[instruction.rm];
        }
        else {
            addresscalcstr = w0mod11Registers[instruction.rm];
        }
    }
    else {
        addresscalcstr = addressCalc[instruction.rm];
    }
    //string addresscalcstr = addressCalc[rm];

    outfile << addresscalcstr;

    if (instruction.mod == 1 || instruction.mod == 2) { //displacement
        outfile << (instruction.displacement >= 0 ? " + " : " - ") << instruction.displacement;
    }

    if (instruction.mod != 3) { // memory mode
        outfile << "]";
    }

    outfile << ", " << data;
}

void Printer::writeImmediateToRegInstruction(const Instruction& instruction)
{
    if (instruction.w) {
        outfile << w1mod11Registers[instruction.reg] << ", " << (unsigned short)instruction.immediate;
    }
    else {
        outfile << w0mod11Registers[instruction.reg] << ", " << (char)instruction.immediate;
    }
}

void Printer::writeJumpInstruction(int offset)
{
    if (offset + 2 > 0) {
        outfile << " $" << (signed short)offset + 2;
    }
    else if (offset + 2 == 0) {
        outfile << " $+0";
    }
    else {
        outfile << " $" << (signed short)offset + 2;
    }
}

string Printer::getOpString(unsigned char opcode, unsigned char opMid)
{
    switch (opcode)
    {
    case addSubCmpOp:
    {
        return getSubOpString(opMid);
    }
    case immediateToRegOp:
        return "mov";
    case moveOp:
        return "mov";
    case immediateFromRegOp: {
        return getSubOpString(opMid);
    }
    case jnzOp:
        return "jnz";
    case jeOp:
        return "je";
    case jlOp:
        return "jl";
    case jleOp:
        return "jle";
    case jbOp:
        return "jb";
    case jbeOp:
        return "jbe";
    case jpOp:
        return "jp";
    case joOp:
        return "jo";
    case jsOp:
        return "js";
    case jnlOp:
        return "jnl";
    case jgOp:
        return "jg";
    case jnbOp:
        return "jnb";
    case jaOp:
        return "ja";
    case jnpOp:
        return "jnp";
    case jnoOp:
        return "jno";
    case jnsOp:
        return "jns";
    case loopOp:
        return "loop";
    case loopzOp:
        return "loopz";
    case loopnzOp:
        return "loopnz";
    case jcxzOp:
        return "jcxz";
    default:
        cout << "opcode not implemented " << bitset<8>(opcode).to_string() << std::endl;
        return "error: unknown op encountered ";
    }
}

void Printer::writeDwInstruction(const Instruction& instruction){
    if (instruction.mod == 0b11) {
        if (instruction.w) {
            outfile << w1mod11Registers[instruction.d ? instruction.reg : instruction.rm] 
                << ", " 
                << w1mod11Registers[instruction.d ? instruction.rm : instruction.reg];
        }
        else {
            outfile << w0mod11Registers[instruction.d ? instruction.reg : instruction.rm] 
                << ", " 
                << w0mod11Registers[instruction.d ? instruction.rm : instruction.reg];
        }
    }
    else {
        string registerstr;
        if (instruction.w) {
            registerstr = w1mod11Registers[instruction.reg];
        }
        else {
            registerstr = w0mod11Registers[instruction.reg];
        }
        string addresscalcstr = addressCalc[instruction.rm];
        if (instruction.d) {
            outfile << registerstr << ", [" << addresscalcstr;
            if (instruction.displacement != 0) {
                outfile << " + " << instruction.displacement;
            }
            outfile << "]";
        }
        else {
            outfile << "[" << addresscalcstr;
            if (instruction.displacement != 0) {
                outfile << " + " << instruction.displacement;

            }
            outfile << "], " << registerstr;
        }
    }
}