#include <iostream>
#include <fstream>
#include <bitset>
#include"AsmReader.h"
#include"Printer.h"
#include "Constants.h"
#include "Simulator.h"
using std::cout;
using std::string;
using std::ofstream;
using std::bitset;

int main(int argc, char *argv[]) {
    AsmReader asmReader;
    auto fileName = argv[1];
    auto instructions = asmReader.read(fileName);

    ofstream outfile;
    auto outFileName = std::string(fileName) + "_print.asm";
    outfile.open(outFileName.c_str());
    Printer printer(outfile);
    Simulator simulator;
    for (const auto& i : instructions) {
        //printer.print(i);
        simulator.Execute(i);
    }
    simulator.PrintRegisters();
    outfile.close();
}