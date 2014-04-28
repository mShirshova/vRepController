#include "Generation.h"
#include "Statistics.h"

Generation::Generation(void)
{
}

void Generation::reset()
 {
	curGeneration = 0;
	curDescendant = 0;
	descensantNum = Parameters::DESCENDANT_NUM;
	generationNum = Parameters::GENERATION_NUM;
}

bool Generation::hasNextGenertion()
{
	return curGeneration < generationNum;
}

bool Generation::hasNextDescendant()
{
	return curDescendant < descensantNum;
}

AgentInfo* Generation::getNextDescendant()
{
	LOG::printParam("cur descendant", curDescendant);
	return &descendances[curDescendant++];
}

void Generation::createNewGeneration(bool createFirst)
{
	LOG::printParam("cur generation", curGeneration);

	if(curGeneration >= generationNum) return;

	if(curGeneration == 0)
	{
		if(createFirst)
		{
			descendances.resize(Parameters::DESCENDANT_NUM);
			clearDescendants();//for that case if descendants already exist

			NetworkParams params;
			Parameters::networkType;
			NeuronFactory::createNetworkParams(params);

			for(int i=0; i< descensantNum; i++)
			{
				AgentInfo& agInf = descendances[i];
				agInf.initializeNetwork(params);
				NeuronFactory::setJointsNums(agInf.neurons);
				NeuronFactory::setDelaysNum(agInf.neurons);
				NeuronFactory::setTransmissionFunc(agInf.neurons);

				if(Parameters::addInhibitoryConnectionsToOutput)
					NeuronFactory::addInhibitoryConnectionsToOutpputNeurons(agInf.neurons);

				if(Parameters::addBackConnectionToOutput)
					NeuronFactory::addBackConnectionsToOutputNeurons(agInf.neurons);

				///TEMP
				mutateDelayTimes(agInf);
			}
		}
		return;
	}

	if(Parameters::evolutionAlgorithmType == Constants::GENETIC_ALG)
	{
		
		curDescendant = 0;
		descendances_prev = descendances;
		int numOfConnects = 0;

		if(Parameters::selectionType == Constants::ROULETTE_SEL_TYPE)
			calcProbDiapasones(descendances_prev);

		if(Parameters::descendantCreationType == Constants::CREATE_DESC_WITH_CROSSOVER)
			numOfConnects = calcNumOfConnections(descendances_prev[0].neurons.networkParams.structure);
			
		sort(descendances_prev.begin(), descendances_prev.end());

		int stablePart = descensantNum*(1 - Parameters::PORTION_OF_CHILDS);
		int descNum = 0;
		AgentInfo* parent1;
		AgentInfo* parent2;
		
		while(descNum != descensantNum)
		{
			if(descNum < stablePart)
			{
				descendances[descNum] = descendances_prev[descNum];
				
				if(Parameters::mutateStablePartOfPopulation)
					mutateParams(descendances[descNum]);
				++descNum;
			}
			else
			{
				if(Parameters::selectionType == Constants::ROULETTE_SEL_TYPE)
				{
					parent1 = selectDescendant(descendances_prev);
					parent2 = selectDescendant(descendances_prev);
				}
				else if(Parameters::selectionType == Constants::RANDOM_SEL_TYPE)
				{
					int num = Parameters::selectFromStable ? stablePart : descensantNum;

					parent1 = &descendances_prev[Util::generateRandomIntegerValue(0,num)];
					parent2 = &descendances_prev[Util::generateRandomIntegerValue(0,num)];
				}

				
				if(Parameters::descendantCreationType == Constants::CREATE_DESC_WITH_CROSSOVER)
				{
					if(descNum < descensantNum - 1)
					{
						createDescendanceWithCrossover(*parent1, *parent2, descendances[descNum], descendances[descNum+1], 
											true, numOfConnects);
						
						mutateParams(descendances[descNum]);
						mutateParams(descendances[descNum+1]);
						descNum += 2;
					}
					else
					{
						descendances[descNum] = *parent1;
						mutateParams(descendances[descNum]);
						++descNum;
					}
				}
				else if(Parameters::descendantCreationType == Constants::CREATE_DESC_MB)
				{
					createDescendanceMB(*parent1, descendances[descNum]);
					++descNum;
				}
				else if(Parameters::descendantCreationType == Constants::CREATE_DESC_MB_2)
				{
					createDescendanceMB_2(*parent1, descendances[descNum]);
					++descNum;
				}
			}
		}
	}
}

