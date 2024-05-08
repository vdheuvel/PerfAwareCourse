#include <iostream>
#include <fstream>
#include <bitset>
#include"AsmReader.h"
#include"Printer.h"
#include "Constants.h"
#include "Simulator.h"
#include "ClockCalculator.h"
using std::cout;
using std::string;
using std::ofstream;
using std::bitset;

int main(int argc, char *argv[]) {
    AsmReader asmReader;
    auto fileName = argv[1];
    auto instructions = asmReader.read(fileName);

    ofstream outfile;
    //auto outFileName = std::string(fileName) + "_print.asm";
    auto outFileName = std::string(fileName) + ".DATA";
    outfile.open(outFileName.c_str());
    Printer printer(cout);
    ClockCalculator clockCalc;
    for (const auto& i : instructions) {
        int cost = clockCalc.Calculate(i);
        printer.print(i);
        cout << " cost:" << cost << std::endl;
    }
    Simulator simulator(instructions);
    auto maxIndex = instructions[instructions.size() - 1].address;
    while(simulator.instructionPointer <= maxIndex) {
        auto i = simulator.Execute();
        //printer.print(instructions[i]);
        cout << std::endl;
    }
    simulator.PrintRegisters();

    for (const auto& byte : simulator.getMemory()) {
        outfile << byte;
    }

    outfile.close();
}