#ifndef CDROPPER_H
#define CDROPPER_H

#include <vector>
#include <math.h>
#include <limits>
#include <string>
#include <sstream>

#include "CNeuralNet.h"
#include "utils.h"
#include "CParams.h"
#include "CObject.h"

using namespace std;

class CDropper {
private:
	std::string wfname;
	CNeuralNet m_ItsBrain;
	int m_NNType;
	double m_dPosition;
	double m_dSpeed;
	int m_iNumRays;
	int m_iRayInterval;
	int m_iRayLength;
	double m_dFitness;
	int m_iCatch;
	vector<int> vCatch;
	int m_iMiss;
	vector<double> GetInputs(double pos,
							vector<CObject*> &balls,
							vector<CObject*> &breads);
	double GetDistance(double pos, int RayIndex, CObject* ball);
	vector<double> GetBreadDistances(double pos, vector<CObject*> &breads);
	//double m_dBreadUpperThreshold;
	//double m_dBreadLowerThreshold;
	double m_dBreadThreshold;
	void ThrowBread(double pos, vector<CObject*> &breads);
	void GatherBread(double prevpos, double curpos, vector<CObject*> &breads);
	vector<double> m_dDistances;
	vector<double> inputs;
	vector<double> outputs;
	double m_dPosDifference;
	double m_dMaxInput;

public:
	CDropper();
	~CDropper();

	bool Update(vector<CObject*> &balls, vector<CObject*> &breads);
	int CatchBall(vector<CObject*> &balls, double size);
	void Reset();
	void AllReset();
	inline double Position() {return m_dPosition;}
	inline void AddFitness(double fit) {m_dFitness = m_dFitness*fit;}
	inline void AddCatch() {m_iCatch++;}
	inline void AddMiss() {m_iMiss++;}
	inline double Fitness() const {return m_dFitness;}
	inline int Catch() const {return m_iCatch;}


	void PutWeights(vector<double> &w);
	void PutWeights(vector<double> &w, double bth);
	//void PutWeights(vector<double> &w, double bth1, double bth2);

	inline int GetNumberOfWeights() const {return m_ItsBrain.GetNumberOfWeights();}
	inline vector<int> CalculateSplitPoints() const {return m_ItsBrain.CalculateSplitPoints();}
	inline vector<double> GetDistances() const {return m_dDistances;}
	inline vector<double> GetInputs() const {return inputs;}
	inline vector<double> GetOutputs() const {return outputs;}
	inline vector<double> GetNetworkWeights() const {return m_ItsBrain.GetWeights();}
	inline double GetDifference() const {return m_dPosDifference;}
	//inline double GetUpperThreshold() const {return m_dBreadUpperThreshold;}
	//inline double GetLowerThreshold() const {return m_dBreadLowerThreshold;}
	inline double GetThreshold() const {return m_dBreadThreshold;}
};

#endif
