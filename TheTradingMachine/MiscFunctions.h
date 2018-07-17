#pragma once
#include <vector>
#include <string>

using namespace std;

void CsvReader(string filePath, vector<vector<string>>& csvVector);


vector<double>& RunningAverage(const vector<double> input, int windowSize);