#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "../ApproximateCPUTimerFreq/ApproximateCPUTimerFreq.cpp"

using f64 = double;
using std::string;
using std::vector;
using hrclock = std::chrono::high_resolution_clock;

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

int main(int argc, char* argv[])
{
    string fileName = argv[1];
    std::ifstream infile(fileName);
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
    string line;
    char buffer[256];  // Adjust the size of buffer as needed

    string input;

    auto cpuFreq = getCpuTimerFreq(1000000);
    hrclock::time_point start = std::chrono::high_resolution_clock::now();
    auto start2 = __rdtsc();

    while (!infile.eof()) {
        infile.read(buffer, sizeof(buffer) - 1);  // Read x characters into buffer
        buffer[infile.gcount()] = '\0';  // Null-terminate the string
        input += buffer;
    }

    auto time1 = __rdtsc();

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

    auto time2 = __rdtsc();

    f64 sum = 0;
    f64 earthRadius = 6372.8;
    for (int i = 0; i < pairs.size(); ++i) {
        f64 result = ReferenceHaversine(pairs[i].values[0], pairs[i].values[1], pairs[i].values[2], pairs[i].values[3], earthRadius);
        sum += result;
        //if (answers.size() > i) {
        //    if (result != answers[i]) {
        //        std::cout << "i : " << i << "Expected: " << answers[i] << " Got : " << result << std::endl;
        //    }
        //}
    }

    hrclock::time_point end = std::chrono::high_resolution_clock::now();
    auto end2 = __rdtsc();
    // get time taken in milliseconds
    auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout.precision(std::numeric_limits<double>::max_digits10);
    std::cout << "Sum: " << sum << std::endl;
    std::cout << "Chrono time taken: " << time_taken << " ms" << std::endl;
    std::cout << "RDTSC time taken: " << double((end2 - start2) * 1000) / cpuFreq << " ms" << std::endl;
    std::cout << "RDTSC time taken for reading: " << double((time1 - start2) * 1000) / cpuFreq << " ms" << std::endl;
    std::cout << "RDTSC time taken for parsing: " << double((time2 - time1) * 1000) / cpuFreq << " ms" << std::endl;
    std::cout << "RDTSC time taken for calculating: " << double((end2 - time2) * 1000) / cpuFreq << " ms" << std::endl;
}
