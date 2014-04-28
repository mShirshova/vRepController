#ifndef CONNECTION_H
#define	CONNECTION_H

#include "Parameters.h"

class Connection
{
public:
	Connection(void);
	Connection(int _typeOfNeuron, int _neuronId, double _weight, int _typeOfConnection = Constants::SIMPLE_CONNECTION, bool _isNew = false);
	~Connection(void);

	int typeOfConnection;//what type of connection
	int typeOfNeuron;//to what type neuron this connection is
    int neuronId;//to what number of neuron this connection is
    double weight;//value of weight
	bool isNew; //indicates that neuron is new
};

#endif

