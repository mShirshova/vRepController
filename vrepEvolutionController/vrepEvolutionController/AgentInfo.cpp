#include "AgentInfo.h"

AgentInfo::AgentInfo(void)
{
}

AgentInfo::AgentInfo(const AgentInfo& agInf)
{
	fitness = agInf.fitness;
	neurons = agInf.neurons;
	activity = agInf.activity;
	distance = agInf.distance;
	avHeight = agInf.avHeight;
	netwConsistency = agInf.netwConsistency;
	timeOfAchievingResults = agInf.timeOfAchievingResults;
}

/* 
   Create the start network of agent, this function called only once
   netwInfo - containes all information about network
 */
void AgentInfo::initializeNetwork(const NetworkParams& netwParam)
{
	neurons.initialize(netwParam);
}

bool AgentInfo::operator<(const AgentInfo& agInf)
{
	return fitness > agInf.fitness;
}

AgentInfo& AgentInfo::operator=(const AgentInfo& agInf)
{
	fitness = agInf.fitness;
	neurons = agInf.neurons;
	activity = agInf.activity;
	distance = agInf.distance;
	avHeight = agInf.avHeight;
	netwConsistency = agInf.netwConsistency;
	timeOfAchievingResults = agInf.timeOfAchievingResults;

	return *this;
}

void AgentInfo::clear()
{
	fitness = 0;
	activity = 0;
	distance = 0;
	avHeight = 0;
	neurons.deleteNeurons();
}

AgentInfo::~AgentInfo(void)
{
}
