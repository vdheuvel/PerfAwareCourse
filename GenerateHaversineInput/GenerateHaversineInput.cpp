/* ========================================================================

   (C) Copyright 2023 by Molly Rocket, Inc., All Rights Reserved.

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.

   Please see https://computerenhance.com for more information

   ======================================================================== */

   /* ========================================================================
      LISTING 65
      ======================================================================== */
#include <iostream>
#include <fstream>
#include <string>
using f64 = double;
using std::string;

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

int rand2k() {
    return rand() % 2000;
}

f64 randF64()
{
    return (rand() % 2000 - 1000) / 1000.0;
}

int main(int argc, char* argv[])
{
    bool uniform = argv[1] == "uniform";
    // parse argv[1] into int seed using std::stoi
    int seed = std::stoi(argv[2]);
    int n = std::stoi(argv[3]);
    auto earthRadius = 6372.8;
    srand(seed);

    string jsonInput = "{\"pairs\":[";
    string answers = "";
    int multiplier[4] = { 180, 90, 180, 90 };
    f64 sum = 0;
    int centers[4] = { rand2k(), rand2k() ,rand2k() ,rand2k() };
    for (int i = 0; i < n; i++)
    {
        f64 values[4];
        for (int j = 0; j < 4; j++)
        {
            f64 value = uniform ? randF64() * multiplier[j] :
                f64((centers[j] + rand2k()/4) % 2000 - 1000) / 1000.f * multiplier[j];
			values[j] = value;
		}
		f64 result = ReferenceHaversine(values[0], values[1], values[2], values[3], earthRadius);
        sum += result;
		answers += std::to_string(result) + "\n";
        jsonInput += "    {\"X0\":" + std::to_string(values[0]) + ",\"Y0\":" + std::to_string(values[1]) + ",\"X1\":" + std::to_string(values[2]) + ",\"Y1\":" + std::to_string(values[3]) + "}";
        if (i < n - 1)
        {
			jsonInput += ",";
		}
        jsonInput += "\n";
    }
    jsonInput += "]}";

    // write jsonInput to file
    std::ofstream jsonFile("input.json");
    jsonFile << jsonInput;
    jsonFile.close();

    // write answers to file
    std::ofstream answerFile("answers.txt");
    answerFile << answers;
    answerFile.close();

    // write sum to file
    std::ofstream sumFile("sum.txt");
    sumFile << std::to_string(sum);
    sumFile.close();

    return 0;
}
