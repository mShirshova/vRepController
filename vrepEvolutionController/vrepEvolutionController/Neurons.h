#ifndef NEURONS_H
#define	NEURONS_H

#include <vector>
#include <map>

#include "Util.h"
#include "LOG.h"
#include "Neuron.h"
#include "NetworkInfo.h"

class Neuron;

/*
	It is container for Neurons which rule the deletion, addition Neurons and connections, creation 
	the network structure e.t.c.
*/
class Neurons
{
public:
	Neurons(void);
	Neurons(const Neurons& neurons);
	~Neurons(void);

	NetworkParams networkParams;
	vector<Neuron*> neuronsPtrs;

	void initialize(const NetworkParams& netParams);
	void cloneNeuron(int id);
	void cloneNeurons(vector<int>& ids);
	bool removeNeuron(int id);
	void removeNeurons(vector<int>& ids);
	void addConnection(int id);
	void removeConnection(int id);
	void addConnections(vector<int>& ids);
	void removeConnections(vector<int>& ids);
	int size();
	int connectionNum();
	int arConnectionNum();
	int activateConnectionNum();
	double getMaxDelay();
	double getMinDelay();
	double getAvDelay();
	int getStartID(int neuronType = Constants::INTERM_NEURON_TYPE);
	int getEndID(int neuronType = Constants::INTERM_NEURON_TYPE);
	void updateIDs();
	void updatePrevIDs();
	void updtateConnectionsIDs();

	Neuron& operator[](int index);
	void operator=(const Neurons& Neurons);
	Neuron& getNeuronByID(int id);//id equal index in - NeuronS& operator[](int index);
	Neuron& getNeuronByPrevID(int id);
	void deleteNeurons();
};

#endif;

