#ifndef PARAMETERS_H
#define	PARAMETERS_H 

#include <vector>
#include "Constants.h"
#include "LOG.h"

namespace Parameters
{
	extern int programVersion;

	extern int JOINT_NUM;//22;
	extern int OUTPUT_NUM;
	extern int ALL_SPACE_DIM;

	//----Common Evolution params------
	extern int GENERATION_NUM;
	extern int DESCENDANT_NUM;
	extern int ONE_AGENT_LIFE_TIME;
	extern int evolutionAlgorithmType; 
	extern int descendantCreationType;
	extern int typeOfFitness;
	extern int typeOfControl;
	extern int networkType;

	//---agent params
	extern double ANGLE_STEP_SIZE;
	extern int dimentionOfInput;
	//---process params
	extern int NAO_NUM_IN_SCENE;
	extern int PROCCESSES_NUM;
	extern int SAVE_GENERATION_NUM;
	extern int SEED;
	
	//not written in files params
	extern int LAUNCH_NUM;
	extern bool autoloadParams;
	extern bool autosaveNetworks;
	extern bool autosaveParams;
	extern bool autosaveStatistic;
	extern bool showJointsData;

	//----Common networks params----
	extern int neuronType;
	extern int inputTransmissionFuncType;
	extern int intermTransmissionFuncType;
	extern int outTransmissionFuncType;
	extern double LOG_FUNC_LINEAR_MAX;
	extern double LOG_FUNC_K_COEFF;
	extern bool useNormalization;
	extern bool useRandomAddition;
	
	extern double MAX_RANDOM_WEIGHT;//init random weight
	extern double MAX_POTENTIAL;
	extern double MIN_POTENTIAL;
	extern double ACTIVATION_THR;
	extern double GAUSSE_BOUND_VALUE;
	extern double A_RAND;
	extern double U_RAND;
	extern double RESULT_THR;
	extern int NEURON_DELAY_TIME;
	extern double OUT_MILTIPLY_COEFF;
	extern double SIN_FREQUENCY;

	extern bool useReadyNetwork;
	extern int readyNetworkType;
	//----Feedforward (and Jordan network params) networks params---
	extern std::string neuronsMatrix;
	//----Fully network params
	extern int FULLY_NETW_NEURON_NUM;
	//----Jordan network params
	extern int INPUT_DELAY_NUM;
	extern int BACK_FROM_OUTPUT_DELAY_NUM;

	//---Common evolution networks params
	extern double P_MUTATION_WEIGHT;
	extern double P_MUTATION_FREQUENCY;
	extern double P_MUTATION_THRESHOLD;
	extern double S_WEGHT;
	extern double S_FREQUENCY;
	extern double S_THRESH;
	//---MB evolution networks params (for reccurent, time delay and ar network)
	extern int START_INTERM_NEURON_NUM;
	extern double P_DUPLICATION;
	extern double P_DELETION;
	extern double P_CONNECTION_ADD;
	extern double P_CONNECTION_REMOVE;
	extern double P_MUTATION_RES_THRESHOLD;
	extern double P_MUTATION_TIME;
	extern double S_TIME_DELAY;
	extern double S_RES_THRESH;
	extern bool dublicateInputNeurons;
	extern bool dublicateOutputNeurons;
	extern bool deleteInputNeurons;
	extern bool deleteOutputNeurons;

	extern double PORTION_OF_CHILDS;
	extern bool mutateStablePartOfPopulation;
	extern bool selectFromStable;
	extern int selectionType;

	//version 1
	extern bool addInhibitoryConnectionsToOutput;
	extern bool addBackConnectionToOutput;

	void setDimentionOfInput(int dim);
	void setDefaultNeuronMatrix();
	void setDefaultParameters();

	void printParams();
	void printTransFunc(int type);
}

#endif