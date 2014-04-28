#ifndef AR_NEURON_H
#define	AR_NEURON_H

#include "Neuron.h"
#include "Constants.h"

class ARNeuron : public Neuron
{
public:
	ARNeuron(void);
	ARNeuron(const NeuronParams& neurParam);

	~ARNeuron(void);

	virtual bool randomAddConnection(int typeOfNeuron, int neuronId, double weight);
	virtual bool randomRemoveConnection(int toId);
	virtual void copyFrom(Neuron& neuron);
};

#endif
