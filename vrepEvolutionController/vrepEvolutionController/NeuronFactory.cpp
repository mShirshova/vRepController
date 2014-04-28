#include "NeuronFactory.h"

/*Different type of neuron are needed only if some operations related to clone\delete neurons and connections are different*/
Neuron* NeuronFactory::createNeuron(int neuronType)
{
	if(neuronType == Constants::AR_NEURON)
		return new ARNeuron();
	else
		return new Neuron();

	return new Neuron();
}

void NeuronFactory::createNetworkParams(NetworkParams& params)
{
	params.type = Parameters::networkType;
	params.neuronType = Parameters::neuronType;
	createNeuronParams(params.neuronParams);

	if(Parameters::useReadyNetwork)
	{
		if(Parameters::readyNetworkType == Constants::FEEDFORWARD_NETWORK_TYPE)
			createFeedForwardNetStrucuture(params);
		else if(Parameters::readyNetworkType == Constants::FULLY_CONNECTED_NETWORK)
			createFullyConnectedNetStrucuture(params);
		else if(Parameters::readyNetworkType == Constants::JORDAN_SIMILAR_NETWORK)
			createJordanNetworkStrucuture(params);
	}
	else
		createTreeLayerNetStrucuture(params);
}

void NeuronFactory::createTreeLayerNetStrucuture(NetworkParams& params)
{
	params.structure.resize(3);//input, interm and output neurons 
	params.structure[0] = Parameters::ALL_SPACE_DIM;
	params.structure[1] = Parameters::START_INTERM_NEURON_NUM;//create only one intermediate neuron
	params.structure[2] = Parameters::OUTPUT_NUM;

	createStructure(params);
}

void NeuronFactory::createFullyConnectedNetStrucuture(NetworkParams& params)
{
	params.structure.resize(3);
	params.structure[0] = 0;
	params.structure[1] = Parameters::FULLY_NETW_NEURON_NUM - Parameters::JOINT_NUM;
	params.structure[2] = Parameters::JOINT_NUM;

	int id = 0;
	vector<int> ids;

	for(int i=1; i < 3; ++i)
	{
		for(int j=0; j < params.structure[i]; j++)
		{
			if(i == 2)
				params.typeMap[id] = Constants::OUTPUT_NEURON_TYPE;
			else 
				params.typeMap[id] = Constants::INTERM_NEURON_TYPE;

			ids.push_back(id);
			id++;
		}
	}

	vector<int>::iterator iter;
	for(int i=0; i < ids.size(); ++i)
	{
		params.connections[ids[i]] = ids;
		iter = find(params.connections[ids[i]].begin(), params.connections[ids[i]].end(), ids[i]);
		params.connections[ids[i]].erase(iter);
	}
}

void NeuronFactory::createFeedForwardNetStrucuture(NetworkParams& params)
{
	params.structure.resize(0);
	string s="";
	string& nS = Parameters::neuronsMatrix;
	bool b = true;
	unsigned int sPos=0;
	unsigned int ePos=0;
	int ind = 0;
	while(b)
	{
		ePos = nS.find_first_of(',',sPos);
		if(ePos < 0 || ePos > nS.length() - 1)
		{
			ePos = nS.length() ;
			b = false;
		}
		s = nS.substr(sPos,ePos-sPos);
		params.structure.push_back(atoi(s.data()));
		ind++;
		sPos = ePos+1;
	}

	createStructure(params);
}

void NeuronFactory::createStructure(NetworkParams& params)
{
	int layersNum; 
	int id = 0;
	vector<int> curIds;
	vector<int> prevIds;

	layersNum = params.structure.size();
	for(int i=0; i < layersNum; i++)
	{
		curIds.resize(0);

		for(int j=0; j < params.structure[i]; j++)
		{
			//fill map of types
			if(i == 0)//input layer
				params.typeMap[id] = Constants::INPUTE_NEURON_TYPE;
			else if(i == layersNum - 1)
				params.typeMap[id] = Constants::OUTPUT_NEURON_TYPE;
			else 
				params.typeMap[id] = Constants::INTERM_NEURON_TYPE;

			//fill map of connections
			if(i > 0)
			{
				params.connections[id] = prevIds;//push all id-s of prev layer neuron
				if(Parameters::neuronType == Constants::AR_NEURON)
					params.arConnections[id] = prevIds;
			}

			curIds.push_back(id);
			id++;
		}

		prevIds = curIds;
	}
}

