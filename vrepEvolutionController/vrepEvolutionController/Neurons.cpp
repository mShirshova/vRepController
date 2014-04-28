#include "Neurons.h"
#include "NeuronFactory.h"

Neurons::Neurons(void)
{
}

Neurons::Neurons(const Neurons& neurons)
{
	networkParams = neurons.networkParams;
	deleteNeurons();//delete previous Neurons in heap

	for(unsigned int i=0; i<neurons.neuronsPtrs.size(); i++)
	{
		neuronsPtrs.push_back(NeuronFactory::createNeuron(networkParams.neuronType));
		(*neuronsPtrs[i]) = (*neurons.neuronsPtrs[i]);//copying all rest params
	}
}

void Neurons::operator=(const Neurons& Neurons)
{
	networkParams = Neurons.networkParams;
	deleteNeurons();

	int size = Neurons.neuronsPtrs.size();
	for(int i=0; i < size; i++)
	{
		neuronsPtrs.push_back(NeuronFactory::createNeuron(networkParams.neuronType));
		(*neuronsPtrs[i]) = (*Neurons.neuronsPtrs[i]);//copying all rest params
	}
}

int Neurons::size()
{
	return neuronsPtrs.size();
}

int Neurons::getStartID(int neuronType)
{
	int size = neuronsPtrs.size();
	for(int i=0; i < size; i++)
	{
		if(neuronsPtrs[i]->type == neuronType)
			return neuronsPtrs[i]->id;
	}
	return -1;
}

int Neurons::connectionNum()
{
	int num = 0;
	int size = neuronsPtrs.size();
	for(int i=0; i<size; i++)
		num += neuronsPtrs[i]->connections.size();
	return num;
}

int Neurons::arConnectionNum(){
	if(networkParams.neuronType == Constants::AR_NEURON){
		int num = 0;
		int size = neuronsPtrs.size();
		int conNum = 0;
		for(int i=0; i<size; i++){
			conNum = neuronsPtrs[i]->connections.size();
			for(int j=0;j<conNum; ++j){
				if(neuronsPtrs[i]->connections[j].typeOfConnection == Constants::AR_CONNECTION)
					++num;
			}
		}
		return num;
	}
	
	return 0;
}

int Neurons::activateConnectionNum(){
	if(networkParams.neuronType == Constants::AR_NEURON){
		return connectionNum() - arConnectionNum();
	}
	return connectionNum();
}

double Neurons::getMaxDelay(){
	int size = neuronsPtrs.size();
	double max = 0;
	for(int i=0; i<size; i++){
		if(neuronsPtrs[i]->neuronParams.delayTime > max)
			max = neuronsPtrs[i]->neuronParams.delayTime;
	}
	return max;
}

double Neurons::getMinDelay(){
	int size = neuronsPtrs.size();
	double min = neuronsPtrs[0]->neuronParams.delayTime;
	for(int i=1; i<size; i++){
		if(neuronsPtrs[i]->neuronParams.delayTime < min)
			min = neuronsPtrs[i]->neuronParams.delayTime;
	}
	return min;
}

double Neurons::getAvDelay(){
	int size = neuronsPtrs.size();
	double sum = 0;
	for(int i=0; i<size; i++){
		sum += neuronsPtrs[i]->neuronParams.delayTime;
	}
	return sum/size;
}

int Neurons::getEndID(int neuronType)
{
	int size = neuronsPtrs.size();
	bool find = false;
	for(int i=0; i < size; i++)
	{
		if(!find && neuronsPtrs[i]->type == neuronType)
			find = true;
		
		if(find && (i == size - 1 || neuronsPtrs[i+1]->type != neuronType))
			return i;
	}
	return -1;
}

Neuron& Neurons::operator[](int index)
{
	return *neuronsPtrs[index];
}

Neuron& Neurons::getNeuronByID(int id)
{
	int size = neuronsPtrs.size();
	for(int i =0; i<size; i++)
	{
		if(neuronsPtrs[i]->id == id)
			return *neuronsPtrs[i];
	}
}

Neuron& Neurons::getNeuronByPrevID(int id)
{
	int size = neuronsPtrs.size();
	for(int i =0; i<size; i++)
	{
		if(neuronsPtrs[i]->prevId == id)
			return *neuronsPtrs[i];
	}
}

