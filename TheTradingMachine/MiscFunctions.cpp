#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include "MiscFunctions.h"

using namespace std;

//
// Reads a Csv and turn it into a 2D vector of string
//
void CsvReader(string filePath, vector<vector<string>>& csvVector)
{

	fstream inputdata(filePath, ios::in);
	string inputline;
	
	for (auto& i : csvVector)
		i.clear();
	csvVector.clear();

	while (getline(inputdata, inputline))
	{
		//
		// Make a new line
		//
		csvVector.push_back(vector<string>());
		size_t lastRowIndex = csvVector.size() - 1;

		stringstream s(inputline);
		string token;
		//
		// Get each token separated by ,
		//
		while (getline(s, token, ','))
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
vector<double>& RunningAverage(const vector<double> input, int windowSize)
{
	static vector<double> runningAverageVector;
	runningAverageVector.clear();
	queue<double> currentWindow;

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