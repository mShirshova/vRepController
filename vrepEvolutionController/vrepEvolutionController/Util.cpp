#include "Util.h"
#include <stdio.h>

const double Util::PI = 3.14;

void Util::setVectorToZero(vector<double>& vect){
	vector<double>::iterator iter;
	for(iter = vect.begin(); iter != vect.end(); iter++)
	{
		(*iter) = 0;
	}
}

void Util::copyArray(double* fromArr,double* toArr, int len){
	for(int i=0; i<len;i++)
	{
		toArr[i] = fromArr[i];
	}
}

string Util::to_string(int val) {
	 char buff[32];
	 sprintf(buff,"%d",val);
	 return string(buff);
}

int Util::calcOrderOfNumer(double number)
{
	int order = 0;

	while(div((int)number, pow(10.0,order)).quot > 10){
		order++;
	}
	return order;
}

//probability 0..1
bool Util::checkChance(double probability)
{
	double v = (double)(rand()%10000)/10000;
	return v < probability;
}

double Util::generateRandomValue(double min, double max)
{
	double v =  (double)(rand()%1000)/1000;
	return min + (max - min)*v;
}

int Util::generateRandomIntegerValue(int min, int max)
{
	double v = (double)(rand()%100)/100;
	return min + floor((max - min)*v);
}


double  Util::generateGaussRandomValue(double average, double dispersion)
{
	//generate from average - 2*sqrt(dispersion) to average + sqrt(dispersion)
	double value = 0;
	double prob;
	int iterations = 0;

	while(iterations < 100)
	{
		iterations++;
		value = generateRandomValue(average - 2*sqrt(dispersion), average + 2*sqrt(dispersion));
		prob = exp(-(value - average)/(2*dispersion));
		if(checkChance(prob))
		{
			return value;
		}
	}
	return value;
}

double Util::calcLogFuction(double x, double logCoeff)
{
	return 1/(1+exp(-x));
}

string Util::guidToString(GUID guid)
{
	char output[40];
	_snprintf(output, 40, "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	return string(output);
}

void Util::createEvtName(const char* initName, char* nameBuf, int num)
{
	int size=0;
	while(initName[size] != 0)
	{
		nameBuf[size] = initName[size];
		size++;
	}
	nameBuf[size]='_';
	size++;

	char buff[32];
	sprintf(buff,"%d",num);

	int i=0;
	while(buff[i] != 0)
	{
		nameBuf[size] = buff[i];
		i++;
		size++;
	}
	nameBuf[size]=0;
}

