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
using u8 = uint8_t;
using std::string;
using std::vector;
using std::cout;
using hrclock = std::chrono::high_resolution_clock;
using namespace std::chrono_literals;
using u32 = unsigned int;

u64 cpuTimeFreq;
int pageCount = 1024;
int pageSize = 4096;

u8 branch[1024 * 4096];

// assemble: C:\Users\JeroenvandenHeuvel\AppData\Local\bin\NASM > nasm -f win64 "C:\Users\JeroenvandenHeuvel\source\repos\vdheuvel\PerfAwareCourse\LoopAssembly\LoopAssemblyRaw.asm" -o "C:\Users\JeroenvandenHeuvel\source\repos\vdheuvel\PerfAwareCourse\LoopAssembly\LoopAssemblyRaw.obj"
// assemble: C:\Users\JeroenvandenHeuvel\AppData\Local\bin\NASM > nasm -f win64 "C:\Users\Jeroen\source\repos\PerfAwareCourse\LoopAssembly\LoopAssemblyRaw.asm" -o "C:\Users\Jeroen\source\repos\PerfAwareCourse\LoopAssembly\LoopAssemblyRaw.obj"
extern "C" void MoveAllBytesASM(u64 count, u8* data);
extern "C" void NoOp3x1ASM(u64 count, u8* data);
extern "C" void CmpASM(u64 count, u8* data);
extern "C" void DecASM(u64 count, u8* data);
extern "C" void NoOp1x3ASM(u64 count, u8* data);
extern "C" void NoOp1x9ASM(u64 count, u8* data);
extern "C" void NoOp3x1CondASM(u64 count, u8* data);

extern "C" void Load_1xASM(u64 count, u8* data);
extern "C" void Load_2xASM(u64 count, u8* data);
extern "C" void Load_3xASM(u64 count, u8* data);
extern "C" void Load_4xASM(u64 count, u8* data);
extern "C" void Store_1xASM(u64 count, u8* data);
extern "C" void Store_2xASM(u64 count, u8* data);
extern "C" void Store_3xASM(u64 count, u8* data);
extern "C" void Store_4xASM(u64 count, u8* data);

// assemble: C:\Users\JeroenvandenHeuvel\AppData\Local\bin\NASM > nasm -f win64 "C:\Users\JeroenvandenHeuvel\source\repos\vdheuvel\PerfAwareCourse\LoopAssembly\LoopLoadStore.asm" -o "C:\Users\JeroenvandenHeuvel\source\repos\vdheuvel\PerfAwareCourse\LoopAssembly\LoopLoadStore.obj"
// assemble: C:\Users\JeroenvandenHeuvel\AppData\Local\bin\NASM > nasm -f win64 "C:\Users\Jeroen\source\repos\PerfAwareCourse\LoopAssembly\LoopLoadStore.asm" -o "C:\Users\Jeroen\source\repos\PerfAwareCourse\LoopAssembly\LoopLoadStore.obj"
extern "C" void Load_128x1ASM(u64 count, u8* data);
extern "C" void Load_128x2ASM(u64 count, u8* data);
extern "C" void Load_128x3ASM(u64 count, u8* data);
extern "C" void Load_256x1ASM(u64 count, u8* data);
extern "C" void Load_256x2ASM(u64 count, u8* data);
extern "C" void Load_256x3ASM(u64 count, u8* data);
extern "C" void Load_512x1ASM(u64 count, u8* data);
extern "C" void Load_512x2ASM(u64 count, u8* data);
extern "C" void Load_512x3ASM(u64 count, u8* data);
// assemble: C:\Users\JeroenvandenHeuvel\AppData\Local\bin\NASM > nasm -f win64 "C:\Users\JeroenvandenHeuvel\source\repos\vdheuvel\PerfAwareCourse\LoopAssembly\LoopLoadSIMD.asm" -o "C:\Users\JeroenvandenHeuvel\source\repos\vdheuvel\PerfAwareCourse\LoopAssembly\LoopLoadSIMD.obj"
// assemble: nasm -f win64 "C:\Users\Jeroen\source\repos\PerfAwareCourse\LoopAssembly\LoopLoadSIMD.asm" -o "C:\Users\Jeroen\source\repos\PerfAwareCourse\LoopAssembly\LoopLoadSIMD.obj"

