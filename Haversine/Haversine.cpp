#define PROFILER 1
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "../ApproximateCPUTimerFreq/ApproximateCPUTimerFreq.cpp"
#include "../Profiler/Profiler.cpp"
#include "../Profiler/RepetitionTester.cpp"
#include "Haversine.h"

using f64 = double;
using std::string;
using std::vector;
using hrclock = std::chrono::high_resolution_clock;
using namespace std::chrono_literals;

u64 cpuTimeFreq;

static f64 Square(f64 A)
{
    f64 Result = (A * A);
    return Result;
}

static f64 RadiansFromDegrees(f64 Degrees)
{
    f64 Result = 0.01745329251994329577 * Degrees;
    return Result;
}

// NOTE(casey): EarthRadius is generally expected to be 6372.8
static f64 ReferenceHaversine(f64 X0, f64 Y0, f64 X1, f64 Y1, f64 EarthRadius)
{
    /* NOTE(casey): This is not meant to be a "good" way to calculate the Haversine distance.
       Instead, it attempts to follow, as closely as possible, the formula used in the real-world
       question on which these homework exercises are loosely based.
    */

    f64 lat1 = Y0;
    f64 lat2 = Y1;
    f64 lon1 = X0;
    f64 lon2 = X1;

    f64 dLat = RadiansFromDegrees(lat2 - lat1);
    f64 dLon = RadiansFromDegrees(lon2 - lon1);
    lat1 = RadiansFromDegrees(lat1);
    lat2 = RadiansFromDegrees(lat2);

    f64 a = Square(sin(dLat / 2.0)) + cos(lat1) * cos(lat2) * Square(sin(dLon / 2));
    f64 c = 2.0 * asin(sqrt(a));

    f64 Result = EarthRadius * c;

    return Result;
}

void readWhitespace(const string& input, int& i)
{
    while (i < input.size() && (input[i] == ' ' || input[i] == '\n' || input[i] == '\t' || input[i] == '\r')) {
		++i;
	}
}
// reads name surrounded with double quotes
void readName(const string& input, int& i, const string &expectedName)
{
    if (input[i] != '\"') {
		std::cout << "Invalid input file, expected opening \"" << std::endl;
		return;
	}
	++i;
    string name;
    while (i < input.size() && input[i] != '\"') {
		name += input[i];
		++i;
	}
    if (name != expectedName) {
		std::cout << "Invalid input file, expected " << expectedName << std::endl;
        return;
    }
    if (input[i] != '\"') {
		std::cout << "Invalid input file, expected closing \"" << std::endl;
		return;
	}
	++i;
}

void readChar(const string& input, int& i, char expectedChar)
{
    if (input[i] != expectedChar) {
		std::cout << "Invalid input file, expected " << expectedChar << std::endl;
		return;
	}
	++i;
}

f64 readNumber(const string& input, int& i)
{
	string number;
    while (i < input.size() && (input[i] == '-' || input[i] == '.' || (input[i] >= '0' && input[i] <= '9'))) {
		number += input[i];
		++i;
	}
	return std::stod(number);
}

struct pair {
	f64 values[4];
    //f64 y0;
    //f64 x1;
    //f64 y1;
};

f64 readNamedNumber(const string& input, int& i, const string& name)
{
	readName(input, i, name);
	readWhitespace(input, i);
	readChar(input, i, ':');
    readWhitespace(input, i);
	return readNumber(input, i);
}

string names[4] = {"X0", "Y0", "X1", "Y1"};
pair readPair(const string& input, int& i) {
    pair p;
    readChar(input, i, '{');
    readWhitespace(input, i);
    for (int j = 0; j < 4; ++j) {
        p.values[j] = readNamedNumber(input, i, names[j]);
        readWhitespace(input, i);
        if (j < 3) {
			readChar(input, i, ',');
		}
		readWhitespace(input, i);
    }
    readChar(input, i, '}');
    //std::cout << "X0: " << p.values[0] << " Y0: " << p.values[1] << " X1: " << p.values[2] << " Y1: " << p.values[3] << std::endl;
    return p;
}