/***********************************************
* For networks with fixed architecture
***************************************************/
void Generation::createDescendanceWithCrossover(AgentInfo& parent1, AgentInfo& parent2, AgentInfo& child1, 
	                                AgentInfo& child2, bool crossParents, int numOfConnections)
{
	if(crossParents)//realize crossover of parent
	{
		int crossPoint = Util::generateRandomIntegerValue(0, numOfConnections);
		bool isCross = false;
		int curConnect = 0;
		int size = parent1.neurons.size();
		int size1;
		child1 = parent1;
		child2 = parent2;

		for(int i = 0; i < size; i++)
		{
			size1 = parent1.neurons[i].connections.size();
			for(int j = 0; j < size1; j++)
			{
				isCross = curConnect >= crossPoint;
				if(isCross)
				{
					child1.neurons[i].connections[j].weight = parent2.neurons[i].connections[j].weight;
					child2.neurons[i].connections[j].weight = parent1.neurons[i].connections[j].weight;
				}
				else
				{
					child1.neurons[i].connections[j].weight = parent1.neurons[i].connections[j].weight;
					child2.neurons[i].connections[j].weight = parent2.neurons[i].connections[j].weight;
				}
				curConnect++;
			}
			//exchange others params
			if(isCross)
			{
				child1.neurons[i].neuronParams.threshold = parent2.neurons[i].neuronParams.threshold;
				child1.neurons[i].neuronParams.resThreshold = parent2.neurons[i].neuronParams.resThreshold;
				child1.neurons[i].neuronParams.delayTime = parent2.neurons[i].neuronParams.delayTime;
				child1.neurons[i].neuronParams.frequency = parent2.neurons[i].neuronParams.frequency;

				child2.neurons[i].neuronParams.threshold = parent1.neurons[i].neuronParams.threshold;
				child2.neurons[i].neuronParams.resThreshold = parent1.neurons[i].neuronParams.resThreshold;
				child2.neurons[i].neuronParams.delayTime = parent1.neurons[i].neuronParams.delayTime;
				child2.neurons[i].neuronParams.frequency = parent1.neurons[i].neuronParams.frequency;
			}
			else
			{
				child1.neurons[i].neuronParams.threshold = parent1.neurons[i].neuronParams.threshold;
				child1.neurons[i].neuronParams.delayTime = parent1.neurons[i].neuronParams.delayTime;
				child1.neurons[i].neuronParams.resThreshold = parent1.neurons[i].neuronParams.resThreshold;
				child1.neurons[i].neuronParams.frequency = parent1.neurons[i].neuronParams.frequency;

				child2.neurons[i].neuronParams.threshold = parent2.neurons[i].neuronParams.threshold;
				child2.neurons[i].neuronParams.delayTime = parent2.neurons[i].neuronParams.delayTime;
				child2.neurons[i].neuronParams.resThreshold = parent2.neurons[i].neuronParams.resThreshold;
				child2.neurons[i].neuronParams.frequency = parent2.neurons[i].neuronParams.frequency;
			}
		}
	}
	else
	{
		child1 = parent1;
		child2 = parent2;
	}
}


/***********************************************
* GENETIC_ALG_FOR_FFNETWORK end
***************************************************/

void Generation::createDescendanceMB(AgentInfo& parent, AgentInfo& child)
{
	child = parent;

	vector<int> addedNIds(0);//ids of interm neurons which must be cloned
	vector<int> deletedNIds(0);//ids of interm neurons which must be deleted

	//dublicate and delete of interm neurons
	int startID = parent.neurons.getStartID(Constants::INTERM_NEURON_TYPE);
	int endID = parent.neurons.getEndID(Constants::INTERM_NEURON_TYPE);
	if(endID - startID > 0)
		selectRandomNeurons(startID, endID, deletedNIds, Parameters::P_DELETION);
	////temp to not remove all interm neurons
	if(deletedNIds.size() == endID - startID + 1)
		deletedNIds.erase(deletedNIds.begin(),deletedNIds.begin()+1);
	child.neurons.removeNeurons(deletedNIds);

	startID = child.neurons.getStartID(Constants::INTERM_NEURON_TYPE);
	endID = child.neurons.getEndID(Constants::INTERM_NEURON_TYPE);
	selectRandomNeurons(startID, endID, addedNIds, Parameters::P_DUPLICATION);
	child.neurons.cloneNeurons(addedNIds);
	
	//add new coonectionts to the neurons
	addedNIds.resize(0);
	deletedNIds.resize(0);
	//again select neuronts to which connections must be added or deleted
	startID = 0;//child.neurons.getStartID(Constants::INTERM_NEURON_TYPE);
	endID = child.neurons.size()-1;//child.neurons.getEndID(Constants::INTERM_NEURON_TYPE);

	selectRandomNeurons(startID, endID, addedNIds, Parameters::P_CONNECTION_ADD);
	selectRandomNeurons(startID, endID, deletedNIds, Parameters::P_CONNECTION_REMOVE);

	child.neurons.addConnections(addedNIds);
	child.neurons.removeConnections(deletedNIds);

	mutateParams(child);
}