void Neurons::initialize(const NetworkParams& netParams)
{
	networkParams = netParams;
	int layersNum = networkParams.structure.size();//number of "layers"
	int id = 0;
	int jNum = 0;

	for(int i=0; i < layersNum; ++i)
	{
		for(int j=0; j < networkParams.structure[i]; ++j)
		{
			neuronsPtrs.push_back(NeuronFactory::createNeuron(networkParams.neuronType));
			neuronsPtrs[id]->id = id;
			neuronsPtrs[id]->prevId = id;
			neuronsPtrs[id]->neuronParams = networkParams.neuronParams;
			neuronsPtrs[id]->type = networkParams.typeMap[id];
			id++;
		}
	}

	//map<int, int> fbConnections;
	map<int, vector<int>>::iterator iter; 
	int fromId, nType, size;
	vector<int> toIds;

	for(iter = networkParams.connections.begin(); iter != networkParams.connections.end(); iter++)
	{
		fromId = iter->first;
		toIds = iter->second;
		size = toIds.size();
		for(int i=0; i<size; i++)
		{
			//toIds[i] - id if Neuron to whithc connection is
			nType = neuronsPtrs[toIds[i]]->type;//type of Neuron to whitch connection is
			double weigth = Util::generateRandomValue(networkParams.neuronParams.minWeightValue, networkParams.neuronParams.maxWeightValue);
			neuronsPtrs[fromId]->addConnection(nType,toIds[i],weigth,Constants::SIMPLE_CONNECTION);
		}
	}

	for(iter = networkParams.arConnections.begin(); iter != networkParams.arConnections.end(); iter++)
	{
		fromId = iter->first;
		toIds = iter->second;
		size = toIds.size();
		for(int i=0; i<size; i++)
		{
			nType = neuronsPtrs[toIds[i]]->type;//type of Neuron to whitch connection is
			double weigth = Util::generateRandomValue(networkParams.neuronParams.minWeightValue, networkParams.neuronParams.maxWeightValue);
			neuronsPtrs[fromId]->addConnection(nType,toIds[i],weigth,Constants::AR_CONNECTION);
		}
	}
}

void Neurons::updateIDs()
{
	int size = neuronsPtrs.size();
	for(int i = 0; i < size; i++)
		neuronsPtrs[i]->id = i;
}

void Neurons::updatePrevIDs()
{
	int size = neuronsPtrs.size();
	for(int i = 0; i < size; i++)
		neuronsPtrs[i]->prevId = neuronsPtrs[i]->id;
}

void Neurons::updtateConnectionsIDs()
{
	//it is assumed that ids od Neurons are updated, but prevIds are saved
	int size = neuronsPtrs.size();
	int size1 = 0;
	for(int i=0; i<size; i++)
	{
		size1 = neuronsPtrs[i]->connections.size();
		for(int j=0; j<size1; j++)
		{
			if(!neuronsPtrs[i]->connections[j].isNew)
			{
				Neuron& nTo = getNeuronByPrevID(neuronsPtrs[i]->connections[j].neuronId);
				if(nTo.id != nTo.prevId)
					neuronsPtrs[i]->connections[j].neuronId = nTo.id;
			}
			else
				neuronsPtrs[i]->connections[j].isNew = false;
		}
	}
}

void Neurons::cloneNeuron(int id)
{
	Neuron* oldN = neuronsPtrs[id];
	Neuron* newN = NeuronFactory::createNeuron(networkParams.neuronType);
	//copy properties of Neurons
	newN->copyFrom(*oldN);
	neuronsPtrs.insert(neuronsPtrs.begin()+id+1, newN);
	newN->id = id + 1;
	newN->prevId = -1;

	updateIDs();

	//add new connection to Neurons which have connections to old Neuron
	int size = neuronsPtrs.size();
	for(int i=0; i<size; i++)
		neuronsPtrs[i]->cloneConnections(id, id+1, true);

	updtateConnectionsIDs();
	updatePrevIDs();
}

