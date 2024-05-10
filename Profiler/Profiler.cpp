#define NameConcat2(A, B) A##B
#define NameConcat(A, B) NameConcat2(A, B)
#define TimeFunction() TimeBlock(__FUNCTION__)
#define TimeBlock(name) auto NameConcat(Block, __LINE__) = Profiler::_timeNamed(name, __COUNTER__ +1) // global counter only works with 1 translation unit
#define EndAndPrintResults() Profiler::_endAndPrintResults()
#define StartProfiling() Profiler()
#include "../ApproximateCPUTimerFreq/ApproximateCPUTimerFreq.cpp"
#include <string>
#include <iostream>

using std::string;
using std::cout;

struct Timing {
	u64 start;
	string name;
	u64 end;
	int parentIndex; // aggregate timing
};

struct AggregateTiming {
	string name;
	u64 time;
	u64 childrenTime;
};

static class Profiler
{
private:
	static const int maxCount = 1000;
	static u64 _cpuTimeFreq;
	static u64 _cpuTimeStart;
	static AggregateTiming _timings[maxCount];
	static int _timingsCount;
	static int _currentTimingIndex;
	public:
	Profiler() {
		_timingsCount = 0;
		_cpuTimeFreq = getCpuTimerFreq(1000000);
		_cpuTimeStart = __rdtsc();
	}
	static void _endAndPrintResults() {
		u64 cpuTimeEnd = __rdtsc();
		u64 cpuTimeElapsed = cpuTimeEnd - _cpuTimeStart;
		double cpuTimeElapsedMs = (double)cpuTimeElapsed * 1000 / (double)_cpuTimeFreq;
		cout << "CPU Time Elapsed: " << cpuTimeElapsedMs << " ms" << std::endl;

		for (int i = 1; i <= _timingsCount; ++i) {
			auto &t = _timings[i];
			auto time = double(t.time - t.childrenTime) * 1000 / _cpuTimeFreq;
			auto percentage = time * 100 / cpuTimeElapsedMs;
			cout << "Timing: " << _timings[i].name << " Time: " << time << "ms" << "(" << percentage << "%)";
			if (t.childrenTime > 0) {
				auto timeIncludingChildren = double(t.time) * 1000 / _cpuTimeFreq;
				auto percentageInclChildren = timeIncludingChildren * 100 / cpuTimeElapsedMs;
				cout << " ,incl. children: " << timeIncludingChildren << "ms" << "(" << percentageInclChildren << "%)";
			}
			cout << std::endl;
		}
	}
	static void SumbitTiming(string name, int index, int parentIndex, u64 start, u64 end) {
		if (index >= maxCount) {
			cout << "Error: too many timings" << std::endl;
		}
		Timing timing;
		timing.name = name;
		timing.start = start;
		timing.end = end;
		_timings[index].name = name;
		_timings[index].time += end - start;
		_timingsCount = max(_timingsCount, index);
		if (parentIndex > 0) {
			_timings[parentIndex].childrenTime += end - start;
		}
		_currentTimingIndex = parentIndex;
	}

	class Timer
	{
		string _name;
		u64 _start;
		int _index;
		int _parentIndex;
	public:
		Timer(string name, int index, int parentIndex) {
			_name = name;
			_start = __rdtsc();
			_index = index;
			_parentIndex = parentIndex;
		}
		~Timer() {
			auto end = __rdtsc();
			Profiler::SumbitTiming(_name, _index, _parentIndex, _start, end);
		}
	};
	
	static Timer _timeNamed(const string& name, int index) {
		auto t = Timer(name, index, _currentTimingIndex);
		_currentTimingIndex = index;
		return t;
	}
	static void Reset();

};

u64 Profiler::_cpuTimeFreq;
u64 Profiler::_cpuTimeStart;
int Profiler::_timingsCount;
AggregateTiming Profiler::_timings[maxCount];
int Profiler::_currentTimingIndex;