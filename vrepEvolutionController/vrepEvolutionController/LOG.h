#ifndef LOG_H
#define	LOG_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

namespace LOG
{
	extern fstream out;
	extern ofstream out2;
	extern ifstream in;
	extern bool enableLogging;
	extern bool enableFileInput;

	void printMessage(string message, bool needChange = false);
	void printParam(string paramName, double param, bool needChange = false);

	void startFileInput(string s = "log.txt");
	void endFileInput();
	void writeMessageInFile(string message, bool needChange = false);
	void writeParamInFile(double param, bool needChange = false);
	void writeArrInFile(const vector<double>& arr, bool needChange = false);
};

#endif