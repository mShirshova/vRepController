#ifndef GENERATION_H
#define	GENERATION_H

#include <vector>
#include "AgentInfo.h"
#include "Parameters.h"
#include "Util.h"
#include "LOG.h"
#include "NeuronFactory.h"

using namespace std;

struct probDiapasone
{
	double probability;
	double supr;//bottom value of diapasone
	double inf;//top value of diapasone
};

class Generation
{
public:
	Generation(void);
	~Generation(void);

	int curGeneration;
    int curDescendant;
	int descensantNum;
	int curDescendantNum;
	int generationNum;

	vector<AgentInfo> descendances;

	void reset();
	bool hasNextGenertion();
	bool hasNextDescendant();
	void createNewGeneration(bool createFirst = true);
	AgentInfo* getNextDescendant();
	void calcFitness(AgentInfo& agent);//writes the fitness in curAgent
	void createGenerationFromOneDescendant(AgentInfo& agent);
	void clearDescendants();
	
private:
	vector<AgentInfo> descendances_prev;
	vector<probDiapasone> diapasones;
	int diapasoneSize;

	void calcProbDiapasones(vector<AgentInfo>& descns);
	void createDescendanceWithCrossover(AgentInfo& parent1, AgentInfo& parent2, AgentInfo& child1, 
		                    AgentInfo& child2, bool crossParents, int numOfConnections);
	void createDescendanceMB(AgentInfo& parent, AgentInfo& child);
	void createDescendanceMB_2(AgentInfo& parent, AgentInfo& child);
	void checkInputAndOutputNeurons(vector<int>& ids, Neurons& neurons);

	int calcNumOfConnections(vector<int>& nm);
	AgentInfo* selectDescendant(vector<AgentInfo>& descns);
	void selectRandomNeurons(int startID, int endID, vector<int>& ids, double prob);
	void mutateParams(AgentInfo& child);
	void mutateDelayTimes(AgentInfo& child);
};

#endif;