void Generation::createDescendanceMB_2(AgentInfo& parent, AgentInfo& child)
{
	child = parent;

	vector<int> addedNIds(0);//ids of interm neurons which must be cloned
	vector<int> deletedNIds(0);//ids of interm neurons which must be deleted

	int startID;
	int endID;
	if(Parameters::deleteInputNeurons)
		startID = 0;
	else
		startID = child.neurons.getStartID(Constants::INTERM_NEURON_TYPE);

	if(Parameters::deleteOutputNeurons)
		endID = child.neurons.size()-1;
	else
		endID = child.neurons.getEndID(Constants::INTERM_NEURON_TYPE);

	selectRandomNeurons(startID, endID, deletedNIds, Parameters::P_DELETION);
	child.neurons.removeNeurons(deletedNIds);

	if(Parameters::dublicateInputNeurons)
		startID = 0;
	else
		startID = child.neurons.getStartID(Constants::INTERM_NEURON_TYPE);

	if(Parameters::dublicateOutputNeurons)
		endID = child.neurons.size()-1;
	else
		endID = child.neurons.getEndID(Constants::INTERM_NEURON_TYPE);

	selectRandomNeurons(startID, endID, addedNIds, Parameters::P_DUPLICATION);
	child.neurons.cloneNeurons(addedNIds);
	
	//add new coonectionts to the neurons
	addedNIds.resize(0);
	deletedNIds.resize(0);

	selectRandomNeurons(0, child.neurons.size()-1, addedNIds, Parameters::P_CONNECTION_ADD);
	selectRandomNeurons(0, child.neurons.size()-1, deletedNIds, Parameters::P_CONNECTION_REMOVE);

	child.neurons.addConnections(addedNIds);
	child.neurons.removeConnections(deletedNIds);

	mutateParams(child);
}

/*Delete all input and output neurons*/
void checkInputAndOutputNeurons(vector<int>& ids, Neurons& neurons)
{
	vector<int>::iterator iter = ids.begin();
	int ind=0;
	while(iter != ids.end())
	{
		if(neurons.neuronsPtrs[*iter]->type == Constants::INPUTE_NEURON_TYPE
			|| neurons.neuronsPtrs[*iter]->type == Constants::OUTPUT_NEURON_TYPE)
		{
			ind = *iter;
			ids.erase(iter);
			iter = ids.begin()+ind;
		}
		else
			++iter;
	}
}

void Generation::selectRandomNeurons(int startID, int endID, vector<int>& ids, double prob)
{
	if(startID >= 0 && endID >= 0)
	{
		for(int i=startID; i<=endID; i++)
		{
			if(Util::checkChance(prob))
				ids.push_back(i);
		}
	}
}

