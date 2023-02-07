#include "CNeuralNet.h"

//SNeuron::SNeuron(int NumInputs):m_NumInputs(NumInputs) {	// Without bias
SNeuron::SNeuron(int NumInputs):m_NumInputs(NumInputs+1) {	// With bias
	for(int i=0;i<m_NumInputs;++i)
		m_vecWeights.push_back(RandomClamped());
		//m_vecWeights.push_back(RandomRanged(CParams::dMaxWeight));
}

SNeuronLayer::SNeuronLayer(int NumNeurons, int NumInputsPerNeuron):m_NumNeurons(NumNeurons) {
	for(int i=0;i<m_NumNeurons;++i)
		m_vecNeurons.push_back(SNeuron(NumInputsPerNeuron));
}

CNeuralNet::CNeuralNet() {
	m_NNType = CParams::iNNType;
	m_MemoryOrder = CParams::iMemoryOrder;
	m_NumInputs = CParams::iNumInputs;
	m_NumHiddens1 = CParams::iNumHiddens1;
	m_NumHiddens2 = CParams::iNumHiddens2;
	m_NumOutputs = CParams::iNumOutputs;
	m_NumLayers = CParams::iNumLayers;

	switch(m_NNType) {
		case 0:	// ff
			break;
		case 1:	// recurr
			m_NumInputs += m_NumHiddens1*m_MemoryOrder;
			break;
		case 2:	// bread crumb
			m_NumInputs += 2;
			m_NumOutputs++;
			break;
		case 3: // recurrent bread crumb
			m_NumInputs += m_NumHiddens1*m_MemoryOrder;
			m_NumInputs += 2;
			m_NumOutputs++;
		default:
			break;
	}
	
	CreateNet();

	
}

void CNeuralNet::CreateNet() {
	m_vecLayers.push_back(SNeuronLayer(m_NumHiddens1, m_NumInputs));
	//m_vecLayers.push_back(SNeuronLayer(m_NumHiddens2, m_NumHiddens1));
	//m_vecLayers.push_back(SNeuronLayer(m_NumOutputs, m_NumHiddens2));
	m_vecLayers.push_back(SNeuronLayer(m_NumOutputs, m_NumHiddens1));

	int i, j;

	if(m_NNType==1||m_NNType==3) {						// For 'recurrent' OR 'recurrent w/ breadcrumbs' agents,
		for(i=0;i<m_MemoryOrder;++i) {					// recurrent connections from the 1st hiddens to inputs
			for(j=0;j<m_NumHiddens1;++j)				// as many times as the 'memory order'
				m_vecMemory.push_back(0.0);
		}
	}
}

vector<double> CNeuralNet::GetWeights() const {
	vector<double> weights;

	for(int i=0;i<CParams::iNumLayers;++i) {
		for(int j=0;j<m_vecLayers[i].m_NumNeurons;++j) {
			for(int k=0;k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs;++k)
				weights.push_back(m_vecLayers[i].m_vecNeurons[j].m_vecWeights[k]);
		}
	}
	return weights;
}

void CNeuralNet::PushMemory(vector<double> NewHidden) {			// Update agent's memory
	vector<double> tmpVec;
	for(int i=0;i<(int)m_vecMemory.size();++i) {
		if(i<(int)m_vecMemory.size()-m_NumHiddens1)				// restore '(memory_order-1)*first_hidden' size of memory
			tmpVec.push_back(m_vecMemory[i+m_NumHiddens1]*CParams::dMemDecay);
		else													// and take 'first_hidden' size of new memory
			tmpVec.push_back(NewHidden[i-m_vecMemory.size()+m_NumHiddens1]*CParams::dMemDecay);
	}
	m_vecMemory = tmpVec;
}

void CNeuralNet::PutWeights(vector<double> &weights) {			// Dispatch weights in the form of chromosome to network
	int cWeight = 0;
	for(int i=0;i<m_NumLayers;++i) {										// loop as many as the number of layers
		for(int j=0;j<m_vecLayers[i].m_NumNeurons;++j) {		// loop as many as the number of edges in each layer
			for(int k=0;k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs;++k)
			{
				m_vecLayers[i].m_vecNeurons[j].m_vecWeights[k] = weights[cWeight++];
		
			}
		}
	}
}

int CNeuralNet::GetNumberOfWeights() const {
	int weights = 0;
	for(int i=0;i<m_NumLayers;++i) {
		for(int j=0;j<m_vecLayers[i].m_NumNeurons;++j) {
			for(int k=0;k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs;++k)
				weights++;
		}
	}

	return weights;
}

vector<double> CNeuralNet::Update(vector<double> &inputs) {
	vector<double> outputs;
	int cWeight = 0;
	int i;

	if(m_NNType==1||m_NNType==3) {
		for(i=0;i<(int)m_vecMemory.size();++i)
			inputs.push_back(m_vecMemory[i]);
	}
	if(CParams::iMode>0)
		WriteOutput("input_state.dat", inputs, false);
	
	for(i=0;i<m_NumLayers;++i) {
		if(i>0)
			inputs = outputs;

		if((m_NNType==1||m_NNType==3)&&i==1)
			PushMemory(outputs);

		outputs.clear();
		cWeight = 0;

		for(int j=0;j<m_vecLayers[i].m_NumNeurons;++j) {
			double netinput = 0;
			int NumInputs = m_vecLayers[i].m_vecNeurons[j].m_NumInputs;
			
			//for(int k=0;k<NumInputs;++k)			// Without bias
			for(int k=0;k<NumInputs-1;++k)			// With bias
				netinput += m_vecLayers[i].m_vecNeurons[j].m_vecWeights[k]*inputs[cWeight++];
			netinput += m_vecLayers[i].m_vecNeurons[j].m_vecWeights[NumInputs-1]*CParams::dBias;
			
			//outputs.push_back(tanh(netinput));
			outputs.push_back(Sigmoid(netinput));
			cWeight = 0;
		}
		if(CParams::iMode>0) {
			if(i==0) {
				WriteOutput("hidden_state.dat", outputs, false);
			}
		}
	}

	return outputs;
}

vector<int> CNeuralNet::CalculateSplitPoints() const {
	vector<int> SplitPoints;
	int WeightCounter = 0;

	for(int i=0;i<m_NumLayers;++i) {
		for(int j=0;j<m_vecLayers[i].m_NumNeurons;++j) {
			for(int k=0;k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs;++k)
				++WeightCounter;
			SplitPoints.push_back(WeightCounter-1);
		}
	}
	
	return SplitPoints;
}
