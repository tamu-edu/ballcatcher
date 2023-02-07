#ifndef CGENALG_H
#define CGENALG_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <limits>
#include <time.h>

#include "utils.h"
#include "CParams.h"

struct SGenome {
	vector<double> vecWeights;
	double dFitness;
	//double dBreadUpperThreshold;
	//double dBreadLowerThreshold;
	double dBreadThreshold;
	int iCatch;
	int iMiss;
	
	SGenome():dFitness(0),iCatch(0),iMiss(0){}
	SGenome(vector<double> w, double bth):vecWeights(w),
			//dBreadUpperThreshold(bth1),
			//dBreadLowerThreshold(bth2),
			dBreadThreshold(bth),
			dFitness(0),
			iCatch(0),
			iMiss(0)
	{}
	
	friend bool operator< (const SGenome &lhs, const SGenome &rhs) {
		return (lhs.dFitness<rhs.dFitness);
	}
};

class CGenAlg {
private:
	vector<SGenome> m_vecPop;
	int m_iPopSize;
	int m_iChromoLength;
	vector<int> m_vecSplitPoints;
	double m_dTotalFitness;
	double m_dBestFitness;
	double m_dAvgFitness;
	double m_dWorstFitness;
	int m_iTotalCatch;
	int m_iTotalMiss;
	double m_dAvgCatch;
	double m_dRetainedAvgCatch;
	double m_dRetainedAvgFitness;
	int m_iBestCatch;
	int m_iFittestGenome;
	double m_dMutationRate;
	double m_dMutationMagnitude;
	double m_dCrossoverRate;
	double m_dBreadThreshold;
	int m_iROG;
	int m_iGeneration;
	doubleVec fitVec;
	vector<doubleVec> fitnessVec;
	intVec catVec;
	vector<intVec> catchVec;
	void Crossover(SGenome &mum, SGenome &dad, SGenome &baby);
	void Crossover(SGenome &mum, SGenome &dad, SGenome &baby1, SGenome &baby2);
	void Mutate(SGenome &gene, double scale);
	void GrabNBest(int NBest, vector<SGenome> &vecPop);
	void Roulette(int NBest, vector<SGenome> &vecPop);
	void FitnessScaleRank();
	void CalculateBestWorstAvgTot();
	void StoreFitAndCatch();
	void Reset();

public:
	CGenAlg(int popsize, 
			double MutRat, 
			int numWeights, 
			//vector<int> splits, 
			int mode);
	~CGenAlg();
	vector<SGenome> Epoch(vector<SGenome> &old_pop);
	vector<SGenome> GetChromos() const {return m_vecPop;}
	double AverageFitness() const {return m_dAvgFitness;}
	double BestFitness() const {return m_dBestFitness;}
	double AverageCatch() const {return m_dAvgCatch;}
	int BestCatch() const {return m_iBestCatch;}
	vector<doubleVec> GetFitnesses() const {return fitnessVec;}
	vector<intVec> GetCatches() const {return catchVec;}
	double GetRetainedAvgCatch() const {return m_dRetainedAvgCatch;}
	double GetRetainedAvgFitness() const {return m_dRetainedAvgFitness;}
};

#endif
