#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include "MiscFunctions.h"

//
// Reads a Csv and turn it into a 2D vector of string
//
void CsvReader(std::string filePath, std::vector<std::vector<std::string>>& csvVector)
{

	std::fstream inputdata(filePath, std::ios::in);
	std::string inputline;
	
	for (auto& i : csvVector)
		i.clear();
	csvVector.clear();

	while (std::getline(inputdata, inputline))
	{
		//
		// Make a new line
		//
		csvVector.push_back(std::vector<std::string>());
		size_t lastRowIndex = csvVector.size() - 1;

		std::stringstream s(inputline);
		std::string token;
		//
		// Get each token separated by ,
		//
		while (std::getline(s, token, ','))
		{
			csvVector[lastRowIndex].push_back(token);
		}
	}
}

//
// Uses a queue to calculate the running average. Each time a new number comes
// in, we subtract the front of the queue from the sum and push in the new 
// as well as add the new number to the sum. 
//
std::vector<double>& RunningAverage(const std::vector<double> input, int windowSize)
{
	static std::vector<double> runningAverageVector;
	runningAverageVector.clear();
	std::queue<double> currentWindow;

	size_t i = 0;
	double sum = 0;
	double val;
	//
	// left edge
	//
	while (currentWindow.size() < windowSize && i < input.size())
	{
		val = input[i++];
		currentWindow.push(val);
		sum += val;
		runningAverageVector.push_back(sum / currentWindow.size());
	}
	
	//
	// Body to end.
	//
	for (; i < input.size(); i++)
	{
		val = input[i];
		sum = sum + val - currentWindow.front();
		currentWindow.pop();
		currentWindow.push(val);
		runningAverageVector.push_back(sum / windowSize);
	}

	return runningAverageVector;
}