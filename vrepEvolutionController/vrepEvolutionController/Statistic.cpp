#include "Statistics.h"

Statistic::Statistic()
{
}

Statistic::~Statistic()
{
	clear();
}

void  Statistic::clear()
{
	avFitness.resize(0);
	maxFitness.resize(0);
	maxNumOfNeurons.resize(0);
	minNumOfNeurons.resize(0);
	avNumOfNeurons.resize(0);
	maxNumOfConnections.resize(0);
	minNumOfConnections.resize(0);
	avNumOfConnections.resize(0);
	avNetConsistency.resize(0);
	bestAgentNetConsistency.resize(0);
	avResultTime.resize(0);
	bestAgentResultTime.resize(0);

	avWeight.resize(0);
	fitnessDiversity.resize(0);
	sortedAgents.resize(0);
	
	jointValues.resize(0);
	outNetwValuesValues.resize(0);

	bestAgentAvDelayTime.resize(0);
	dispersOfBestAgentAvDelayTime.resize(0);
	averageAvDelayTime.resize(0);
	dispersOfAverageAvDelayTime.resize(0);
}

double Statistic::calcAvNetConsistency(Generation& g)
{
	double res = 0;
	for(int i=0; i<g.descensantNum; i++)
	{
		res += g.descendances[i].netwConsistency;
	}

	res /= g.descensantNum;
	return res;
}

double  Statistic::calcAvarageResultTime(Generation& g)
{
	double res = 0;
	for(int i=0; i<g.descensantNum; i++)
	{
		res += g.descendances[i].timeOfAchievingResults;
	}

	res /= g.descensantNum;
	return res;
}

double Statistic::calcAverageFitness(Generation& g)
{
	double res = 0;
	for(int i=0; i<g.descensantNum; i++)
	{
		res += g.descendances[i].fitness;
	}
	res /= g.descensantNum;
	return res;
}

double Statistic::calcMaxFitness(Generation& g)
{
	double max = 0;
	for(int i=0; i<g.descensantNum; i++)
	{
		if(g.descendances[i].fitness > max)
			max = g.descendances[i].fitness;
	}
	return max;
}

double Statistic::calcFitnessDiversity(Generation& g)
{
	double res = 0;
	int num = 0;
	for(int i=0; i<g.descensantNum; i++)
	{
		for(int j=0; j<g.descensantNum; j++)
		{
			if(i != j)
			{
				res += pow((g.descendances[i].fitness - g.descendances[j].fitness),2);
				num++;
			}
		}
	}
	return sqrt(res/num);
}

double Statistic::calcAvarageWeight(Generation& g)
{
	double res = 0;
	int num = 0;
	AgentInfo& agent = g.descendances[getMaxFitnDescendantNum(g)];
	int size = agent.neurons.size();
	int size1 = 0;
	for(int i=0; i < size; i++)
	{
		size1 = agent.neurons[i].connections.size();
		for(int j=0; j < size1; j++)
		{
			res += agent.neurons[i].connections[j].weight;
			num++;
		}
	}
	res /= num;
	return res;
}

double Statistic::calcAvarageDelayTimeInAgent(AgentInfo& agent)
{
	double res = 0;
	int size = agent.neurons.size();

	for(int i=0; i < size; i++)
		res += agent.neurons[i].neuronParams.delayTime;

	res /= size;
	return res;
}

double Statistic::calcAvarageDelayTimeInGeneration(Generation& g)
{
	double res = 0;
	for(int i=0; i < g.descensantNum; i++)
		res += calcAvarageDelayTimeInAgent(g.descendances[i]);

	res /= g.descensantNum;
	return res;
}

double Statistic::calcDispersOfAvaregeDelayTimeInAgent(AgentInfo& agent)
{
	double res = 0;
	double average = calcAvarageDelayTimeInAgent(agent);
	int size = agent.neurons.size();

	for(int i=0; i<size; i++)
	{
		res += pow((agent.neurons[i].neuronParams.delayTime - average),2);
	}
	return sqrt(res/size);
}

double Statistic::calcDispersOfAvarageDelayTimeInGeneration(Generation& g)
{
	double res = 0;
	double av = 0;
	vector<double> avs(g.descensantNum);

	for(int i=0; i<g.descensantNum; ++i)
	{
		avs[i] = calcAvarageDelayTimeInAgent(g.descendances[i]);
		av += avs[i];
	}

	av /= g.descensantNum;

	for(int i=0; i<g.descensantNum; ++i)
		res +=  pow(avs[i] - av,2);

	return sqrt(res/g.descensantNum);
}

int Statistic::getMaxFitnDescendantNum(Generation& g)
{
	double max = 0;
	int num = 0;
	for(int i=0; i<g.descensantNum; i++)
	{
		if(g.descendances[i].fitness > max)
		{
			max = g.descendances[i].fitness;
			num = i;
		}
	}
	return num;
}

