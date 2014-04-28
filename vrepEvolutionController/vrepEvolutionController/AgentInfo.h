#ifndef AGENTINFO_H
#define	AGENTINFO_H

#include "Neurons.h"

using namespace std;

class AgentInfo
{
public:
	AgentInfo(void);
	AgentInfo(const AgentInfo& agInf);

	~AgentInfo(void);

	double fitness;
	double activity;
	double distance;
	double netwConsistency;
	double avHeight;
	double timeOfAchievingResults;

	Neurons neurons;

	//must be called befor weights updating
	void initializeNetwork(const NetworkParams& netwParam);
	void clear();

	bool operator<(const AgentInfo& agINf);
	AgentInfo& operator=(const AgentInfo& agINf);
};

#endif

