// ApproximateCPUTimerFreq.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <intrin.h>
#include <Windows.h>

using u64 = unsigned long long;

static u64 getOsTimerFreq(void) {
	LARGE_INTEGER value;
	QueryPerformanceCounter(&value);
	return value.QuadPart;
}

// duration in one ten-millionth of a second
static u64 getCpuTimerFreq(int duration) {
	u64 start = __rdtsc();
	u64 startOs = getOsTimerFreq();
	u64 endOs = startOs;
	while (startOs + duration > endOs) {
		endOs = getOsTimerFreq();
	}
	u64 end = __rdtsc();
	return (end - start) * 10000000 / duration;
}
