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
using std::string;
using std::vector;
using hrclock = std::chrono::high_resolution_clock;
using namespace std::chrono_literals;

u64 cpuTimeFreq;
const int pageCount = 128;
const int pageSize = 4096;

void TestAscending() {
    char* buffer = (char*)VirtualAlloc(NULL, pageSize * pageCount, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    RepetitionTester tester(10, pageSize, cpuTimeFreq);
    tester.Start();
    int start = tester.GetPageFaultCountForProcess();
    for (int i = 0; i < pageCount; ++i) {
        TimeBandwidth("fread", pageSize);
        buffer[i * pageSize] = rand();
        int count = tester.GetPageFaultCountForProcess() - start;
        std::cout << "Written page " << i << ", total page faults: " << count << std::endl;
    }
    tester.Finish();
    tester.Print();
}

void TestDescending() {
    char* buffer = (char*)VirtualAlloc(NULL, pageSize * pageCount, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    RepetitionTester tester(10, pageSize, cpuTimeFreq);
    tester.Start();
    int start = tester.GetPageFaultCountForProcess();
    for (int i = pageCount - 1; i >= 0; --i) {
        TimeBandwidth("fread", pageSize);
        buffer[i * pageSize] = rand();
        int count = tester.GetPageFaultCountForProcess() - start;
        std::cout << "Written page " << i << ", total page faults: " << count << std::endl;
    }
    tester.Finish();
    tester.Print();
}



int main()
{
    cpuTimeFreq = getCpuTimerFreq(1000000);
    TestAscending();
    TestDescending();
}
