#define TimeFunction() auto _ = Profiler::_timeNamed(__FUNCTION__)
#define TimeBlock(name) auto _ = Profiler::_timeNamed(name)
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
};

struct AggregateTiming {
	string name;
	u64 time;
	double percentage;
};

static class Profiler
{
private:
	static const int maxCount = 1000;
	static u64 _cpuTimeFreq;
	static u64 _cpuTimeStart;
	static Timing _timings[maxCount];
	static int _timingsCount;
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
		AggregateTiming aggTimings[maxCount];
		int i = 0;
		int j = 0;
		int aggTimingsCount = 0;
		//aggregate
		for (int i = 0; i < _timingsCount; ++i) {
			Timing timing = _timings[i];
			bool found = false;
			for (int j = 0; j < aggTimingsCount; ++j) {
				if (aggTimings[j].name == timing.name) {
					aggTimings[j].time += timing.end - timing.start;
					found = true;
					break;
				}
			}
			if (!found) {
				if (aggTimingsCount >= maxCount) {
					cout << "Error: too many timings" << std::endl;
				}
				aggTimings[aggTimingsCount].name = timing.name;
				aggTimings[aggTimingsCount].time = timing.end - timing.start;
				++aggTimingsCount;
			}
		}
		//print
		for (int i = 0; i < aggTimingsCount; ++i) {
			auto time = double(aggTimings[i].time) * 1000 / _cpuTimeFreq;
			aggTimings[i].percentage = time * 100 / cpuTimeElapsedMs;
			cout << "Timing: " << aggTimings[i].name << " Time: " << time << "ms" << "(" << aggTimings[i].percentage << "%)" << std::endl;
		}
	}
	static void SumbitTiming(string name, u64 start, u64 end) {
		if (_timingsCount >= maxCount) {
			cout << "Error: too many timings" << std::endl;
		}
		Timing timing;
		timing.name = name;
		timing.start = start;
		timing.end = end;
		_timings[_timingsCount] = timing;
		++_timingsCount;
	}

	class Timer
	{
		string _name;
		u64 _start;
	public:
		Timer(string name) {
			_name = name;
			_start = __rdtsc();
		}
		~Timer() {
			auto end = __rdtsc();
			Profiler::SumbitTiming(_name, _start, end);
		}
	};
	
	static Timer _timeNamed(const string& name) {
		return Timer(name);
	}
	static void Reset();

};

u64 Profiler::_cpuTimeFreq;
u64 Profiler::_cpuTimeStart;
int Profiler::_timingsCount;
Timing Profiler::_timings[maxCount];