void Neurons::cloneNeurons(vector<int>& ids)
{
	int size = ids.size();
	if(size > 0)
	{
		int id = 0;
		int shift = 0;
		std::sort(ids.begin(), ids.end());

		for(int i=0; i<size; i++)
		{
			id = ids[i];
			Neuron* oldN = neuronsPtrs[id + shift];
			Neuron* newN = NeuronFactory::createNeuron(networkParams.neuronType);
			newN->copyFrom(*oldN);
			neuronsPtrs.insert(neuronsPtrs.begin()+ id + shift + 1, newN);
			newN->id = id + shift + 1;
			newN->prevId = -1;//it is needed to avoid the Neurons with the same prevIds, it violates the work of updtateConnectionsIDs function
			shift++;

			newN->neuronParams.rBound = oldN->neuronParams.rBound;
			oldN->neuronParams.rBound = oldN->neuronParams.lBound + (oldN->neuronParams.rBound - oldN->neuronParams.lBound)/2;
			newN->neuronParams.lBound = oldN->neuronParams.rBound;
		}

		updateIDs();

		//add new connection to Neurons which have connections to old Neurons
		int size1 = neuronsPtrs.size();
		for(int i=0; i<size1; i++)
		{
			shift = 0;
			for(int j = 0; j < size; j++)
			{
				id = ids[j];
				//true set the property isNew of connection to true, it helps to avoid the updation the id of this connection
				//(so, as the updation is done by prevIds it finds wrong Neuron, because new Neuron has prevId = -1 )
				neuronsPtrs[i]->cloneConnections(id, id+shift+1, true);
				shift++;
			}
		}

		updtateConnectionsIDs();
		updatePrevIDs();
	}
}

bool Neurons::removeNeuron(int id)
{
	int size = neuronsPtrs.size();

	if(id > 0 && id < size)
	{
		neuronsPtrs.erase(neuronsPtrs.begin()+id, neuronsPtrs.begin()+id+1);
		updateIDs();

		//remove connections to deleted Neurons from other Neurons
		//and update connections Ids 
		int size = neuronsPtrs.size();

		for(int i=0; i<size; i++)
			neuronsPtrs[i]->removeConnections(id);
		
		updtateConnectionsIDs();
		updatePrevIDs();
		return true;
	}
	return false;
}

void Neurons::removeNeurons(vector<int>& ids)
{
	int size = ids.size();
	if(size > 0)
	{
		int id = 0;
		int shift = 0;
		std::sort(ids.begin(), ids.end());

		for(int i=0; i<size; i++)
		{
			id = ids[i]-shift;
			neuronsPtrs.erase(neuronsPtrs.begin()+ id, neuronsPtrs.begin()+ id + 1);
			shift++;
		}

		updateIDs();

		int size1 = neuronsPtrs.size();
		for(int i=0; i<size1; i++)
		{
			shift = 0;
			for(int j = 0; j < size; j++)
			{
				id = ids[j];
				neuronsPtrs[i]->removeConnections(id);
				shift++;
			}
		}

		updtateConnectionsIDs();
		updatePrevIDs();
	}
}

//id - Neuron id to witch connections must be added
void Neurons::addConnection(int id)
{
	int toId = Util::generateRandomIntegerValue(0, neuronsPtrs.size());
	neuronsPtrs[id]->randomAddConnection(neuronsPtrs[toId]->type,toId,Util::generateRandomValue(networkParams.neuronParams.minWeightValue, networkParams.neuronParams.maxWeightValue));
}

void Neurons::removeConnection(int id)
{
	neuronsPtrs[id]->randomRemoveConnection();
}

void Neurons::addConnections(vector<int>& ids)
{
	int size = ids.size();
	for(int i=0; i<size; i++)
	{
		int toId = Util::generateRandomIntegerValue(0, neuronsPtrs.size());
		neuronsPtrs[ids[i]]->randomAddConnection(neuronsPtrs[toId]->type,toId,Util::generateRandomValue(networkParams.neuronParams.minWeightValue, networkParams.neuronParams.maxWeightValue));
	}
}

void Neurons::removeConnections(vector<int>& ids)
{
	int size = ids.size();
	for(int i=0; i<size; i++)
		neuronsPtrs[ids[i]]->randomRemoveConnection();
}

void Neurons::deleteNeurons()
{
	int size = neuronsPtrs.size();
	for(int i=0; i<size; i++)
		delete neuronsPtrs[i];
	neuronsPtrs.resize(0);
}

Neurons::~Neurons(void)
{
	deleteNeurons();
}