/*Save some parameters for current generation*/
void Statistic::saveResults(Generation& g)
{
	avWeight.push_back(calcAvarageWeight(g));
	fitnessDiversity.push_back(calcFitnessDiversity(g));

	avFitness.push_back(calcAverageFitness(g));
	maxFitness.push_back(calcMaxFitness(g));
	
	maxNumOfNeurons.push_back(calcMaxNumOfNeurons(g));
	minNumOfNeurons.push_back(calcMinNumOfNeurons(g));
	avNumOfNeurons.push_back(calcAvarageNumOfNeurons(g));

	maxNumOfConnections.push_back(calcMaxNumOfConnections(g));
	minNumOfConnections.push_back(calcMinNumOfConnections(g));
	avNumOfConnections.push_back(calcAvarageNumOfConnections(g));

	updateSortAgents(g);

	bestAgentAvDelayTime.push_back(calcAvarageDelayTimeInAgent(g.descendances[sortedAgents[0]]));
	averageAvDelayTime.push_back(calcAvarageDelayTimeInGeneration(g));

	dispersOfBestAgentAvDelayTime.push_back(calcDispersOfAvaregeDelayTimeInAgent(g.descendances[sortedAgents[0]]));
	dispersOfAverageAvDelayTime.push_back(calcDispersOfAvarageDelayTimeInGeneration(g));

	bestAgentNetConsistency.push_back(g.descendances[sortedAgents[0]].netwConsistency);
	bestAgentResultTime.push_back(g.descendances[sortedAgents[0]].timeOfAchievingResults);
	avNetConsistency.push_back(calcAvNetConsistency(g));
	avResultTime.push_back(calcAvarageResultTime(g));
}

Statistic& Statistic::operator+=(const Statistic& stat) throw(StatisticError){
	if(stat.size() != size())
		throw StatisticError("Trying to add not equal statistic");

	for(int i=0; i<stat.size(); ++i){
		if(i < stat.avWeight.size())
			avWeight[i] += stat.avWeight[i];

		if(i < stat.fitnessDiversity.size())
			fitnessDiversity[i] += stat.fitnessDiversity[i];

		if(i < stat.avFitness.size())
			avFitness[i] += stat.avFitness[i];
		if(i < stat.maxFitness.size())
			maxFitness[i] += stat.maxFitness[i];

		if(i < stat.maxNumOfNeurons.size())
			maxNumOfNeurons[i] += stat.maxNumOfNeurons[i];
		if(i < stat.minNumOfNeurons.size())
			minNumOfNeurons[i] += stat.minNumOfNeurons[i];
		if(i < stat.avNumOfNeurons.size())
			avNumOfNeurons[i] += stat.avNumOfNeurons[i];

		if(i < stat.maxNumOfConnections.size())
			maxNumOfConnections[i] += stat.maxNumOfConnections[i];
		if(i < stat.minNumOfConnections.size())
			minNumOfConnections[i] += stat.minNumOfConnections[i];
		if(i < stat.avNumOfConnections.size())
			avNumOfConnections[i] += stat.avNumOfConnections[i];

		if(i < stat.bestAgentAvDelayTime.size())
			bestAgentAvDelayTime[i] += stat.bestAgentAvDelayTime[i];
		if(i < stat.averageAvDelayTime.size())
			averageAvDelayTime[i] += stat.averageAvDelayTime[i];

		if(i < stat.dispersOfBestAgentAvDelayTime.size())
			dispersOfBestAgentAvDelayTime[i] += stat.dispersOfBestAgentAvDelayTime[i];
		if(i < stat.dispersOfAverageAvDelayTime.size())
			dispersOfAverageAvDelayTime[i] += stat.dispersOfAverageAvDelayTime[i];

		if(i < stat.bestAgentNetConsistency.size())
			bestAgentNetConsistency[i] += stat.bestAgentNetConsistency[i];
		if(i < stat.bestAgentResultTime.size())
			bestAgentResultTime[i] += stat.bestAgentResultTime[i];
		if(i < stat.avNetConsistency.size())
			avNetConsistency[i] += stat.avNetConsistency[i];
		if(i < stat.avResultTime.size())
			avResultTime[i] += stat.avResultTime[i];
	}
}

