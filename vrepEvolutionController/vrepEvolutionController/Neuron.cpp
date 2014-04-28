#include "Neuron.h"

Neuron::Neuron(void)
{
}

Neuron::Neuron(const NeuronParams& neurParam)
{
	neuronParams = neurParam;
}

Neuron::~Neuron(void)
{
}

void Neuron::copyFrom(Neuron& Neuron)
{
	(*this) = Neuron;
}

/*Clone the connection to the Neuron with id = toIdOld (if Neuron has it) and connect it to the Neuron with id = toIdNew*/
bool Neuron::cloneConnections(int toIdOld, int toIdNew, bool isNew)
{
	bool isCloned = false;
	int size = connections.size();
	for(int i=0; i<size; i++)
	{
		//check if need to add connection to new Neuron
		if(connections[i].neuronId == toIdOld && !connections[i].isNew)
		{
			connections.push_back(connections[i]);
			connections[connections.size()-1].neuronId = toIdNew;
			connections[connections.size()-1].isNew = isNew;
			//divide weights to old and new newrons halve
			connections[i].weight /= 2;
			connections[connections.size()-1].weight /= 2;
			isCloned = true;
		}
	}
	return isCloned;
}

/*
	remove connection of determ type (typeOfConnect) to the Neuron with id=toId 
	if typeOfConnect = -1 function simply delete the first connection in array with id=toId
*/
bool Neuron::removeConnection(int toId,  int typeOfConnect)
{
	vector<Connection>::iterator iter;
	for(iter = connections.begin(); iter != connections.end(); iter++)
	{
		if((*iter).neuronId == toId && (typeOfConnect == -1 || (*iter).typeOfConnection == typeOfConnect))
			break;
	}
	if(iter != connections.end())
	{
		connections.erase(iter);
		return true;
	}
	return false;
}

/*
	remove connections of all types to the Neuron with id = toId
	One Neuron can have more than one connection to the Neuron with the same id
*/
bool Neuron::removeConnections(int toId)
{
	bool erased = false;
	vector<Connection>::iterator iter;

	while(removeConnection(toId, -1))
	{
		erased = true;
	}
	return erased;
}

bool Neuron::randomRemoveConnection()
{
	int size = connections.size();
	if(size > 0)
	{
		int toId = Util::generateRandomIntegerValue(0,size);
		connections.erase(connections.begin()+toId);
		return true;
	}
	return false;
}

bool Neuron::hasConnection(int typeOfNeuron, int neuronId, int typeOfConnect)
{
	vector<Connection>::iterator iter;

	for(iter = connections.begin(); iter != connections.end(); iter++)
	{
		if((*iter).typeOfConnection == typeOfConnect && (*iter).neuronId == neuronId && (*iter).typeOfNeuron == typeOfNeuron)
			return true;
	}
	
	return false;
}

bool Neuron::addConnection(int typeOfNeuron, int neuronId, double weight, int typeOfConnect)
{
	if(!hasConnection(typeOfNeuron,neuronId,typeOfConnect))
	{
		connections.push_back(Connection(typeOfNeuron,neuronId,weight,typeOfConnect));
		return true;
	}
	return false;
}

//random add the connection of some type (if it more than one) to the Neuron with id = neuronId
//this base class add only the simple connections but its derivetivse can add connection of other types
bool Neuron::randomAddConnection(int typeOfNeuron, int neuronId, double weight)
{
	return addConnection(typeOfNeuron, neuronId, weight, Constants::SIMPLE_CONNECTION);
}
