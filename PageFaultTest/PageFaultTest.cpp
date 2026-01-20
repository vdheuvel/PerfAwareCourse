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
const int pageCount = 1024;
const int pageSize = 4096;

void TestAscending(bool useMalloc) {
    RepetitionTester tester(10, pageSize * pageCount, cpuTimeFreq);
    tester.Start();
    while (!tester.IsDone()) {
        char* buffer = useMalloc ? (char*)malloc(pageSize * pageCount) :
            (char*)VirtualAlloc(NULL, pageSize * pageCount, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        int start = tester.GetPageFaultCountForProcess();
        for (int i = 0; i < pageCount; ++i) {
            TimeBandwidth("fread", pageSize);
            buffer[i * pageSize] = rand();
            for (int j = 0; j < pageSize; ++j) {
                buffer[i * pageSize + j] = j;
            }
            int count = tester.GetPageFaultCountForProcess() - start;
            //std::cout << "Written page " << i << ", total page faults: " << count << std::endl;
        }
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}

void TestDescending(bool useMalloc) {
    RepetitionTester tester(10, pageSize * pageCount, cpuTimeFreq);
    tester.Start();
    int start = tester.GetPageFaultCountForProcess();
    while (!tester.IsDone()) {
        char* buffer = useMalloc ? (char*)malloc(pageSize * pageCount) :
            (char*)VirtualAlloc(NULL, pageSize * pageCount, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        for (int i = pageCount - 1; i >= 0; --i) {
            TimeBandwidth("fread", pageSize);
            buffer[i * pageSize] = rand();
            for (int j = 0; j < pageSize; ++j) {
                buffer[i * pageSize + j] = j;
            }
            int count = tester.GetPageFaultCountForProcess() - start;
            //std::cout << "Written page " << i << ", total page faults: " << count << std::endl;
        }
        tester.SubmitRepetition();
    }
    tester.Finish();
    tester.Print();
}



int main()
{
    // note: memory will fill up with all tests
    cpuTimeFreq = getCpuTimerFreq(1000000);
    std::cout << "Test ascending with VirtualAlloc" << std::endl;
    TestAscending(false); // 0.0007 ms because of prefetching
    //std::cout << "Test descending with VirtualAlloc" << std::endl;
    //TestDescending(false); // 0.0013 ms
    //std::cout << "Test ascending with malloc" << std::endl;
    //TestAscending(true); // 0.0008 ms
    //std::cout << "Test descending with malloc" << std::endl;
    //TestDescending(true); // 0.0013
}
