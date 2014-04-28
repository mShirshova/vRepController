#include "Parameters.h"
#include "Util.h"

int Parameters::programVersion;

int Parameters::JOINT_NUM;//22;
int Parameters::OUTPUT_NUM;
int Parameters::ALL_SPACE_DIM;

//----Common Evolution params------
int Parameters::GENERATION_NUM;
int Parameters::DESCENDANT_NUM;
int Parameters::ONE_AGENT_LIFE_TIME;
int Parameters::evolutionAlgorithmType; 
int Parameters::descendantCreationType;
int Parameters::typeOfFitness;
int Parameters::typeOfControl;
int Parameters::networkType;

//---agent params
double Parameters::ANGLE_STEP_SIZE;
int Parameters::dimentionOfInput;
//---process params
int Parameters::NAO_NUM_IN_SCENE;
int Parameters::PROCCESSES_NUM;
int Parameters::SAVE_GENERATION_NUM;
int Parameters::SEED;

int Parameters::LAUNCH_NUM;
bool Parameters::autoloadParams;
bool Parameters::autosaveNetworks;
bool Parameters::autosaveParams;
bool Parameters::autosaveStatistic;
bool Parameters::showJointsData;

//networks params
int Parameters::readyNetworkType;
int Parameters::neuronType;
int Parameters::inputTransmissionFuncType;
int Parameters::intermTransmissionFuncType;
int Parameters::outTransmissionFuncType;
double Parameters::LOG_FUNC_LINEAR_MAX;
double Parameters::LOG_FUNC_K_COEFF;
bool Parameters::useNormalization;
bool Parameters::useRandomAddition;
bool Parameters::useReadyNetwork;
double Parameters::MAX_RANDOM_WEIGHT;//init random weight
double Parameters::MAX_POTENTIAL;
double Parameters::MIN_POTENTIAL;
double Parameters::ACTIVATION_THR;
double Parameters::GAUSSE_BOUND_VALUE;
double Parameters::A_RAND;
double Parameters::U_RAND;
double Parameters::RESULT_THR;
int Parameters::NEURON_DELAY_TIME;
double Parameters::OUT_MILTIPLY_COEFF;
double Parameters::SIN_FREQUENCY;

//----Feedforward networks params---
std::string Parameters::neuronsMatrix;
//----Fully network params
int Parameters::FULLY_NETW_NEURON_NUM;
//----Jordan network params
int Parameters::INPUT_DELAY_NUM;
int Parameters::BACK_FROM_OUTPUT_DELAY_NUM;

//---Common evolution networks params
double Parameters::P_MUTATION_WEIGHT;
double Parameters::P_MUTATION_THRESHOLD;
double Parameters::S_WEGHT;
double Parameters::S_THRESH;
//---MB evolution networks params (for reccurent, time delay and ar network)
int Parameters::START_INTERM_NEURON_NUM;
double Parameters::P_DUPLICATION;
double Parameters::P_DELETION;
double Parameters::P_CONNECTION_ADD;
double Parameters::P_CONNECTION_REMOVE;
double Parameters::P_MUTATION_RES_THRESHOLD;
double Parameters::P_MUTATION_TIME;
double Parameters::S_TIME_DELAY;
double Parameters::S_RES_THRESH;
double Parameters::P_MUTATION_FREQUENCY;
double Parameters::S_FREQUENCY;
bool Parameters::dublicateInputNeurons;
bool Parameters::dublicateOutputNeurons;
bool Parameters::deleteInputNeurons;
bool Parameters::deleteOutputNeurons;

double Parameters::PORTION_OF_CHILDS;
bool Parameters::mutateStablePartOfPopulation;
bool Parameters::selectFromStable;
int Parameters::selectionType;

//program version = 1
bool Parameters::addInhibitoryConnectionsToOutput;
bool Parameters::addBackConnectionToOutput;

