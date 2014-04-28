#include "LOG.h"

fstream LOG::out;
ofstream LOG::out2;
ifstream LOG::in;
bool LOG::enableLogging = true;
bool LOG::enableFileInput = true;

void LOG::printMessage(string message, bool needChange)
{
	bool enable = needChange ? !enableLogging : enableLogging; 
	if(enable && message != "")
	{
		cout <<"\n"<<message<<"\n";
	}
}

void LOG::printParam(string paramName, double param, bool needChange)
{
	bool enable = needChange ? !enableLogging : enableLogging; 
	if(enable && paramName != "")
	{
		cout <<"\n"<<paramName<<" = " << param << "\n";
	}
}

void LOG::startFileInput(string s)
{
    out.open(s,ios::app);
}

void LOG::endFileInput()
{
	out.close();
}

void LOG::writeMessageInFile(string message, bool needChange)
{
	bool enable = needChange ? !enableFileInput : enableFileInput; 
	if(enable && message != "")
	{
		out << message << endl; // Вывели в файл фразу Hello Worl
	}
}

void LOG::writeParamInFile(double param, bool needChange)
{
	bool enable = needChange ? !enableFileInput : enableFileInput; 
	if(enable)
	{
		out << param << endl;
	}
}

void LOG::writeArrInFile(const vector<double>& arr, bool needChange)
{
	bool enable = needChange ? !enableFileInput : enableFileInput; 
	if(enable)
	{
		int size = arr.size();
		for(int i=0; i<size; i++)
			out << arr[i] <<',';
		out << endl;
	}
}