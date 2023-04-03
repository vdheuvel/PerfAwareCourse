#pragma once
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

const string w0mod11Registers[8] = {
    "AL",
    "CL",
    "DL",
    "BL",
    "AH",
    "CH",
    "DH",
    "BH"
};
const string w1mod11Registers[8] = {
    "AX",
    "CX",
    "DX",
    "BX",
    "SP",
    "BP",
    "SI",
    "DI"
};
const string addressCalc[8] = {
    "BX + SI",
    "BX + DI",
    "BP + SI",
    "BP + DI",
    "SI",
    "DI",
    "BP", // exception for mod 00
    "BX",
};