void NeuronFactory::createJordanNetworkStrucuture(NetworkParams& params)
{
	params.structure.resize(3);
	params.structure[0] = Parameters::ALL_SPACE_DIM*(Parameters::INPUT_DELAY_NUM+1) + Parameters::JOINT_NUM*Parameters::BACK_FROM_OUTPUT_DELAY_NUM;
	params.structure[1] = Parameters::START_INTERM_NEURON_NUM;
	params.structure[2] = Parameters::JOINT_NUM;

	createStructure(params);

	int startOutId = params.structure[0] + params.structure[1] - 1;
	vector<int> ids(0);
	ids.push_back(startOutId);
	int id = Parameters::ALL_SPACE_DIM*(Parameters::INPUT_DELAY_NUM+1);

	for(int i=0; i < Parameters::JOINT_NUM; ++i)
	{
		for(int j=0; j < Parameters::BACK_FROM_OUTPUT_DELAY_NUM; ++j)
		{
			params.connections[id] = ids;
			++id;
		}
		++ids[0];
	}
}

void NeuronFactory::setDelaysNum(Neurons& neurons)
{
	if(Parameters::useReadyNetwork && Parameters::readyNetworkType == Constants::JORDAN_SIMILAR_NETWORK)
	{
		int id=0;
		int dNum=0;
		for(int i=0; i<Parameters::JOINT_NUM; ++i)
		{
			dNum = 0;
			for(int j=0; j<Parameters::INPUT_DELAY_NUM+1; j++)
			{
				neurons[id].neuronParams.delayNum = dNum;
				++id;
				++dNum;
			}
		}

		for(int i=0; i<Parameters::JOINT_NUM; ++i)
		{
			dNum = 0;
			for(int j=0; j<Parameters::BACK_FROM_OUTPUT_DELAY_NUM; j++)
			{
				neurons[id].neuronParams.delayNum = dNum;
				++id;
				++dNum;
			}
		}
	}
}

void NeuronFactory::setTransmissionFunc(Neurons& neurons)
{
	int size = neurons.size();
	for(int i=0; i < size; ++i)
	{
		if(neurons[i].type == Constants::INPUTE_NEURON_TYPE)
			neurons[i].neuronParams.transmissionFuncType = Parameters::inputTransmissionFuncType;
		else if(neurons[i].type == Constants::INTERM_NEURON_TYPE)
			neurons[i].neuronParams.transmissionFuncType = Parameters::intermTransmissionFuncType;
		else 
			neurons[i].neuronParams.transmissionFuncType = Parameters::outTransmissionFuncType;
	}
}

void NeuronFactory::setJointsNums(Neurons& neurons)
{
	if(Parameters::useReadyNetwork && Parameters::readyNetworkType == Constants::JORDAN_SIMILAR_NETWORK)
		setJointNumsForJordanNet(neurons);
	else
	{
		int jNum = 0;
		int size = neurons.neuronsPtrs.size();

		for(int i=0; i < size; ++i)
		{
			if((neurons.neuronsPtrs[i]->type == Constants::INPUTE_NEURON_TYPE && i==0)
				|| (i > 0 && neurons.neuronsPtrs[i-1]->type == Constants::INTERM_NEURON_TYPE &&  neurons.neuronsPtrs[i]->type == Constants::OUTPUT_NEURON_TYPE))
				jNum = 0;

			if(neurons.neuronsPtrs[i]->type == Constants::INPUTE_NEURON_TYPE)
			{
				neurons.neuronsPtrs[i]->neuronParams.jNum = jNum;
				++jNum;
			}
			else if(neurons.neuronsPtrs[i]->type == Constants::OUTPUT_NEURON_TYPE)
			{
				neurons.neuronsPtrs[i]->neuronParams.jNum = jNum;

				if(Parameters::typeOfControl == Constants::ONE_NEURON_CONTROL || Parameters::typeOfControl == Constants::FORCE_COTROL)
					++jNum;
				else
				{
					if(i>0 && (neurons.neuronsPtrs[i-1]->type != Constants::OUTPUT_NEURON_TYPE
						|| !neurons.neuronsPtrs[i-1]->neuronParams.isIncreasing))
						neurons.neuronsPtrs[i]->neuronParams.isIncreasing = true;
					else if(i>0 && neurons.neuronsPtrs[i-1]->neuronParams.isIncreasing)
					{
						neurons.neuronsPtrs[i]->neuronParams.isIncreasing = false;
						++jNum;
					}
				}
			}
		}
	}
}