void Generation::mutateParams(AgentInfo& child)
{
	int size = child.neurons.size();
	int size1 = 0;
	for(int i=0; i<size; i++)
	{
		size1 = child.neurons[i].connections.size();

		if(Util::checkChance(Parameters::P_MUTATION_TIME))
			child.neurons[i].neuronParams.delayTime += Util::generateRandomValue(-Parameters::S_TIME_DELAY,Parameters::S_TIME_DELAY);//Util::generateGaussRandomValue(0, Parameters::S_TIME_DELAY);

		if(Util::checkChance(Parameters::P_MUTATION_THRESHOLD))
			child.neurons[i].neuronParams.threshold += Util::generateRandomValue(-Parameters::S_THRESH,Parameters::S_THRESH);//Util::generateGaussRandomValue(0, Parameters::S_THRESH);

		if(Util::checkChance(Parameters::P_MUTATION_RES_THRESHOLD))
			child.neurons[i].neuronParams.resThreshold += Util::generateRandomValue(-Parameters::S_RES_THRESH,Parameters::S_RES_THRESH);//Util::generateGaussRandomValue(0, Parameters::S_RES_TIME_DELAY);

		if(Util::checkChance(Parameters::P_MUTATION_FREQUENCY))
			child.neurons[i].neuronParams.frequency += Util::generateRandomValue(-Parameters::S_FREQUENCY,Parameters::S_FREQUENCY);

		
		//mutate weights
		for(int j=0; j<size1; j++)
		{
			Connection& c = child.neurons[i].connections[j];
			if(Util::checkChance(Parameters::P_MUTATION_WEIGHT))
				c.weight += Util::generateRandomValue(-Parameters::S_WEGHT,Parameters::S_WEGHT);//Util::generateGaussRandomValue(0, Parameters::S_WEGHT);
		}
	}
}

void Generation::mutateDelayTimes(AgentInfo& child)
{
	int size = child.neurons.size();

	for(int i=0; i<size; i++)
	{
		child.neurons[i].neuronParams.delayTime += Util::generateRandomValue(-Parameters::S_TIME_DELAY,Parameters::S_TIME_DELAY);//Util::generateGaussRandomValue(0, Parameters::S_TIME_DELAY);
	}
}

void Generation::calcProbDiapasones(vector<AgentInfo>& descns)
{
	diapasones.resize(descns.size());
	double fitnessSum = 0;

	int size = descns.size();

	for(int i = 0; i < size; i++)
		fitnessSum += descns[i].fitness;

	for(int i = 0; i < size; i++)
		diapasones[i].probability = descns[i].fitness/fitnessSum;

	//generate diapasone for random search of descendance whitch depends of order of descendances num
	// <10 descendances - 0..100 (order - 0)
	// >10 <100 - 10000 (order 1)
	// etc
	diapasoneSize = (int) pow(10.0, Util::calcOrderOfNumer(descns.size())+2);

	for(int i = 0; i < size; i++)
	{
		probDiapasone& obj = diapasones[i];
		if(i == 0)
		{
			obj.inf = 0;
			obj.supr = floor(diapasoneSize*obj.probability);
		}
		else if(i == descns.size() - 1)
		{
			obj.inf = diapasones[i-1].supr;
			 obj.supr = diapasoneSize;
		}
		else
		{
			obj.inf = diapasones[i-1].supr;
		    obj.supr = obj.inf + floor(diapasoneSize*obj.probability);
		}
	}
}


//select parent through the probability which calculated with help of fitness function
AgentInfo* Generation::selectDescendant(vector<AgentInfo>& descns)
{
	int rand = Util::generateRandomValue(0, diapasoneSize);
	int size =  descns.size();
	for(int i = 0; i < size; i++)
	{
		if(rand >= diapasones[i].inf && rand <= diapasones[i].supr){
			return &descns[i];
		}
	}

	//it is impossible!!! 
	return &descns[0];
}

void Generation::calcFitness(AgentInfo& agent)
{
	if(Parameters::typeOfFitness == Constants::DISTANCE_FTNESS)
		agent.fitness = agent.distance;
	else if(Parameters::typeOfFitness == Constants::ACTIVITY_FTNESS)
		agent.fitness = agent.activity;
	else if(Parameters::typeOfFitness == Constants::MIX_FTNESS)
	{
		if(curGeneration < generationNum/2)
			agent.fitness = agent.activity;
		else 
			agent.fitness = agent.distance;
	}
}


int Generation::calcNumOfConnections(vector<int>& nm)
{
	int num = 0;
	int size = nm.size();
	for(int i=1; i < size; i++)
		num+= nm[i]*nm[i-1];
	return num;
}

void Generation::clearDescendants()
{
	int size = descendances.size();
	for(int i=0; i<size; ++i)
		descendances[i].clear();
}

void Generation::createGenerationFromOneDescendant(AgentInfo& agent)
{
	descendances.resize(descensantNum);

	for(int i=0; i<descensantNum; ++i)
	{
		descendances[i] = agent;
		if(Util::checkChance(0.5))
			mutateParams(descendances[i]);
	}
}

Generation::~Generation(void)
{
}
