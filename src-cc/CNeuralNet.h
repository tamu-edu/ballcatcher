#ifndef CNEURALNET_H
#define CNEURALNET_H

#include <vector>
#include <math.h>

#include "utils.h"
#include "CParams.h"

using namespace std;

struct SNeuron {
	int m_NumInputs;
	vector<double> m_vecWeights;

	SNeuron(int NumInputs);
	SNeuron(int NumInputs, double value);
};

struct SNeuronLayer {
	int m_NumNeurons;
	vector<SNeuron> m_vecNeurons;

	SNeuronLayer(int NumNeurons, int NumInputsPerNeuron);
};

class CNeuralNet {
private:
	int m_NNType;
	int m_MemoryOrder;
	int m_NumInputs;
	int m_NumOutputs;
	int m_NumHiddens1;
	int m_NumHiddens2;
	int m_NumLayers;

	vector<SNeuronLayer> m_vecLayers;
	vector<double> m_vecMemory;

public:
	CNeuralNet();
	void CreateNet();
	vector<double> GetWeights() const;
	int GetNumberOfWeights() const;
	void PutWeights(vector<double> &weights);
	vector<double> Update(vector<double> &inputs);
	inline double Sigmoid(double activation) {return (1/(1+exp(-activation)));}
	vector<int> CalculateSplitPoints() const;
	void PushMemory(vector<double> NewHidden);
};

#endif
