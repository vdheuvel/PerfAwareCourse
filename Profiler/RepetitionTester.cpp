#include <string>
#include <iostream>
#include <intrin.h>
using std::string;
using std::cout;
using u64 = unsigned long long;


struct ReadFileInput {
    string filePath;
};

struct RepetitionTesterResult {
    u64 startTime;
    u64 endTime;
    u64 minTime;
    u64 maxTime;
    u64 firstTime;
    int count;
};

enum RepetitionTesterState {
    RepetitionTester_New,
    RepetitionTester_Running,
    RepetitionTester_Done,
    RepetitionTester_Error
};

class RepetitionTester {
public:
    RepetitionTesterState state;
    RepetitionTesterResult result;
    const int durationIncrement;
    u64 endTime;
    u64 cpuTimeFrequency;
    u64 repetitionStartTime;
    const int dataSize;
    RepetitionTester(const int durationSeconds, int dataSize, u64 cpuTimeFrequency)
        : durationIncrement(durationSeconds),
        dataSize(dataSize)
    {
        state = RepetitionTester_New;
        this->cpuTimeFrequency = cpuTimeFrequency;
        result.count = 0;
        result.minTime = -1; // overflows to max value
        result.maxTime = 0;
        result.firstTime = 0;
    }

    void Start() {
        if (state != RepetitionTester_New) {
            state = RepetitionTester_Error;
            return;
        }
        repetitionStartTime = result.startTime = __rdtsc();
        state = RepetitionTester_Running;
        endTime = result.startTime + durationIncrement * cpuTimeFrequency;
    }

    bool IsDone() {
        if (state != RepetitionTester_Running) {
            state = RepetitionTester_Error;
            cout << "IsDone fault";
            return true;
        }
        //cout << "IsDone " << endTime << ", " << __rdtsc()  << ": " << (endTime < __rdtsc()) << std::endl;
        return endTime < __rdtsc();
    }

    void SubmitRepetition() {
        if (state != RepetitionTester_Running) {
            state = RepetitionTester_Error;
            return;
        }

        u64 currentTime = __rdtsc();
        u64 repetitionDuration = currentTime - repetitionStartTime;
        if (repetitionDuration < result.minTime) {
            result.minTime = repetitionDuration;
            endTime = currentTime + durationIncrement * cpuTimeFrequency;
            cout << "found improvement" << repetitionDuration << " tsc, " << double(repetitionDuration) / cpuTimeFrequency << "ms" << std::endl
                << double(dataSize) / (1024 * 1024 * 1024) / (double(repetitionDuration) / cpuTimeFrequency) << "GB/s" << std::endl;
        }
        if (repetitionDuration > result.maxTime) {
            result.maxTime = repetitionDuration;
        }
        if (result.firstTime == 0) {
            result.firstTime = repetitionDuration;
        }
        result.count++;
        repetitionStartTime = __rdtsc();
    }

    void Finish() {
        if (state != RepetitionTester_Running) {
            state = RepetitionTester_Error;
            return;
        }
        result.endTime = __rdtsc();
        state = RepetitionTester_Done;
    }

    void Print() const {
        if (state == RepetitionTester_Error) {
            cout << "RepetitionTester faulted";
            return;
        }
        cout << "RepetitionTester done: " << std::endl
            << "Min time: " << result.minTime * 1000 / cpuTimeFrequency << " ms" << std::endl
            << "Max time: " << result.maxTime * 1000 / cpuTimeFrequency << " ms" << std::endl
            << "First time " << result.firstTime * 1000 / cpuTimeFrequency << " ms" << std::endl
            << "Count: " << result.count << std::endl
            << "In " << (result.endTime - result.startTime) / cpuTimeFrequency << "s";
    }
};