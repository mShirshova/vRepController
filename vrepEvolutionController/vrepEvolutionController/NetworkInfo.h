#ifndef NETWORK_INFO_H
#define	NETWORK_INFO_H

#include <vector>
#include <map>
using namespace std;
/*
	It is used for neuron initialization in some cases
*/
struct NeuronParams
{
	bool useARConnections;
	bool useNormalization;
	bool useRandomAddition;
	int transmissionFuncType;

	double minWeightValue;
	double maxWeightValue;

	double delayTime;//initial value of delay. It's equal for all neurons
	double threshold;
	double resThreshold;
	double logCoeff;
	double uRand;
	double aRand;
	double minPotential;
	double maxPotential;
	double frequency;

	double lBound;//for input neurons - left boung of the gausse conversion diapasone
	double rBound;//right boung of the gausse conversion diapasone
	double boundVal;

	
	int jNum;//num of joint corresponde this neuron (for input and output neurons)
	bool isIncreasing;
	
	int delayNum;
	int id;
};

struct ARModelInfo
{
	vector<bool>* restartedNeurons;
	vector<double>* aResult;
};

/*
	This stucture is need for unified initialization of all types of networks
	It contains all necessary initial parameters and all types of flags that 
	essentially determine the type of network. Such stucture is needed because it allows
	to avoid the repeated code for initializations for different types of networks which differ  
	only sligthly, for example - one or two parameters
*/
struct NetworkParams
{
	int type;//type of network
	int neuronType;
	vector<int> structure;//matrix of nums of input,interm and output neurons 
	map<int, vector<int>> connections;
	map<int, vector<int>> arConnections;//map of AR connections (1st - ID of start neuron, 2nd - ID of end neuron)
	map<int, int> typeMap; //map that determ the types of neurons (1st - neuron ID, 2nd - neuron type)

	NeuronParams neuronParams;//initial parameters of each neuron (all equal)
};

#endif