//must be called after setting the typeOfControl param
void Parameters::setDimentionOfInput(int dim)
{
	dimentionOfInput = dim;

	int rot_state_num;
	int gyro_rate_state_num;

	if(dimentionOfInput == Constants::FULL_DIM)
		JOINT_NUM = 18;//22;
	else if(dimentionOfInput == Constants::MIDDLE_DIM)
		JOINT_NUM = 12;//22;
	else 
		JOINT_NUM = 18;

	if(typeOfControl == Constants::ONE_NEURON_CONTROL || typeOfControl == Constants::FORCE_COTROL)
		OUTPUT_NUM = JOINT_NUM;
	else
		OUTPUT_NUM = 2*JOINT_NUM;

	rot_state_num = 3;//num of rotational state variables
	gyro_rate_state_num = 2;
	ALL_SPACE_DIM = JOINT_NUM;// + rot_state_num + gyro_rate_state_num;
}

void Parameters::setDefaultNeuronMatrix()
{
	neuronsMatrix = Util::to_string(ALL_SPACE_DIM)+",3,4,"+Util::to_string(OUTPUT_NUM);
}


void Parameters::setDefaultParameters()
{
	START_INTERM_NEURON_NUM = 26;
	
	LOG_FUNC_LINEAR_MAX = 1.5;
	LOG_FUNC_K_COEFF = 1;
	ANGLE_STEP_SIZE = 0.03;
	ACTIVATION_THR = 0.5;
	RESULT_THR = 0.6;
	MAX_RANDOM_WEIGHT = 1;
	MAX_POTENTIAL = 1;
	MIN_POTENTIAL = -1;
	NEURON_DELAY_TIME = 32*5;
	GAUSSE_BOUND_VALUE = 0.1;
	OUT_MILTIPLY_COEFF = 1;
	SIN_FREQUENCY = 1;

	U_RAND = 0.005;
	A_RAND = 0.005;

	P_DUPLICATION = 0.001;
	P_DELETION = 0.001;
	P_CONNECTION_ADD = 0.001;
	P_CONNECTION_REMOVE = 0.001;
	P_MUTATION_WEIGHT = 0.1;
	P_MUTATION_THRESHOLD = 0.1;
	P_MUTATION_RES_THRESHOLD = 0.1;
	P_MUTATION_TIME = 0.1;
	P_MUTATION_FREQUENCY = 0.1;

	S_WEGHT = MAX_RANDOM_WEIGHT/20;
	S_TIME_DELAY = NEURON_DELAY_TIME/20;
	S_THRESH = 0.01;
	S_RES_THRESH = 0.01;
	S_FREQUENCY = 0.01;

	GENERATION_NUM = 40;
	DESCENDANT_NUM = 16;
	ONE_AGENT_LIFE_TIME = 20000;

	NAO_NUM_IN_SCENE = 2;
	PROCCESSES_NUM = 4;
	SAVE_GENERATION_NUM = 1;
	SEED = 5000;
	showJointsData = true;

	LAUNCH_NUM = 1;
	autoloadParams = false;
	autosaveNetworks = true;
	autosaveStatistic = true;
	autosaveParams = true;

	networkType = Constants::RECCURENT_NETWORK_TYPE;
	neuronType = Constants::SIMPLE_NEURON;
	evolutionAlgorithmType = Constants::GENETIC_ALG;
	descendantCreationType = Constants::CREATE_DESC_MB;
	typeOfFitness = Constants::DISTANCE_FTNESS;
	typeOfControl = Constants::ONE_NEURON_CONTROL;

	inputTransmissionFuncType = Constants::SIN_FUNCTION;
	intermTransmissionFuncType =  Constants::SIN_FUNCTION;
	outTransmissionFuncType = Constants::SIN_FUNCTION;

	useNormalization = false;
	useRandomAddition = false;

	dublicateInputNeurons = false;
	dublicateOutputNeurons = false;
	deleteInputNeurons = false;
	deleteOutputNeurons = false;

	PORTION_OF_CHILDS = 0.5;
	mutateStablePartOfPopulation = false;
	selectFromStable = true;
	selectionType = Constants::RANDOM_SEL_TYPE;

	setDimentionOfInput(Constants::FULL_DIM);
	setDefaultNeuronMatrix();

	useReadyNetwork = false;
	readyNetworkType = Constants::FEEDFORWARD_NETWORK_TYPE;

	//fully connected network
	FULLY_NETW_NEURON_NUM = JOINT_NUM + 5;

	//jordan network
	INPUT_DELAY_NUM = 2;
	BACK_FROM_OUTPUT_DELAY_NUM = 2;

	//program version = 1
	addInhibitoryConnectionsToOutput = false;
	addBackConnectionToOutput = false;
}

