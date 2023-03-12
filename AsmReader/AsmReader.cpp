#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <bitset>
using std::cout;
using std::string;
using std::ofstream;
using std::bitset;

const int moveOp = 0b100010; // op6
const int immediateToRegOp = 0b1011; // op4
const int addOp = 0;
const int subOp = 0b101;
const int cmpOp = 0b111;
const int immediateFromRegOp = 0b100000; // op6
const int addSubCmpOp = 0; // op2
const int jnzOp = 0b01110101; // op8
const int jeOp = 0b01110100; // op8
const int jlOp = 0b01111100; // op8
const int jleOp = 0b01111110; // op8
const int jbOp = 0b01110010; // op8
const int jbeOp = 0b01110110; // op8
const int jpOp = 0b01111010; // op8
const int joOp = 0b01110000; // op8
const int jsOp = 0b01111000; // op8
const int jneOp = 0b01110101; // op8
const int jnlOp = 0b01111101; // op8
const int jgOp = 0b01111111; // op8
const int jnbOp = 0b01110011; // op8
const int jaOp = 0b01110111; // op8
const int jnpOp = 0b01111011; // op8
const int jnoOp = 0b01110001; // op8
const int jnsOp = 0b01111001; // op8
const int loopOp = 0b11100010; // op8
const int loopzOp = 0b11100001; // op8
const int loopnzOp = 0b11100000; // op8
const int jcxzOp = 0b11100011; // op8

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

int writeDwInstruction(string op, unsigned char *buffer, ofstream &outfile, int &i){
    bool d = (buffer[i] & 0b00000010) >> 1;
    bool w = buffer[i] & 0b00000001;
    unsigned char mod = buffer[i + 1] >> 6;
    unsigned char reg = (buffer[i + 1] >> 3) & 0b00000111;
    unsigned char rm = buffer[i + 1] & 0b00000111;
    outfile << op << " ";
    auto displacement = getDisplacement(buffer, w, mod, rm, i);

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
    return i;
}

int main()
{
    FILE* fileptr;
    unsigned char* buffer;
    long filelen;

    fileptr = fopen("listing41", "rb");  // Open the file in binary mode
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
        const unsigned char op2 = buffer[i] >> 6;
        if (op2 == addSubCmpOp) {
            const unsigned char opMid = buffer[i] >> 3 & 0b00000111;
            string op = getOp(opMid);
            bool d = (buffer[i] & 0b00000010) >> 1;
            bool w = buffer[i] & 0b00000001;
            bool accumulator = buffer[i] >> 2 & 0b00000001;
            if (accumulator) {
                outfile << op;
                if (w) {
                    outfile << " ax, " << (unsigned short)(buffer[i + 2]) * 256 + buffer[i + 1];
                    i += 3;
                }
                else {
                    outfile << " al, " << (unsigned short)buffer[i + 1];
                    i += 2;
                }
            }
            else {
                writeDwInstruction(op, buffer, outfile, i);
            }

        }
        else {
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
                        writeDwInstruction("mov", buffer, outfile, i);
                        break;
                    }
                    case immediateFromRegOp:
                    {
                        int subOpCode = buffer[i + 1] >> 3 & 0b00000111;
                        string op = getOp(subOpCode);
                        bool s = buffer[i] >> 1 & 0b00000001;
                        bool w = buffer[i] & 0b00000001;
                        unsigned char mod = buffer[i + 1] >> 6;
                        unsigned char rm = buffer[i + 1] & 0b00000111;

                        outfile << op << " ";
                        if (mod != 3) { // memory mode
                            outfile << (w ? "word" : "byte") << " [";
                        }

                        auto displacement = getDisplacement(buffer, w, mod, rm, i);
                        bool twoByteData = w && !s;
                        int data = getData(buffer, twoByteData, i);
                        i += 1 + twoByteData;
                        string addresscalcstr;
                        if (mod == 3) {
                            if (w) {
                                addresscalcstr = w1mod11Registers[rm];
                            }
                            else {
                                addresscalcstr = w0mod11Registers[rm];
                            }
                        }
                        else {
                            addresscalcstr = addressCalc[rm];
                        }
                        //string addresscalcstr = addressCalc[rm];
                        
                        outfile << addresscalcstr;

                        if (mod == 1 || mod == 2) { //displacement
                            outfile << (displacement >= 0 ? " + " : " - ") << displacement;
                        }

                        if (mod != 3) { // memory mode
                            outfile << "]";
                        }

                        outfile << ", " << data;


                        break;
                    }
                    default:
                        if (buffer[i] == jnzOp) {
                            outfile << "jnz";
                        }
                        else if (buffer[i] == jeOp) {
                            outfile << "je";
                        }
                        else if (buffer[i] == jlOp) {
                            outfile << "jl";
                        }
                        else if (buffer[i] == jleOp) {
                            outfile << "jle";
                        }
                        else if (buffer[i] == jbOp) {
                            outfile << "jb";
                        }
                        else if (buffer[i] == jbeOp) {
                            outfile << "jbe";
                        }
                        else if (buffer[i] == jpOp) {
                            outfile << "jp";
                        }
                        else if (buffer[i] == joOp) {
                            outfile << "jo";
                        }
                        else if (buffer[i] == jsOp) {
                            outfile << "js";
                        }
                        else if (buffer[i] == jneOp) {
                            outfile << "jne";
                        }
                        else if (buffer[i] == jnlOp) {
                            outfile << "jnl";
                        }
                        else if (buffer[i] == jgOp) {
                            outfile << "jg";
                        }
                        else if (buffer[i] == jnbOp) {
                            outfile << "jnb";
                        }
                        else if (buffer[i] == jaOp) {
                            outfile << "ja";
                        }
                        else if (buffer[i] == jnpOp) {
                            outfile << "jnp";
                        }
                        else if (buffer[i] == jnoOp) {
                            outfile << "jno";
                        }
                        else if (buffer[i] == jnsOp) {
                            outfile << "jns";
                        }
                        else if (buffer[i] == loopOp) {
                            outfile << "loop";
                        }
                        else if (buffer[i] == loopzOp) {
                            outfile << "loopz";
                        }
                        else if (buffer[i] == loopnzOp) {
                            outfile << "loopnz";
                        }
                        else if (buffer[i] == jcxzOp) {
                            outfile << "jcxz";
                        }
                        else {
                            cout << "opcode not implemented " << bitset<8>(op6).to_string() << std::endl;
                            return -1;
                        }
                        signed char offset = buffer[i + 1];
                        i += 2;
                        outfile << " $ + 2" << (offset >= 0 ? " + " : "") << (signed short)offset;
                }
            }
        }

        outfile << std::endl;
    }
    outfile.close();
    cout << "done";
}