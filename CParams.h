#ifndef CPARAMS_H
#define CPARAMS_H

#include <fstream>
#include <iostream>

using namespace std;

class CParams {
public:
	static double dPi;
	static int EnvWidth;
	static int EnvHeight;
	static int iNumInputs;
	static int iNumHiddens1;
	static int iNumHiddens2;
	static int iNumOutputs;
	static int iNumLayers;
	static int iNNType;
	static int iGAType;
	static int iMemoryOrder;
	static double dBias;
	static double dAgentRadius;
	static double dAgentSpeed;
	static int iNumDroppers;
	static int iNumRays;
	static int iRayWidth;
	static double dRayLength;
	static double dBRayLength;
	static int iBallLayout;
	static int iNumBalls;
	static double dBallRadius;
	static double dBallSpeed;
	static double dBreadRadius;
	static int iNumGeneration;
	static double dRetentionRate;
	static double dMutationRate;
	static double dMutationMagnitude;
	static int iMode;
	static int iDisplacement;
	static double dMaxThresh;
	static double dMinThresh;
	//static double dBreadUpperThresh;
	//static double dBreadLowerThresh;
	static double dBreadThresh;
	static double dMaxWeight;
	static double dMinWeight;
	static double dROG;
	static double dBreadSensorDivisor;
	static double dMemDecay;
	//////////////////// delay implemented
	static int iDelayMode;		// 0: no delay, 1: delayed input, 2: delayed hidden, 3: delayed input & hidden
	static int iDelaySize;		// same size of delay at input & hidden (if delayed hidden)

	
	CParams() {
		if(!LoadInParameters("params.ini"))
			cout << "Cannot find .ini file!!" << endl;
		bool LoadInParameters(char* szFileName);
	}

	bool LoadInParameters(char* szFileName);
};

#endif
