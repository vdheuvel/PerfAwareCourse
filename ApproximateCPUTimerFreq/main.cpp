#include <iostream>
#include "ApproximateCPUTimerFreq.cpp"

int main()
{
	u64 freq = getCpuTimerFreq(1000000);
	std::cout << "CPU Timer Frequency: " << freq << " Hz" << std::endl;
	return 0;
}