#ifndef UTIL_H
#define	UTIL_H

#include <string>
#include <algorithm>
#include <cstdarg>
#include <vector>
#include <Windows.h>

using namespace std;

namespace Util
{
	extern const double PI;

	string to_string(int val);
	void copyArray(double* fromArr, double* toArr, int len);
	double calc_Phi(double *dir);
	double calc_Teta(double *dir);
	bool checkChance(double probability);
	double generateRandomValue(double min = 0, double max = 1);
	int generateRandomIntegerValue(int min = 0, int max = 100);
	double generateGaussRandomValue(double average, double dispersion);
	int calcOrderOfNumer(double number);
	void setVectorToZero(vector<double>& vect);
	void createEvtName(const char* initName, char* nameBuf, int num);
	double  calcLogFuction(double x, double logCoeff);

	string guidToString(GUID guid);


	template< class T> 
	T selectRandomArgument(int num, T first,...)
	{
		T* pointer = &first;
		int rand = generateRandomIntegerValue(0, num);
		return *(pointer+rand);
	}

	template< class T > 
	T select( int num, ... )
	{
	   va_list args;
	   va_start( args, num );

	   typename vector<T> vect(0);
    
	   for ( int i = 0; i < num; i++ )
	   {
		  vect[i] = va_arg( args, T );
	   }
   
	   va_end( args );
   
	   return vect[generateRandomIntegerValue(0,num)];
	}
};

#endif