void Parameters::printParams()
{
	LOG::startFileInput("log.txt");

	LOG::writeMessageInFile("generationNum:");
	LOG::writeParamInFile(Parameters::GENERATION_NUM);
	LOG::writeMessageInFile("descendantNum:");
	LOG::writeParamInFile(Parameters::DESCENDANT_NUM);
	LOG::writeMessageInFile("agentLifeTime:");
	LOG::writeParamInFile(Parameters::ONE_AGENT_LIFE_TIME);

	if(Parameters::evolutionAlgorithmType == Constants::GENETIC_ALG)
		LOG::writeMessageInFile("evolutionAlgorithmType: GENETIC_ALG");

	if(Parameters::descendantCreationType == Constants::CREATE_DESC_WITH_CROSSOVER)
		LOG::writeMessageInFile("descendantCreationType: CREATE_DESC_WITH_CROSSOWER");
	else if(Parameters::descendantCreationType == Constants::CREATE_DESC_MB)
		LOG::writeMessageInFile("descendantCreationType: CREATE_DESC_MB");
	else if(Parameters::descendantCreationType == Constants::CREATE_DESC_MB_2)
		LOG::writeMessageInFile("descendantCreationType: CREATE_DESC_MB_2");

	if(Parameters::typeOfFitness == Constants::ACTIVITY_FTNESS)
		LOG::writeMessageInFile("typeOfFitness: ACTIVITY_FTNESS");
	else if(Parameters::typeOfFitness == Constants::DISTANCE_FTNESS)
		LOG::writeMessageInFile("typeOfFitness: DISTANCE_FTNESS");
	else if(Parameters::typeOfFitness == Constants::MIX_FTNESS)
		LOG::writeMessageInFile("typeOfFitness: MIX_FITNESS");

	if(Parameters::typeOfControl == Constants::COMPETITION_CONTROL)
		LOG::writeMessageInFile("typeOfControl: COMPETITION_CONTROL");
	else if(Parameters::typeOfControl == Constants::DELTA_COMPETTION_CONTROL)
		LOG::writeMessageInFile("typeOfControl: DELTA_COMPETTION_CONTROL");
	else if(Parameters::typeOfControl == Constants::PROP_COMPETTION_CONTROL)
		LOG::writeMessageInFile("typeOfControl: PROP_COMPETTION_CONTROL");
	else if(Parameters::typeOfControl == Constants::ONE_NEURON_CONTROL)
		LOG::writeMessageInFile("typeOfControl: ONE_NEURON_CONTROL");
	else if(Parameters::typeOfControl == Constants::MULTIPLE_COMPETTITION_CONTROL)
		LOG::writeMessageInFile("typeOfControl: MULTIPLE_COMPETTITION_CONTROL");
	else if(Parameters::typeOfControl == Constants::FORCE_COTROL)
		LOG::writeMessageInFile("typeOfControl: FORCE_COTROL");
	
	LOG::writeMessageInFile("ANGLE_STEP_SIZE:");
	LOG::writeParamInFile(Parameters::ANGLE_STEP_SIZE);
	LOG::writeMessageInFile("dimention of input:");
	LOG::writeParamInFile(Parameters::JOINT_NUM);
	LOG::writeMessageInFile("nao num in scene:");
	LOG::writeParamInFile(Parameters::NAO_NUM_IN_SCENE);
	LOG::writeMessageInFile("robot num in one session:");
	LOG::writeParamInFile(Parameters::PROCCESSES_NUM);
	LOG::writeMessageInFile("seed:");
	LOG::writeParamInFile(Parameters::SEED);

	if(Parameters::neuronType == Constants::SIMPLE_NEURON)
		LOG::writeMessageInFile("neuronType: SIMPLE_NEURON");
	else if(Parameters::neuronType == Constants::TIME_DELAY_NEURON)
		LOG::writeMessageInFile("neuronType: TIME_DELAY_NEURON");
	else if(Parameters::neuronType == Constants::LEAKY_INTEGRATOR_NEURONS)
		LOG::writeMessageInFile("neuronType: LEAKY_INTEGRATOR_NEURONS");
	else if(Parameters::neuronType == Constants::AR_NEURON)
		LOG::writeMessageInFile("neuronType: AR_NEURON");
	
	LOG::writeMessageInFile("inputTransmissionFuncType:");
	printTransFunc(inputTransmissionFuncType);
	LOG::writeMessageInFile("intermTransmissionFuncType:");
	printTransFunc(intermTransmissionFuncType);
	LOG::writeMessageInFile("outTransmissionFuncType:");
	printTransFunc(outTransmissionFuncType);

	LOG::writeMessageInFile("LOG_FUNC_LINEAR_MAX");
	LOG::writeParamInFile(Parameters::LOG_FUNC_LINEAR_MAX);
	LOG::writeMessageInFile("LOG_FUNC_K_COEFF");
	LOG::writeParamInFile(Parameters::LOG_FUNC_K_COEFF);
	LOG::writeMessageInFile("min max weight:");
	LOG::writeParamInFile(Parameters::MAX_RANDOM_WEIGHT);
	LOG::writeMessageInFile("sin frequency:");
	LOG::writeParamInFile(Parameters::SIN_FREQUENCY);
	LOG::writeMessageInFile("MAX_POTENTIAL");
	LOG::writeParamInFile(Parameters::MAX_POTENTIAL);
	LOG::writeMessageInFile("MIN_POTENTIAL");
	LOG::writeParamInFile(Parameters::MIN_POTENTIAL);
	LOG::writeMessageInFile("ACTIVATION_THR:");
	LOG::writeParamInFile(Parameters::ACTIVATION_THR);
	LOG::writeMessageInFile("GAUSSE_BOUND_VALUE:");
	LOG::writeParamInFile(Parameters::GAUSSE_BOUND_VALUE);
	LOG::writeMessageInFile("use normalization:");
	LOG::writeParamInFile(Parameters::useNormalization);
	LOG::writeMessageInFile("use randomAddition:");
	LOG::writeParamInFile(Parameters::useRandomAddition);
	LOG::writeMessageInFile("A_RAND:");
	LOG::writeParamInFile(Parameters::A_RAND);
	LOG::writeMessageInFile("U_RAND:");
	LOG::writeParamInFile(Parameters::U_RAND);
	LOG::writeMessageInFile("RESULT_THR:");
	LOG::writeParamInFile(Parameters::RESULT_THR);
	LOG::writeMessageInFile("NEURON_DELAY_TIME:");
	LOG::writeParamInFile(Parameters::NEURON_DELAY_TIME);

	LOG::writeMessageInFile("useReadyNetwork:");
	LOG::writeParamInFile(Parameters::useReadyNetwork);
	if(Parameters::useReadyNetwork)
	{
		if(Parameters::readyNetworkType == Constants::FEEDFORWARD_NETWORK_TYPE)
		{
			LOG::writeMessageInFile("readyNetworkType: FEEDFORWARD_NETWORK_TYPE");

			vector<double> matrix;
			for(int i=1;i<Parameters::neuronsMatrix.size()-1;i++)
				matrix.push_back(Parameters::neuronsMatrix[i]);
			LOG::writeMessageInFile("neurons matrix:");
			LOG::writeArrInFile(matrix);
		}
		else if(Parameters::readyNetworkType == Constants::FULLY_CONNECTED_NETWORK)
		{
			LOG::writeMessageInFile("readyNetworkType: FULLY_CONNECTED_NETWORK");
			LOG::writeMessageInFile("FULLY_NETW_NEURON_NUM:");
			LOG::writeParamInFile(Parameters::FULLY_NETW_NEURON_NUM);
		}
		else if(Parameters::readyNetworkType == Constants::JORDAN_SIMILAR_NETWORK)
		{
			LOG::writeMessageInFile("readyNetworkType: JORDAN_SIMILAR_NETWORK");

			LOG::writeMessageInFile("INPUT_DELAY_NUM:");
			LOG::writeParamInFile(Parameters::INPUT_DELAY_NUM);

			LOG::writeMessageInFile("BACK_FROM_OUTPUT_DELAY_NUM:");
			LOG::writeParamInFile(Parameters::BACK_FROM_OUTPUT_DELAY_NUM);
		}
	}

	if(Parameters::selectionType == Constants::ROULETTE_SEL_TYPE)
		LOG::writeMessageInFile("selectionType: ROULETTE_SEL_TYPE");
	else if(Parameters::selectionType == Constants::RANDOM_SEL_TYPE)
		LOG::writeMessageInFile("selectionType: RANDOM_SEL_TYPE");

	LOG::writeMessageInFile("portions of childs:");
	LOG::writeParamInFile(Parameters::PORTION_OF_CHILDS);

	LOG::writeMessageInFile("mutateStablePartOfPopulation:");
	LOG::writeParamInFile(Parameters::mutateStablePartOfPopulation);

	LOG::writeMessageInFile("selectFromStable:");
	LOG::writeParamInFile(Parameters::selectFromStable);

	LOG::writeMessageInFile("P_MUTATION_WEIGHT:");
	LOG::writeParamInFile(Parameters::P_MUTATION_WEIGHT);
	LOG::writeMessageInFile("S_WEGHT:");
	LOG::writeParamInFile(Parameters::S_WEGHT);
	LOG::writeMessageInFile("P_MUTATION_THRESHOLD:");
	LOG::writeParamInFile(Parameters::P_MUTATION_THRESHOLD);
	LOG::writeMessageInFile("S_THRESH:");
	LOG::writeParamInFile(Parameters::S_THRESH);

	LOG::writeMessageInFile("START_INTERM_NEURON_NUM");
	LOG::writeParamInFile(Parameters::START_INTERM_NEURON_NUM);

	LOG::writeMessageInFile("P_DUPLICATION:");
	LOG::writeParamInFile(Parameters::P_DUPLICATION);

	LOG::writeMessageInFile("P_DELETION:");
	LOG::writeParamInFile(Parameters::P_DELETION);

	LOG::writeMessageInFile("P_CONNECTION_ADD:");
	LOG::writeParamInFile(Parameters::P_CONNECTION_ADD);

	LOG::writeMessageInFile("P_CONNECTION_REMOVE:");
	LOG::writeParamInFile(Parameters::P_CONNECTION_REMOVE);

	LOG::writeMessageInFile("P_MUTATION_WEIGHT:");
	LOG::writeParamInFile(Parameters::P_MUTATION_WEIGHT);

	LOG::writeMessageInFile("P_MUTATION_THRESHOLD:");
	LOG::writeParamInFile(Parameters::P_MUTATION_THRESHOLD);

	LOG::writeMessageInFile("P_MUTATION_FREQUENCY:");
	LOG::writeParamInFile(Parameters::P_MUTATION_FREQUENCY);

	LOG::writeMessageInFile("P_MUTATION_RES_THRESHOLD:");
	LOG::writeParamInFile(Parameters::P_MUTATION_RES_THRESHOLD);

	LOG::writeMessageInFile("P_MUTATION_TIME:");
	LOG::writeParamInFile(Parameters::P_MUTATION_TIME);

	LOG::writeMessageInFile("S_TIME_DELAY:");
	LOG::writeParamInFile(Parameters::S_TIME_DELAY);

	LOG::writeMessageInFile("S_RES_THRESH:");
	LOG::writeParamInFile(Parameters::S_RES_THRESH);

	LOG::writeMessageInFile("S_FREQUENCY:");
	LOG::writeParamInFile(Parameters::S_FREQUENCY);

	LOG::endFileInput();
}

void Parameters::printTransFunc(int type)
{
	if(type == Constants::LOGISTIC_FUNCTION)
		LOG::writeMessageInFile("LOGISTIC_FUNCTION");
	else if(type == Constants::THRESHOLD_FUNCTION)
		LOG::writeMessageInFile("LOGISTIC_FUNCTION");
	else if(type == Constants::GAUSS_FUNCTION)
		LOG::writeMessageInFile("GAUSS_FUNCTION");
	else if(type == Constants::LOG_AND_THRESHOLD_FUNCTION)
		LOG::writeMessageInFile("LOG_AND_THRESHOLD_FUNCTION");
	else if(type == Constants::SIN_FUNCTION)
		LOG::writeMessageInFile("SIN_FUNCTION");
	else if(type == Constants::NONE_FUNCTION)
		LOG::writeMessageInFile("NONE_FUNCTION");
}