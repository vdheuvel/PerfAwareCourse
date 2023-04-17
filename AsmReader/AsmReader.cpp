#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <bitset>
#include "AsmReader.h"
#include "Constants.h"
using std::cout;
using std::string;
using std::ofstream;
using std::bitset;


int getDisplacement(unsigned char* buffer, bool wide, int mod, int rm, int &i) {
    short displacement = 0;
    switch (mod) {
    case 0b00: // mem mode, no displacement
        if (rm == 0b110) {
            displacement = (unsigned short)(buffer[i + 3]) * 256 + buffer[i + 2];
            i += 4;
        }
        else {
            i += 2;
        }
        break;
    case 0b01: // mem mode, 8-bit displacement follows
        displacement = buffer[i + 2];
        i += 3;
        break;
    case 0b10: // mem mode, 16-bit displacement follows
        displacement = (unsigned short)(buffer[i + 3]) * 256 + buffer[i + 2];
        i += 4;
        break;
    case 0b11: // reg mode, no displacement
        displacement = 0;
        i += 2;
        break;
    }
    return displacement;
}

int getData(unsigned char* buffer, bool wide, int& i) {
    if (wide) {
        return (unsigned short)(buffer[i + 1]) * 256 + buffer[i];
    }
    else {
        return buffer[i];
    }
}

string getOp(int op) {
    switch (op)
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

int parseDwInstruction(unsigned char *buffer, int &i, Instruction &instruction){
    instruction.d = (buffer[i] & 0b00000010) >> 1;
    instruction.w = buffer[i] & 0b00000001;
    instruction.mod = buffer[i + 1] >> 6;
    instruction.reg = (buffer[i + 1] >> 3) & 0b00000111;
    instruction.rm = buffer[i + 1] & 0b00000111;
    instruction.displacement = getDisplacement(buffer, instruction.w, instruction.mod, instruction.rm, i);

    return i;
}

std::vector<Instruction> AsmReader::read(string filename)
{
    FILE* fileptr;
    unsigned char* buffer;
    long filelen;

    fileptr = fopen(filename.c_str(), "rb");  // Open the file in binary mode
    if (!fileptr) {
        std::cerr << "could not open file at " << filename;
        exit(-1);
    }
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file

    buffer = (unsigned char*)malloc(filelen * sizeof(unsigned char)); // Enough memory for the file
    fread(buffer, filelen, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file

    int i = 0;
    std::vector<Instruction> instructions;

    while (i < filelen) {
        Instruction instruction;
        instruction.address = i;
        instruction.opcode = getOperation(buffer[i]);
        switch (instruction.opcode)
        {
        case addSubCmpOp:
        {
            instruction.opMid = buffer[i] >> 3 & 0b00000111;
            instruction.d = (buffer[i] & 0b00000010) >> 1;
            instruction.w = buffer[i] & 0b00000001;
            instruction.accumulator = buffer[i] >> 2 & 0b00000001;
            if (instruction.accumulator) {
                if (instruction.w) {
                    instruction.immediate = (unsigned short)(buffer[i + 2]) * 256 + buffer[i + 1];
                    //outfile << " ax, " << (unsigned short)(buffer[i + 2]) * 256 + buffer[i + 1];
                    i += 3;
                }
                else {
                    instruction.immediate = (unsigned short)buffer[i + 1];
                    //outfile << " al, " << (unsigned short)buffer[i + 1];
                    i += 2;
                }
            }
            else {
                parseDwInstruction(buffer, i, instruction);
            }
            break;
        }
        case immediateToRegOp:
            instruction.w = buffer[i] & 0b00001000;
            instruction.reg = buffer[i] & 0b00000111;

            if (instruction.w) {
                instruction.immediate = (unsigned short)(buffer[i + 2]) * 256 + buffer[i + 1];
                i += 3;
            }
            else {
                instruction.immediate = buffer[i + 1];
                i += 2;
            }
            break;
        case immediateToRegMemOp:
            instruction.w = buffer[i] & 0b00000001;
            instruction.mod = buffer[i + 1] >> 6;
            instruction.rm = buffer[i + 1] & 0b00000111;
            instruction.displacement = getDisplacement(buffer, instruction.w, instruction.mod, instruction.rm, i);
            instruction.immediate = getData(buffer, instruction.w, i);
            i += 1 + instruction.w;
            break;
        case moveOp:
            parseDwInstruction(buffer, i, instruction);
            break;
        case immediateFromRegOp: {
            instruction.opMid = buffer[i + 1] >> 3 & 0b00000111;
            instruction.s = buffer[i] >> 1 & 0b00000001;
            instruction.w = buffer[i] & 0b00000001;
            instruction.mod = buffer[i + 1] >> 6;
            instruction.rm = buffer[i + 1] & 0b00000111;
            instruction.displacement = getDisplacement(buffer, instruction.w, instruction.mod, instruction.rm, i);
            bool twoByteData = instruction.w && !instruction.s;
            instruction.immediate = getData(buffer, twoByteData, i);
            i += 1 + twoByteData;
            break;
        }
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
            instruction.jumpOffset = buffer[i + 1];
            i += 2;
            break;
        default:
            cout << "opcode not implemented " << bitset<8>(instruction.opcode).to_string() << std::endl;
            break;
        }

        instruction.size = i - instruction.address;
        instructions.push_back(instruction);
    }
    return instructions;
}

unsigned char AsmReader::getOperation(unsigned char byte)
{
    const unsigned char op2 = byte >> 6;
    if (op2 == addSubCmpOp) {
        return addSubCmpOp;
    }
    const unsigned char op4 = byte >> 4;
    if (op4 == immediateToRegOp) {
        return immediateToRegOp;
    }

    const unsigned char op6 = byte >> 2;
    if (op6 == moveOp) {
        return moveOp;
    }
    if (op6 == immediateFromRegOp) {
        return immediateFromRegOp;
    }
    const unsigned char op7 = byte >> 1;
    if (op7 == immediateToRegMemOp) {
        return immediateToRegMemOp;
    }

    return byte;
}
