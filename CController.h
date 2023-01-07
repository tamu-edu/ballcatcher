#define  _CRT_SECURE_NO_DEPRECATE

#ifndef CCONTROLLER_H
#define CCONTROLLER_H

#include <vector>
#include <sstream>
#include <string>
#include <math.h>

#include "CDropper.h"
#include "CObject.h"
#include "CGenAlg.h"
#include "utils.h"
#include "CParams.h"

using namespace std;

class CController {
private:
	bool fileWriteBegin;
	vector<SGenome> m_vecThePopulation;
	vector<CDropper> m_vecDroppers;
	vector<CObject*> m_vecBalls;
	vector<CObject*> m_vecBreads;
	
	CGenAlg* m_pGA;
	
	int m_NumGenerations;
	int m_NumDroppers;
	int m_NumBalls;
	int m_NumWeightsInNN;
	int m_Displacement;
	
	vector<int> bDisp;
	vector<double> m_vecAvgFitness;
	vector<double> m_vecBestFitness;
	vector<double> m_vecAvgCatch;
	vector<int> m_vecBestCatch;
	vector<double> m_vecRetainedAvgFitness;
	vector<double> m_vecRetainedAvgCatch;
	
	int m_CountGenerations;
	int m_CountBalls;
	int m_CountDroppers;
	
	bool AllFallen();
	
	void RenewBalls(int mode, int seed);
	void RenewFixedBalls(int mode, int seed);
	void LoadBalls(int idx);
	double maxBest;
	double getMax(vector<double> vec);
	int returnMe(int toreturn);
	void redisplacement(int iseed);
	
	bool flag1;		// for generation index
	bool flag2;		// for dropper index
	bool flag3;		// for ball index
	bool flag4;		// for other outputs at the end
 
public:
	CController();
	~CController();
	
	int Update();
};

#endif
