#ifndef PROFILER
#define PROFILER 0
#endif // !PROFILER

#if PROFILER

#define NameConcat2(A, B) A##B
#define NameConcat(A, B) NameConcat2(A, B)
#define TimeFunction() TimeBlock(__FUNCTION__)
#define TimeBlock(name) auto NameConcat(Block, __LINE__) = Profiler::_timeNamed(name, __COUNTER__ +1) // global counter only works with 1 translation unit
#define TimeBandwidth(name, byteCount) auto NameConcat(Block, __LINE__) = Profiler::_timeNamed(name, __COUNTER__ +1, byteCount) // global counter only works with 1 translation unit


#else
#define TimeFunction(...) 
#define TimeBlock(...)

#endif // PROFILER

#define StartProfiling() Profiler()
#define EndAndPrintResults() Profiler::_endAndPrintResults()
#include "../ApproximateCPUTimerFreq/ApproximateCPUTimerFreq.cpp"
#include <string>
#include <iostream>
#include <iomanip>
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
	u64 timeExclChildren;
	u64 timeInclChildren;
	int hitCount;
	u64 byteCount;
};

static class Profiler
{
private:
	static const int maxCount = 100;
	static u64 _cpuTimeFreq;
	static u64 _cpuTimeStart;
	static AggregateTiming _timings[maxCount];
	static int _openedTimings[maxCount];
	static int _timingsCount;
	static int _currentTimingIndex;
public:
	Profiler() {
		_timingsCount = 0;
		_cpuTimeFreq = getCpuTimerFreq(1000000);
		_cpuTimeStart = GetQueryPerformanceCounter();
	}
	static u64 GetQueryPerformanceCounter() {
		u64 result;
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return li.QuadPart;
	}
	static void _endAndPrintResults() {
		u64 cpuTimeEnd = GetQueryPerformanceCounter();
		u64 cpuTimeElapsed = cpuTimeEnd - _cpuTimeStart;
		double cpuTimeElapsedMs = (double)cpuTimeElapsed / 10000;

		//cout.precision(4);
		cout << "CPU Time Elapsed: " << cpuTimeElapsedMs << " ms" << std::endl;

		for (int i = 1; i <= _timingsCount; ++i) {
			auto& t = _timings[i];
			auto time = double(t.timeExclChildren) * 1000 / _cpuTimeFreq;
			auto percentage = time * 100 / cpuTimeElapsedMs;
			cout << "  Timing: " << _timings[i].name << "[" << _timings[i].hitCount << "] Time: " << time << "ms" << "(" << percentage << "%)";
			if (t.timeExclChildren != t.timeInclChildren) {
				auto timeIncludingChildren = double(t.timeInclChildren) * 1000 / _cpuTimeFreq;
				auto percentageInclChildren = timeIncludingChildren * 100 / cpuTimeElapsedMs;
				cout << ", incl. children: " << timeIncludingChildren << "ms" << "(" << percentageInclChildren << "%)";
			}
			if (t.byteCount > 0) {
				const int megabyte = 1024 * 1024;
				const int gigabtye = 1024 * megabyte;
				auto timeIncludingChildren = double(t.timeInclChildren) * 1000 / _cpuTimeFreq;

				cout << std::setprecision(3) << ", Processed " << t.byteCount / megabyte << "MB at " << t.byteCount / timeIncludingChildren / gigabtye << "GB/s";
			}
			cout << std::endl;
		}
	}
	class Timer;
	static void SubmitTiming(Timer* t, u64 end) {
		if (t->_index >= maxCount) {
			cout << "Error: too many timings" << std::endl;
		}
		if (t->_index == 0) {
			return;
		}
		Timing timing;
		timing.name = t->_name;
		timing.start = t->_start;
		timing.end = end;
		_timings[t->_index].name = t->_name;
		auto elapsed = end - t->_start;
		_timings[t->_index].timeExclChildren += elapsed;
		_timings[t->_parentIndex].byteCount += t->_byteCount;

		_timings[t->_index].timeInclChildren = t->_startSum + elapsed; // overwrites value if any children added to it in the meantime
		_timingsCount = max(_timingsCount, t->_index);
		_timings[t->_parentIndex].timeExclChildren -= elapsed; // if parentindex is 0, it's implicitly discarded
		++_timings[t->_index].hitCount;

		_currentTimingIndex = t->_parentIndex;
		--_openedTimings[t->_index];
	}

	class Timer
	{
	public:
		string _name;
		u64 _start;
		int _index;
		int _parentIndex;
		u64 _startSum;
		u64 _byteCount;
		Timer(string name, int index, int parentIndex, u64 startSum, u64 byteCount = 0) {
			_name = name;
			_start = GetQueryPerformanceCounter();
			_index = index;
			_parentIndex = parentIndex;
			_startSum = startSum;
			_byteCount = byteCount;
		}
		~Timer() {
			auto end = GetQueryPerformanceCounter();
			Profiler::SubmitTiming(this, end);
		}
	};

	static Timer _timeNamed(const string& name, int index) {
		auto parent = _currentTimingIndex;
		_currentTimingIndex = index;
		++_openedTimings[index];
		return Timer(name, index, parent, _timings[index].timeExclChildren);
	}
	static Timer _timeNamed(const string& name, int index, int byteCount) {
		auto parent = _currentTimingIndex;
		_currentTimingIndex = index;
		++_openedTimings[index];

		return Timer(name, index, parent, _timings[index].timeExclChildren, byteCount);
	}
	
	static void Reset();

};

u64 Profiler::_cpuTimeFreq;
u64 Profiler::_cpuTimeStart;
int Profiler::_timingsCount;
AggregateTiming Profiler::_timings[maxCount];
int Profiler::_currentTimingIndex;
int Profiler::_openedTimings[maxCount];