void NeuronFactory::addInhibitoryConnectionsToOutpputNeurons(Neurons& neurons)
{
	//return if only one neuron is used for neuron
	if(Parameters::typeOfControl == Constants::ONE_NEURON_CONTROL || Parameters::typeOfControl == Constants::FORCE_COTROL)
		return;

	int size = neurons.neuronsPtrs.size();
	int outNum = neurons.getStartID(Constants::OUTPUT_NEURON_TYPE);

	for(int i=outNum; i<size; i+=2)
	{
		neurons.neuronsPtrs[i]->addConnection(Constants::OUTPUT_NEURON_TYPE,i+1,-1,Constants::SIMPLE_CONNECTION);
		neurons.neuronsPtrs[i+1]->addConnection(Constants::OUTPUT_NEURON_TYPE,i,-1,Constants::SIMPLE_CONNECTION);
	}
}

void NeuronFactory::addBackConnectionsToOutputNeurons(Neurons& neurons)
{
	int size = neurons.neuronsPtrs.size();
	int outNum = neurons.getStartID(Constants::OUTPUT_NEURON_TYPE);

	for(int i=outNum; i<size; ++i)
	{
		neurons.neuronsPtrs[i]->addConnection(Constants::OUTPUT_NEURON_TYPE,i,0.5,Constants::SIMPLE_CONNECTION);
	}
}

void NeuronFactory::setJointNumsForJordanNet(Neurons& neurons)
{
	int jNum = 0;
	int size = neurons.neuronsPtrs.size();
	int jointInputNeuronsNum = Parameters::ALL_SPACE_DIM*(Parameters::INPUT_DELAY_NUM+1);
	int d = 0;

	for(int i=0; i < size; ++i)
	{
		if((neurons.neuronsPtrs[i]->type == Constants::INPUTE_NEURON_TYPE && i==0)
			|| (i > 0 && neurons.neuronsPtrs[i-1]->type == Constants::INTERM_NEURON_TYPE &&  neurons.neuronsPtrs[i]->type == Constants::OUTPUT_NEURON_TYPE))
			jNum = 0;

		if(neurons.neuronsPtrs[i]->type == Constants::INPUTE_NEURON_TYPE && i < jointInputNeuronsNum)
		{
			neurons.neuronsPtrs[i]->neuronParams.jNum = jNum;
			if((i+1) % (Parameters::INPUT_DELAY_NUM+1) == 0)
				++jNum;
		}
		else if(neurons.neuronsPtrs[i]->type == Constants::OUTPUT_NEURON_TYPE)
		{
			neurons.neuronsPtrs[i]->neuronParams.jNum = jNum;
			++jNum;
		}
	}
}


void NeuronFactory::createNeuronParams(NeuronParams& params)
{
	params.logCoeff = Parameters::LOG_FUNC_K_COEFF;
	params.delayTime = Parameters::NEURON_DELAY_TIME;
	params.resThreshold = Parameters::RESULT_THR;
	params.threshold = Parameters::ACTIVATION_THR;
	params.useNormalization = Parameters::useNormalization;
	params.useRandomAddition = Parameters::useRandomAddition;
	params.useARConnections = false;
	params.maxPotential = Parameters::MAX_POTENTIAL;
	params.minPotential = Parameters::MIN_POTENTIAL;
	params.frequency = Parameters::SIN_FREQUENCY;
	params.uRand = Parameters::U_RAND;
	params.aRand = Parameters::A_RAND;
	params.lBound = 0;
	params.rBound = 1;
	params.boundVal = Parameters::GAUSSE_BOUND_VALUE;
	params.delayNum = 0;
	params.jNum = -1;

	params.minWeightValue = -Parameters::MAX_RANDOM_WEIGHT;
	params.maxWeightValue = Parameters::MAX_RANDOM_WEIGHT;

	if(Parameters::neuronType == Constants::AR_NEURON)
		params.useARConnections = true;
	
}