#ifndef NEURON_FACTORYS_H
#define	NEURON_FACTORYS_H

#include "Neuron.h"
#include "ARNeuron.h"
#include "Constants.h"
#include "Neurons.h"
#include "NetworkInfo.h"

/*
	Creates the objects of neurons the classes of which correspondes 
	the type of network. It retrurnes the pointer on the base Neuron class to provide
	the independence the other classes from the type of neuron
	Also it creates the informational structure (about layers and connections) for different types of network
*/
namespace NeuronFactory
{
	Neuron* createNeuron(int neuronType);
	void createNetworkParams(NetworkParams& params);
	void createNeuronParams(NeuronParams& params);

	void createTreeLayerNetStrucuture(NetworkParams& params);
	void createFullyConnectedNetStrucuture(NetworkParams& params);
	void createFeedForwardNetStrucuture(NetworkParams& params);
	void createJordanNetworkStrucuture(NetworkParams& params);
	void createStructure(NetworkParams& params);

	void setJointsNums(Neurons& neurons);
	void setDelaysNum(Neurons& neurons);
	void setTransmissionFunc(Neurons& neurons);
	void setJointNumsForJordanNet(Neurons& neurons);
	void addInhibitoryConnectionsToOutpputNeurons(Neurons& neurons);
	void addBackConnectionsToOutputNeurons(Neurons& neurons);
};

#endif

