using std::string;
using std::cout;
using u64 = unsigned long long;


struct ReadFileInput {
    string filePath;
};

struct RepetitionTesterResult {
    u64 duration;
    u64 pageFaults;
};
struct RepetitionTesterRunResult {
    u64 startTime;
    u64 endTime;
    RepetitionTesterResult minDurationResult;
    RepetitionTesterResult maxDurationResult;
    RepetitionTesterResult firstResult;
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
    RepetitionTesterRunResult result;
    const int durationIncrement;
    u64 endTime;
    u64 cpuTimeFrequency;
    u64 repetitionStartTime;
    u64 repetitionStartPageFaults;
    const int dataSize;
    HANDLE hProc;
    u64 pageFaultsStart;
    RepetitionTester(const int durationSeconds, int dataSize, u64 cpuTimeFrequency)
        : durationIncrement(durationSeconds),
        dataSize(dataSize)
    {
        state = RepetitionTester_New;
        this->cpuTimeFrequency = cpuTimeFrequency;
        result.count = 0;
        result.minDurationResult.duration = -1; // overflows to max value
        result.maxDurationResult.duration = 0;
        result.firstResult.duration = 0;
        hProc = GetCurrentProcess();
        repetitionStartPageFaults = pageFaultsStart = GetPageFaultCountForProcess();
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
        u64 pageFaultsEnd = GetPageFaultCountForProcess();
        u64 pageFaults = pageFaultsEnd - repetitionStartPageFaults;
        if (repetitionDuration < result.minDurationResult.duration) {
            result.minDurationResult.duration = repetitionDuration;
            result.minDurationResult.pageFaults = pageFaults;
            endTime = currentTime + durationIncrement * cpuTimeFrequency;
            cout << "found improvement: " << repetitionDuration << " tsc, " << double(repetitionDuration) / cpuTimeFrequency << "ms, PF: " << pageFaults << ", "
                << double(dataSize) / (1024 * 1024 * 1024) / (double(repetitionDuration) / cpuTimeFrequency) << "GB/s" << std::endl;
        }
        if (repetitionDuration > result.maxDurationResult.duration) {
            result.maxDurationResult.duration = repetitionDuration;
            result.maxDurationResult.pageFaults = pageFaults;
        }
        if (result.firstResult.duration == 0) {
            result.firstResult.duration = repetitionDuration;
            result.firstResult.pageFaults = pageFaults;
        }
        result.count++;
        repetitionStartTime = __rdtsc();
        repetitionStartPageFaults = pageFaultsEnd;
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
            << "Min time: " << result.minDurationResult.duration * 1000 / cpuTimeFrequency << " ms, PF: " << result.minDurationResult.pageFaults << std::endl
            << "Max time: " << result.maxDurationResult.duration * 1000 / cpuTimeFrequency << " ms, PF: " << result.maxDurationResult.pageFaults << std::endl
            << "First time " << result.firstResult.duration * 1000 / cpuTimeFrequency << " ms, PF: " << result.firstResult.pageFaults << std::endl
            << "Count: " << result.count << std::endl
            << "In " << (result.endTime - result.startTime) / cpuTimeFrequency << "s" << std::endl;
    }

    u64 GetPageFaultCountForProcess() {
        PROCESS_MEMORY_COUNTERS pmc{};
        pmc.cb = sizeof(pmc);
        if (!GetProcessMemoryInfo(hProc, &pmc, sizeof(pmc))) {
            return 0; // or throw/GetLastError handling
        }
        return static_cast<u64>(pmc.PageFaultCount);
    }
};