#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <bitset>
using std::cout;
using std::string;
using std::ofstream;
using std::bitset;

const int moveOp = 0b100010;
const int immediateToRegOp = 0b1011;
string w0mod11Registers[8] = {
    "AL",
    "CL",
    "DL",
    "BL",
    "AH",
    "CH",
    "DH",
    "BH"
};
string w1mod11Registers[8] = {
    "AX",
    "CX",
    "DX",
    "BX",
    "SP",
    "BP",
    "SI",
    "DI"
};
string addressCalc[8] = {
    "BX + SI",
    "BX + DI",
    "BP + SI",
    "BP + DI",
    "SI",
    "DI",
    "BP", // exception for mod 00
    "BX",
};

int main()
{
    FILE* fileptr;
    unsigned char* buffer;
    long filelen;

    fileptr = fopen("listing39", "rb");  // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file

    buffer = (unsigned char*)malloc(filelen * sizeof(unsigned char)); // Enough memory for the file
    fread(buffer, filelen, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file



    ofstream outfile;
    outfile.open("out.asm");
    outfile << "bits 16" << std::endl;
    int i = 0;

    while (i < filelen) {
        unsigned char op4 = buffer[i] >> 4;

        switch (op4) {
            case immediateToRegOp:
            {
                bool w = buffer[i] & 0b00001000;
                unsigned char reg = buffer[i] & 0b00000111;
                outfile << "mov ";
                if (w) {
                    outfile << w1mod11Registers[reg] << ", " << (unsigned short)(buffer[i + 2]) * 256 + buffer[i + 1];
                    i += 3;
                }
                else {
                    outfile << w0mod11Registers[reg] << ", " << buffer[i + 1];
                    i += 2;
                }
                break;
            }
            default:
            unsigned char op6 = buffer[i] >> 2;
            switch (op6)
            {
                case moveOp:
                {
                    bool d = (buffer[i] & 0b00000010) >> 1;
                    bool w = buffer[i] & 0b00000001;
                    unsigned char mod = buffer[i + 1] >> 6;
                    unsigned char reg = (buffer[i + 1] >> 3) & 0b00000111;
                    unsigned char rm = buffer[i + 1] & 0b00000111;
                    outfile << "mov ";
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

                    if (mod == 0b11) {
                        if (w) {
                            outfile << w1mod11Registers[d ? reg : rm] << ", " << w1mod11Registers[d ? rm : reg];
                        }
                        else {
                            outfile << w0mod11Registers[d ? reg : rm] << ", " << w0mod11Registers[d ? rm : reg];
                        }
                    }
                    else {
                        string registerstr;
                        if (w) {
                            registerstr = w1mod11Registers[reg];
                        }
                        else {
                            registerstr = w0mod11Registers[reg];
                        }
                        string addresscalcstr = addressCalc[rm];
                        if (d) {
                            outfile << registerstr << ", [" << addresscalcstr;
                            if (displacement != 0) {
                                outfile << " + " << displacement;
                            }
                            outfile << "]";
                        }
                        else {
                            outfile << "[" << addresscalcstr;
                            if (displacement != 0) {
                                outfile << " + " << displacement;

                            }
                            outfile << "], " << registerstr;
                        }
                    }

                    break;
                }
                default:
                    cout << "opcode not implemented " << bitset<8>(op6).to_string() << std::endl;
                    return -1;
            }
        }
        outfile << std::endl;
    }
    outfile.close();
    cout << "done";
}