extern "C" void CacheTestASM(u64 count, u8* data, u64 mask);
// nasm -f win64 "C:\Users\Jeroen\source\repos\PerfAwareCourse\LoopAssembly\CacheTest.asm" -o "C:\Users\Jeroen\source\repos\PerfAwareCourse\LoopAssembly\CacheTest.obj"

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

void TestMoveAllBytesNOP3x1ASM(u64 size) {
    RepetitionTester tester(10, pageSize * pageCount, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        char* buffer = (char*)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        NoOp3x1ASM(size, (u8*)buffer);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}
void TestMoveAllBytesNOP1x3ASM(u64 size) {
    RepetitionTester tester(10, pageSize * pageCount, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        char* buffer = (char*)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        NoOp1x3ASM(size, (u8*)buffer);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}
void TestMoveAllBytesNOP1x9ASM(u64 size) {
    RepetitionTester tester(10, pageSize * pageCount, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        char* buffer = (char*)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        NoOp1x9ASM(size, (u8*)buffer);
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

void TestCondASM(u64 size) {
    RepetitionTester tester(10, pageSize * pageCount, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        NoOp3x1CondASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}


void setBranchArray(u64 size, int option) {
    for (u64 i = 0; i < size; ++i) {
        switch (option) {
        case 0:
            branch[i] = 0;
            break;
        case 1:
            branch[i] = 1;
            break;
        case 2:
            branch[i] = (i % 2) == 0;
            break;
        case 3:
            branch[i] = (rand() % 2) == 0;
            break;
        case 4:
            branch[i] = (i % 3) == 0;
            break;
        }
    }
}

void TestLoad_1xASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_1xASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestLoad_2xASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_2xASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestLoad_3xASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_3xASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestLoad_4xASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_4xASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestStore_1xASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Store_1xASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestStore_2xASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Store_2xASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestStore_3xASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Store_3xASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestStore_4xASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Store_4xASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}


void TestLoad_128x1ASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_128x1ASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestLoad_128x2ASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_128x2ASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}
void TestLoad_128x3ASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_128x3ASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestLoad_256x1ASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_256x1ASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestLoad_256x2ASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_256x2ASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}
void TestLoad_256x3ASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_256x3ASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}
void TestLoad_512x1ASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_512x1ASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestLoad_512x2ASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_512x2ASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}
void TestLoad_512x3ASM(u64 size) {
    RepetitionTester tester(10, size, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        Load_512x3ASM(size, (u8*)branch);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}
void CacheTestASM(u64 size, u64 mask) {
    u64 cacheTestSize = 1024 * 1024 * 1024;
    u64 repetitions = cacheTestSize * 10;
    char* buffer = (char*)VirtualAlloc(NULL, cacheTestSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    for(u64 i = 0; i < size; ++i) {
        buffer[i] = (u8)i;
    }
    RepetitionTester tester(10, repetitions, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        CacheTestASM(repetitions, (u8*)buffer, mask);
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}
void CacheTestOffsetASM(u64 size, u64 mask) {
    u64 cacheTestSize = 1024 * 1024 * 1024;
    u64 repetitions = cacheTestSize * 10;
    char* buffer = (char*)VirtualAlloc(NULL, cacheTestSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    for(u64 i = 0; i < size; ++i) {
        buffer[i] = (u8)i;
    }
    u64 offsets[] = { 1, 4,7, 8, 15, 16, 32, 33 };
    for (int i = 0; i < 7; ++i) {
        std::cout << "offset " << offsets[i] << std::endl;
        RepetitionTester tester(10, repetitions, cpuTimeFreq);
        tester.Start();
        while (!tester.IsDone()) {
            CacheTestASM(repetitions, (u8*)buffer + offsets[i], mask);
            tester.SubmitRepetition();
        }
        tester.Finish();
        tester.Print();
    }
}
int main()
{
    cpuTimeFreq = getCpuTimerFreq(1000000);
    cout << "cpu time freq " << cpuTimeFreq << std::endl;

    u64 size = pageCount * pageSize;
    // timings are on work laptop with i7-1255U
    //Test(opaque(size)); // 4.5; Casey: 3.5
    //TestMoveAllBytesASM(opaque(size)); // 4.5; Casey: 3.5
    //TestMoveAllBytesNOP3x1ASM(opaque(size)); // 8.7 ; Casey: 3.9
    //TestCmpASM(opaque(size)); // 8.7; Casey: 3.9          
    //TestDecASM(opaque(size)); // 4.4; Casey: 3.9
    //TestMoveAllBytesNOP1x3ASM(opaque(size)); // 4.1; Casey: 3.1
    //TestMoveAllBytesNOP1x9ASM(opaque(size)); // 2.0; Casey: 1.4
    //string branchPatterns[] = 
    //{ 
    //    "all taken", // 2.4
    //    "all not taken", // 2.4
    //    "alternating", // 2.4
    //    "pseudo random", // 0.27
    //    "2 out of 3 taken" // 2.4
    //};
    //int i = 4;
    setBranchArray(size, 0);
    //cout << "branch pattern " << i << " " << branchPatterns[i] << std::endl;
    //TestCondASM(opaque(size));

    //TestLoad_1xASM(size); // 4.4; 5.2
    //TestLoad_2xASM(size); // 8.7; 10.5
    //TestLoad_3xASM(size); // 8.7; 15.7
    //TestLoad_4xASM(size); // 8.7; 21
    //TestStore_1xASM(size); // 8.7; 5.2
    //TestStore_2xASM(size); // 8.7; 10.5
    //TestStore_3xASM(size); // 8.7; 10.5
    //TestStore_4xASM(size); // 8.7; 10.5
    //TestLoad_128x1ASM(size); // 8.7; 10.5
    //TestLoad_128x2ASM(size); // 17.5; 21
    //TestLoad_128x3ASM(size); // 17.5; 21
    //TestLoad_256x1ASM(size); // 17.5; 21
    //TestLoad_256x2ASM(size); // 34.9; 41.9
    //TestLoad_256x3ASM(size); // 34.9; 41.9
    //TestLoad_512x1ASM(size); // NA; 41.9
    //TestLoad_512x2ASM(size); // NA; 83.7
    u64 cacheTestSize = 1024 * 1024 * 1024;
    //CacheTestASM(cacheTestSize, 63); //
    //CacheTestASM(cacheTestSize, 127); // 333
    //CacheTestASM(cacheTestSize, 255); //
    //CacheTestASM(cacheTestSize, 511); //
    //CacheTestASM(cacheTestSize, 1023); // 10
    //CacheTestASM(cacheTestSize, 2047); // 11
    //CacheTestASM(cacheTestSize, 4095); // 12
    //CacheTestASM(cacheTestSize, 8191); // 13 664
    //CacheTestASM(cacheTestSize, 16383); // 14 664
    //CacheTestASM(cacheTestSize, 32767); // 15 663
    //CacheTestASM(cacheTestSize, 65535); // 16 182 <- above L1 cache (80KB / 2 = 40KB)
    //CacheTestASM(cacheTestSize, 131071); // 17 194 
    //CacheTestASM(cacheTestSize, 262143); //  18 192
    //CacheTestASM(cacheTestSize, 524287); // 19 195 <- at L2 cache (1MB)
    //CacheTestASM(cacheTestSize, 1048575); // 20 172 
    CacheTestASM(cacheTestSize, 2097151); // 21 165
    //CacheTestASM(cacheTestSize, 4194303); // 22 
    //CacheTestASM(cacheTestSize, 8388607); // 23 
    //CacheTestASM(cacheTestSize, 16777215); // 24 155
    //CacheTestASM(cacheTestSize, 33554431); // 25 131 <- just above L3 cache (64MB / 2 = 32MB)
    //CacheTestASM(cacheTestSize, 67108863); // 26 74 
    //CacheTestASM(cacheTestSize, 134217727); // 27 58
    //CacheTestASM(cacheTestSize, 268435455); // 28
    //CacheTestASM(cacheTestSize, 268435455); // 28

    // limits: 50 GB/s out of 90 GB/s memory bandwidth
    // L3: 150 GB/s
    // L2: 210 GB/s
    // L1: 330 GB/s

    //CacheTestOffsetASM(cacheTestSize, 127); // L1 test. all 333, except 15, 16: 332
    //CacheTestOffsetASM(cacheTestSize, 262143); // L2 test. 1: 182, 4: 196, 7: 194, 8: 172, 15: 175, 16: 175, 32: 197
    //CacheTestOffsetASM(cacheTestSize, 2097151); // L3 test. 1: 152, 4: 149, 7: 149, 8: 150, 15: 149, 16: 150, 32: 150

    //theory: offset is mostly a performance penalty in L3

}