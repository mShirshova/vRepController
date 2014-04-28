#ifndef STATISTIC_H
#define STATISTIC_H

#include "Generation.h"
#include <vector>
#include <exception>
#include <string>

class Statistic
{
public:
	class StatisticError : public logic_error{
	public:
		StatisticError(const std::string& msg = ""):logic_error(msg){
		}
	 };

	 vector<double> avFitness;
	 vector<double> maxFitness;
	 vector<double> maxNumOfNeurons;
	 vector<double> minNumOfNeurons;
	 vector<double> avNumOfNeurons;
	 vector<double> maxNumOfConnections;
	 vector<double> minNumOfConnections;
	 vector<double> avNumOfConnections;

	 vector<double> avNetConsistency;
	 vector<double> bestAgentNetConsistency;
	 vector<double> avResultTime;
	 vector<double> bestAgentResultTime;

	 vector<double> bestAgentAvDelayTime;
	 vector<double> dispersOfBestAgentAvDelayTime;
	 vector<double> averageAvDelayTime;
	 vector<double> dispersOfAverageAvDelayTime;

	 vector<int> sortedAgents;

	 vector<vector<vector<double>>> jointValues;
	 vector<vector<vector<double>>> outNetwValuesValues;
	
	 vector<double> avWeight;
	 vector<double> fitnessDiversity;

	Statistic();
	~Statistic();

	double  calcAvNetConsistency(Generation& g);
	double  calcAvarageResultTime(Generation& g);

	double  calcAverageFitness(Generation& g);
	double  calcFitnessDiversity(Generation& g);

	double  calcAvarageWeight(Generation& g);//for deescendant with max fitness
	double  calcMaxFitness(Generation& g);

	double  calcMaxNumOfNeurons(Generation& g);
	double  calcMinNumOfNeurons(Generation& g);
	double  calcAvarageNumOfNeurons(Generation& g);

	double  calcMaxNumOfConnections(Generation& g);
	double  calcMinNumOfConnections(Generation& g);
	double  calcAvarageNumOfConnections(Generation& g);

	double calcMax(vector<double>& vect);
	double calcMin(vector<double>& vect);
	AgentInfo& findBestAgent(Generation& g);
	AgentInfo& findBestAgentA(vector<AgentInfo>& g);

	double calcAvarageDelayTimeInAgent(AgentInfo& agent);
	double calcAvarageDelayTimeInGeneration(Generation& g);
	double calcDispersOfAvaregeDelayTimeInAgent(AgentInfo& agent);
	double calcDispersOfAvarageDelayTimeInGeneration(Generation& g);

	int  getMaxFitnDescendantNum(Generation& g);

	void saveResults(Generation& g);
	void updateSortAgents(Generation& g);
	void clear();

	int size() const throw(StatisticError){
		return maxFitness.size();
	}

	Statistic& operator+=(const Statistic& stat);
	Statistic& operator/=(double d);
};

#endif