string ReadFile(std::string& fileName)
{
    TimeFunction();
    string input;
    string line;
	FILE* file = fopen(fileName.c_str(), "rb");
#if _WIN32
    struct __stat64 Stat;
	_stat64(fileName.c_str(), &Stat);
#else
	struct stat Stat;
	stat(fileName.c_str(), &Stat);
#endif
    char* buffer = (char*)malloc(Stat.st_size);
    while (true) {
        for(int i = 0; i < 2; ++i) {
            if (i == 1) {
                cout << "start malloc run" << std::endl;
            }
            else {
                cout << "start reuse run" << std::endl;
            }

            Sleep(500);
	        TimeBandwidth("fread", Stat.st_size);
            RepetitionTester tester(10, Stat.st_size, cpuTimeFreq);
            tester.Start();
            while (!tester.IsDone()) {
                if (i == 1) {
                    buffer = (char*)malloc(Stat.st_size);
                }
	            if (fread(buffer, Stat.st_size, 1, file) != 1) {
		            std::cout << "Error reading file" << std::endl;
                    delete[] buffer;
		            return "";
	            }
                rewind(file);
                tester.SubmitRepetition();
            }
            tester.Finish();
            tester.Print();

	        input = string(buffer, Stat.st_size);
        }
    }
    
    return input;
}

void RecursiveFunction(int depth)
{
	TimeFunction();
    if (depth > 0) {
        std::this_thread::sleep_for(10ms);
        RecursiveFunction(depth - 1);
	}
}

int main(int argc, char* argv[])
{
    string fileName = argv[1];
    vector<f64> answers;
    if (argc == 3) {
        std::ifstream answerfile(argv[2]);
        string line;
        while (std::getline(answerfile, line))
        {
            f64 value = std::stod(line);
            answers.push_back(value);
        }
    }

    cout << "read answers" << std::endl; 
    cpuTimeFreq = getCpuTimerFreq(1000000);
    Profiler();
    Profiler::_cpuTimeFreq = cpuTimeFreq;
    string input = ReadFile(fileName);
    f64 sum = 0;
    {
        TimeBlock("Parse json");
        int i = 0;
        if (input[0] != '{') {
            std::cout << "Invalid input file" << std::endl;
            return 1;
        }
        ++i;
        readWhitespace(input, i);
        readName(input, i, "pairs");
        readWhitespace(input, i);
        readChar(input, i, ':');
        readWhitespace(input, i);
        readChar(input, i, '[');
        readWhitespace(input, i);
        vector<pair> pairs;
        pair p = readPair(input, i);
        pairs.push_back(p);
        readWhitespace(input, i);
        while (input[i] == ',') {
            readChar(input, i, ',');
            readWhitespace(input, i);
            pairs.push_back(readPair(input, i));
            readWhitespace(input, i);
        }
        readWhitespace(input, i);
        readChar(input, i, ']');
        readWhitespace(input, i);
        readChar(input, i, '}');
        //std::cout << "Number of pairs: " << pairs.size() << std::endl;

        f64 earthRadius = 6372.8;
        {
            for (int i = 0; i < pairs.size(); ++i) {
                TimeBandwidth("Calculate Haversine", 32);
                f64 result = ReferenceHaversine(pairs[i].values[0], pairs[i].values[1], pairs[i].values[2], pairs[i].values[3], earthRadius);
                sum += result;
                //if (answers.size() > i) {
                //    if (result != answers[i]) {
                //        std::cout << "i : " << i << "Expected: " << answers[i] << " Got : " << result << std::endl;
                //    }
                //}
            }
        }
    }

    //RecursiveFunction(3); // test recursive profiling

    EndAndPrintResults();
}
