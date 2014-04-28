#include "ARNeuron.h"


ARNeuron::ARNeuron(void):Neuron()
{

}

ARNeuron::ARNeuron(const NeuronParams& neurParam):Neuron(neurParam)
{
}

void ARNeuron::copyFrom(Neuron& neuron)
{
	(*this) = dynamic_cast<ARNeuron&>(neuron);
}

bool ARNeuron::randomAddConnection(int typeOfNeuron, int neuronId, double weight)
{
	if(Util::generateRandomValue() > 0.5)
		return addConnection(typeOfNeuron, neuronId, weight, Constants::SIMPLE_CONNECTION);
	else
		return addConnection(typeOfNeuron, neuronId, weight, Constants::AR_CONNECTION);
}

bool ARNeuron::randomRemoveConnection(int toId)
{
	if(Util::generateRandomValue() > 0.5)
		return removeConnection(toId, Constants::SIMPLE_CONNECTION);
	else 
		return removeConnection(toId, Constants::AR_CONNECTION);
}

ARNeuron::~ARNeuron(void)
{
}
