#ifndef NEURON_H
#define	NEURON_H

#include <vector>
#include "Connection.h"
#include "Util.h"
#include "LOG.h"
#include "Constants.h"
#include "Neurons.h"
#include "NetworkInfo.h"

class Neuron
{
public:
	Neuron(void);
	Neuron(const NeuronParams& neurParam);
	~Neuron(void);

	vector<Connection> connections;//contain all on connections 

	int id;//num of neuron in the layer
	int prevId;//prev id of neuron if ti chanegs it's position
	int type;

	NeuronParams neuronParams;//witch constant during one life of agent

	virtual void copyFrom(Neuron& neuron);
	virtual bool addConnection(int typeOfNeuron, int neuronId, double weight, int typeOfConnect = Constants::SIMPLE_CONNECTION);
	virtual bool removeConnection(int toId,  int typeOfConnect = -1);
	virtual bool randomAddConnection(int typeOfNeuron, int neuronId, double weight);
	virtual bool randomRemoveConnection();
	virtual bool hasConnection(int typeOfNeuron, int neuronId, int typeOfConnect = Constants::SIMPLE_CONNECTION);
	virtual bool cloneConnections(int toIdOld, int toIdNew, bool isNew = false);
	virtual bool removeConnections(int toId);

};

#endif

