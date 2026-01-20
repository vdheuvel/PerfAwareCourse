#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <intrin.h>
#include <iomanip>
#include "../Profiler/ApproximateCPUTimerFreq.cpp"
#include "../Profiler/Profiler.cpp"

using f64 = double;
using std::string;
using std::vector;
using hrclock = std::chrono::high_resolution_clock;
using namespace std::chrono_literals;

u64 cpuTimeFreq;

void TouchMemory(char* buffer, size_t size)
{
    TimeFunction();
    for (size_t i = 0; i < size; i += 4096) {
        buffer[i] = i;
    }
}

int main()
{
    cpuTimeFreq = getCpuTimerFreq(1000000);
    Profiler();
    Profiler::_cpuTimeFreq = cpuTimeFreq;

    const size_t dataSize = 1024 * 1024 * 100;
    char* data = (char*)VirtualAlloc(NULL, dataSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!data) {
        std::cout << "VirtualAlloc failed\n";
        return 1;
    }
    for (int i = 0; i < 10; ++i) {
        std::cout << "Iteration " << i + 1 << "\n";
        TouchMemory(data, dataSize);
    }

    EndAndPrintResults();

    std::cout << "Done!\n";
}