Statistic&  Statistic::operator/=(double d){
	if(d == 0)
		throw StatisticError("Trying to divide on zero!");

	for(int i=0; i<size(); ++i){
		if(i < avWeight.size())
			avWeight[i] /= d;
		if(i < fitnessDiversity.size())
			fitnessDiversity[i] /= d;

		if(i < avFitness.size())
			avFitness[i] /= d;
		if(i < maxFitness.size())
			maxFitness[i] /= d;

		if(i < maxNumOfNeurons.size())
			maxNumOfNeurons[i] /= d;
		if(i < minNumOfNeurons.size())
			minNumOfNeurons[i]  /= d;
		if(i < avNumOfNeurons.size())
			avNumOfNeurons[i]  /= d;

		if(i < maxNumOfConnections.size())
			maxNumOfConnections[i] /= d;
		if(i < minNumOfConnections.size())
			minNumOfConnections[i] /= d;
		if(i < avNumOfConnections.size())
			avNumOfConnections[i]  /= d;

		if(i < bestAgentAvDelayTime.size())
			bestAgentAvDelayTime[i] /= d;
		if(i < averageAvDelayTime.size())
			averageAvDelayTime[i] /= d;

		if(i < dispersOfBestAgentAvDelayTime.size())
			dispersOfBestAgentAvDelayTime[i] /= d;
		if(i < dispersOfAverageAvDelayTime.size())
			dispersOfAverageAvDelayTime[i] /= d;

		if(i < bestAgentNetConsistency.size())
			bestAgentNetConsistency[i] /= d;

		if(i < bestAgentResultTime.size())
			bestAgentResultTime[i]  /= d;

		if(i < avNetConsistency.size())
			avNetConsistency[i] /= d;

		if(i < avResultTime.size())
			avResultTime[i] /= d;
	}
}


double Statistic::calcMaxNumOfNeurons(Generation& g)
{
	double max = 0;
	for(int i=0; i<g.descensantNum; i++)
	{
		int nS = g.descendances[i].neurons.size();
		if(nS > max)
			max = nS;
	}
	return max;
}

double Statistic::calcMinNumOfNeurons(Generation& g)
{
	double min = 0;
	for(int i=0; i<g.descensantNum; i++)
	{
		int nS = g.descendances[i].neurons.size();
		if(nS < min || min == 0)
			min = nS;
	}
	return min;
}

double Statistic::calcAvarageNumOfNeurons(Generation& g)
{
	double res = 0;
	for(int i=0; i<g.descensantNum; i++)
	{
		res += (g.descendances[i].neurons.size());
	}
	return res/g.descensantNum;
}

double Statistic::calcMaxNumOfConnections(Generation& g)
{
	double max = 0;
	int conNum = 0;
	for(int i=0; i<g.descensantNum; i++)
	{
		conNum = g.descendances[i].neurons.connectionNum();
		if(conNum > max)
			max = conNum;
	}
	return max;
}

double Statistic::calcMinNumOfConnections(Generation& g)
{
	double min = 0;
	int conNum = 0;
	for(int i=0; i<g.descensantNum; i++)
	{
		conNum = g.descendances[i].neurons.connectionNum();
		if(conNum < min || min==0)
			min = conNum;
	}
	return min;
}

double Statistic::calcAvarageNumOfConnections(Generation& g)
{
	double res = 0;
	for(int i=0; i<g.descensantNum; i++)
		res += g.descendances[i].neurons.connectionNum();
	return res/g.descensantNum;
}

double Statistic::calcMax(vector<double>& vect)
{
	int size = vect.size();
	double max = 0;
	for(int i=0; i<size; i++)
	{
		if(vect[i]>max)
			max = vect[i];
	}
	return max;
}

double Statistic::calcMin(vector<double>& vect)
{
	int size = vect.size();
	double min = vect[0];
	for(int i=0; i<size; i++)
	{
		if(vect[i]<min)
			min = vect[i];
	}
	return min;
}

AgentInfo& Statistic::findBestAgent(Generation& g)
{
	return findBestAgentA(g.descendances);
}

AgentInfo& Statistic::findBestAgentA(vector<AgentInfo>& g)
{
	double max = 0;
	int ind = -1;
	int size = g.size();
	for(int i=0; i<size; i++)
	{
		if(g[i].fitness > max)
		{
			max = g[i].fitness;
			ind = i;
		}
	}
	if(ind != -1)
		return g[ind];
}

void Statistic::updateSortAgents(Generation& g)
{
	int size = g.descendances.size();
	sortedAgents.resize(size);
	double* arr = new double[size];

	for(int i=0; i<size;++i)
	{
		sortedAgents[i] = i;
		arr[i] = g.descendances[i].fitness;
	}
	
	double temp=0;
	int tempInd=0;
	int i,j;

	for(i=1; i<size; ++i)
	{
		temp = arr[i];
		tempInd = sortedAgents[i];
		for(j=i-1; j>=0; j--)
		{
			if(temp > arr[j])
			{
				arr[j+1] = arr[j];
				sortedAgents[j+1] = sortedAgents[j];
			}
			else
				break;
			
		}
		sortedAgents[j+1] = tempInd;
		arr[j+1] = temp;
	}

	delete[] arr;
}