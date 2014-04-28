#include "Connection.h"


Connection::Connection(void)
{
}

Connection::Connection(int _typeOfNeuron, int _neuronId, double _weight, int _typeOfConnection,  bool _isNew)
{
	typeOfConnection = _typeOfConnection;
	neuronId = _neuronId;
	typeOfNeuron = _typeOfNeuron;
    weight = _weight;
	isNew = _isNew;
}

Connection::~Connection(void)
{
}
