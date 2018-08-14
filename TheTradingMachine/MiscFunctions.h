#pragma once
#include <vector>
#include <string>

void CsvReader(std::string filePath, std::vector<std::vector<std::string >>& csvVector);
std::vector<double>& RunningAverage(const std::vector<double> input, int windowSize);