#define PROFILER 1
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <intrin.h>
#include "../Profiler/ApproximateCPUTimerFreq.cpp"
#include <iomanip>
#include "../Profiler/Profiler.cpp"
#include <psapi.h>
#include "../Profiler/RepetitionTester.cpp"

using f64 = double;
using u8 = short;
using std::string;
using std::vector;
using std::cout;
using hrclock = std::chrono::high_resolution_clock;
using namespace std::chrono_literals;
using u32 = unsigned int;

u64 cpuTimeFreq;
int pageCount = 1024;
int pageSize = 4096;

extern "C" void MoveAllBytesASM(u64 count, u8* data);
extern "C" void NoOpASM(u64 count, u8* data);
extern "C" void CmpASM(u64 count, u8* data);
extern "C" void DecASM(u64 count, u8* data);


// to convince compiler that const size is not const, so it uses 3 byte compare (same as casey's)
// otherwise it'll use 7 byte compare
u64 opaque(u64 x) { return x; } 

void Test(u64 size) {
    RepetitionTester tester(10, pageSize * pageCount, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        char* buffer = (char*)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        for (u64 i = 0; i < size; ++i) {
            buffer[i] = (u8)i;
        }
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestMoveAllBytesASM(u64 size) {
    RepetitionTester tester(10, pageSize * pageCount, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        char* buffer = (char*)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        MoveAllBytesASM(size, (u8*)buffer);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestMoveAllBytesNOPASM(u64 size) {
    RepetitionTester tester(10, pageSize * pageCount, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        char* buffer = (char*)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        NoOpASM(size, (u8*)buffer);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestCmpASM(u64 size) {
    RepetitionTester tester(10, pageSize * pageCount, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        char* buffer = (char*)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        CmpASM(size, (u8*)buffer);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestDecASM(u64 size) {
    RepetitionTester tester(10, pageSize * pageCount, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        char* buffer = (char*)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        DecASM(size, (u8*)buffer);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}


int main()
{
    cpuTimeFreq = getCpuTimerFreq(1000000);
    cout << "cpu time freq " << cpuTimeFreq << std::endl;

    u64 size = pageCount * pageSize;
    //Test(opaque(size)); // 4.5; Casey: 3.5
    //TestMoveAllBytesASM(opaque(size)); // 4.5; Casey: 3.5
    //TestMoveAllBytesNOPASM(opaque(size)); // 8.7 ; Casey: 3.9
    //TestCmpASM(opaque(size)); // 8.7; Casey: 3.9
    TestDecASM(opaque(size)); // 4.4; Casey: 3.9
}