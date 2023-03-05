#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
using std::cout;
using std::string;
using std::ofstream;

int moveOp = 0b100010;
string w0Registers[8] = {
    "AL",
    "CL",
    "DL",
    "BL",
    "AH",
    "CH",
    "DH",
    "BH"
};
string w1Registers[8] = {
    "AX",
    "CX",
    "DX",
    "BX",
    "SP",
    "BP",
    "SI",
    "DI"
};

string mods[4] = {
    "mem mode, no displacement",
    "mem mode, 8-bit displacement follows",
    "mem mode, 16-bit displacement follows",
    "reg mode, no displacement"
};

int main()
{
    FILE* fileptr;
    unsigned char* buffer;
    long filelen;

    fileptr = fopen("listing38", "rb");  // Open the file in binary mode
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
        unsigned char op = buffer[i] >> 2;
        bool d = (buffer[i] & 0b00000010) >> 1;
        bool w = buffer[i] & 0b00000001;

        unsigned char mod = buffer[i + 1] >> 6;
        unsigned char reg = (buffer[i + 1] >> 3) & 0b00000111;
        unsigned char rm = buffer[i + 1] & 0b00000111;

        if (op == moveOp) {
            outfile << "mov ";
        }
        if (w) {
            outfile << w1Registers[d ? reg : rm] << ", " << w1Registers[d ? rm : reg];
        }
        else {
            outfile << w0Registers[d ? reg : rm] << ", " << w0Registers[d ? rm : reg];
        }
        outfile << std::endl;
        i += 2;
    }
    outfile.close();
}