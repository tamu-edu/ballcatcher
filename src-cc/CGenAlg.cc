#include "CGenAlg.h"

CGenAlg::CGenAlg(int popsize,
				double MutRat,
				int numWeights,
				//vector<int> splits,
				int mode):m_iPopSize(popsize),
						m_dMutationRate(MutRat),
						m_dMutationMagnitude(CParams::dMutationMagnitude),
						m_iChromoLength(numWeights),
						m_dTotalFitness(0),
						m_iGeneration(0),
						m_iFittestGenome(0),
						m_dBestFitness(0),
						m_iBestCatch(0),
						m_dWorstFitness(9999999),
						m_dAvgFitness(0),
						m_dAvgCatch(0),
						//m_vecSplitPoints(splits),
						m_dRetainedAvgCatch(0),
						m_dRetainedAvgFitness(0),
						//m_dBreadUpperThreshold(CParams::dBreadUpperThresh),
						//m_dBreadLowerThreshold(CParams::dBreadLowerThresh),
						m_dBreadThreshold(CParams::dBreadThresh),
						m_iROG(int(floor(CParams::dROG*CParams::iNumDroppers)))
{
	srand((unsigned int)time(0));
	for(int i=0;i<m_iPopSize;++i) {
		if(CParams::iMode>0) {
			m_vecPop.push_back(SGenome());
			vector<double> tmpVec = ReadFile();
			
			for(int j=0;j<(int)tmpVec.size()-1;++j)
				m_vecPop[i].vecWeights.push_back(tmpVec[j]);
			
			//m_vecPop[i].dBreadUpperThreshold = tmpVec[tmpVec.size()-2];
			//m_vecPop[i].dBreadLowerThreshold = tmpVec[tmpVec.size()-1];
			m_vecPop[i].dBreadThreshold = tmpVec[tmpVec.size()-1];
		}
		else {
			m_vecPop.push_back(SGenome());
			for(int j=0;j<m_iChromoLength;++j)
				m_vecPop[i].vecWeights.push_back(RandomClamped());
			//m_vecPop[i].vecWeights.push_back(RandomRanged(CParams::dMaxWeight));
	  
			//m_vecPop[i].dBreadUpperThreshold = RandomClamped();
			//m_vecPop[i].dBreadLowerThreshold = RandomClamped();
			
			m_vecPop[i].dBreadThreshold = RandomClamped();
			//m_vecPop[i].dBreadThreshold = 0.0;
		}

		fitnessVec.push_back(fitVec);
		catchVec.push_back(catVec);
	}
}

CGenAlg::~CGenAlg() {
	for(int i=0;i<(int)m_vecPop.size();++i)
		m_vecPop[i].vecWeights.clear();
	m_vecPop.clear();
	m_vecSplitPoints.clear();
	for(int i=0;i<(int)fitnessVec.size();++i)
		fitnessVec[i].clear();
	for(int i=0;i<(int)catchVec.size();++i)
		catchVec[i].clear();
}

void CGenAlg::Mutate(SGenome &gene, double MutationScale) {
	for(int i=0;i<(int)gene.vecWeights.size();++i) {
		if(RandFloat()<m_dMutationRate) {
			gene.vecWeights[i] += (RandomClamped()*MutationScale*m_dMutationMagnitude);
			/*if(gene.vecWeights[i]>CParams::dMaxWeight)
				gene.vecWeights[i] = CParams::dMaxWeight;
			else if(gene.vecWeights[i]<CParams::dMinWeight)
				gene.vecWeights[i] = CParams::dMinWeight;*/
		}
	}

	/*if(RandFloat()<m_dMutationRate) {
		do {
			gene.dBreadUpperThreshold += (RandomClamped()*MutationScale*m_dMutationMagnitude);
		}while(gene.dBreadUpperThreshold<=-1.0&&gene.dBreadUpperThreshold>=1.0);
	}
	if(RandFloat()<m_dMutationRate) {
		do {
			gene.dBreadLowerThreshold += (RandomClamped()*MutationScale*m_dMutationMagnitude);
		}while(gene.dBreadLowerThreshold<=-1.0&&gene.dBreadLowerThreshold>=1.0);
	}*/

	////////////////////////////////////////
	if(RandFloat()<m_dMutationRate) {
		//do {
			gene.dBreadThreshold += (RandomClamped()*MutationScale*m_dMutationMagnitude);
		//}while(gene.dBreadThreshold<=-1.0&&gene.dBreadThreshold>=1.0);
	}
	////gene.dBreadThreshold = 0.0;
}

void CGenAlg::Crossover(SGenome &mum, SGenome &dad, SGenome &baby) {
	int cp = RandInt(0, m_iChromoLength-2);
	int i;

	for(i=0;i<cp;++i) {
		baby.vecWeights.push_back(mum.vecWeights[i]);
	}
	for(i=cp;i<(int)mum.vecWeights.size();++i) {
		baby.vecWeights.push_back(dad.vecWeights[i]);
	}
	if(RandFloat()<0.5)
		baby.dBreadThreshold = mum.dBreadThreshold;
	else
		baby.dBreadThreshold = dad.dBreadThreshold;
}

void CGenAlg::Crossover(SGenome &mum, SGenome &dad, SGenome &baby1, SGenome &baby2) {
	int cp = RandInt(0, m_iChromoLength-2);
	int i;

	for(i=0;i<cp;++i) {
		baby1.vecWeights.push_back(mum.vecWeights[i]);
		baby2.vecWeights.push_back(dad.vecWeights[i]);
	}
	for(i=cp;i<(int)mum.vecWeights.size();++i) {
		baby1.vecWeights.push_back(dad.vecWeights[i]);
		baby2.vecWeights.push_back(mum.vecWeights[i]);
	}
	if(RandFloat()<0.5)
		baby1.dBreadThreshold = mum.dBreadThreshold;
	else
		baby1.dBreadThreshold = dad.dBreadThreshold;
	if(RandFloat()<0.5)
		baby2.dBreadThreshold = mum.dBreadThreshold;
	else
		baby2.dBreadThreshold = dad.dBreadThreshold;

	
	/*if(RandFloat()<0.5)
		baby.dBreadUpperThreshold = mum.dBreadUpperThreshold;
	else
		baby.dBreadUpperThreshold = dad.dBreadUpperThreshold;
	if(RandFloat()<0.5)
		baby.dBreadLowerThreshold = mum.dBreadLowerThreshold;
	else
		baby.dBreadLowerThreshold = dad.dBreadLowerThreshold;*/
}

vector<SGenome> CGenAlg::Epoch(vector<SGenome> &old_pop) {
	m_dRetainedAvgCatch = 0;
	m_dRetainedAvgFitness = 0;
	m_vecPop = old_pop;
	Reset();

	sort(m_vecPop.begin(), m_vecPop.end());
	
					//vector<double> tmpDVec;
					/*for(int i=0;i<(int)m_vecPop.size();++i) {
						cout<< m_vecPop[i].dFitness<<"\n";
					}*/
					//std::string wfname;
					//char* cwfname;
					//wfname = "testsort.dat";
					//cwfname = new char[wfname.size()];
					//strcpy(cwfname, wfname.c_str());
					//WriteOutput(cwfname, tmpDVec, true);
					//delete cwfname;
					//exit(1);
	

	CalculateBestWorstAvgTot();

	StoreFitAndCatch();

	vector<SGenome> vecNewPop;

	int NumRetain = int(floor((double)CParams::iNumDroppers*CParams::dRetentionRate));
	if(NumRetain<2)
		NumRetain = 2;
	
	vector<double> slots, sqslots;
	int i, dadID, mumID;
	vector<int> parentVec;
	double fitSum, sqfitSum, currentSum, roulettePointer;
	fitSum = sqfitSum = 0.0;
	for(i=0;i<(int)m_vecPop.size();++i) {
		//fitSum += m_vecPop[i].dFitness;
		sqfitSum += sqrt(m_vecPop[i].dFitness);
	}
	for(i=0;i<(int)m_vecPop.size();++i) {
		if(i==0) {
			//slots.push_back(m_vecPop[i].dFitness/fitSum);
			sqslots.push_back(sqrt(m_vecPop[i].dFitness)/sqfitSum);
		}
		else {
			//slots.push_back(m_vecPop[i].dFitness/fitSum+slots[i-1]);
			sqslots.push_back(sqrt(m_vecPop[i].dFitness)/sqfitSum+sqslots[i-1]);
		}
	}

	roulettePointer = -1.0;
	dadID = mumID = -1;
	
	while((int)vecNewPop.size()<m_iPopSize) {
		currentSum = 0.0;
		roulettePointer = RandFloat();
		//cout<<"current roulettePointer="<<roulettePointer<<"\n";
		if(CParams::iGAType==0) {					// Normal Reproduction
			for(i=0;i<(int)sqslots.size();++i) {
				if(roulettePointer<sqslots[i]) {
					dadID = i;
					break;
				}
			}
			//cout<<"dadID="<<dadID<<"\n";
			do {
				roulettePointer = RandFloat();
				for(i=0;i<(int)sqslots.size();++i) {
					if(roulettePointer<sqslots[i]) {
						mumID = i;
						break;
					}
				}
			}while(dadID==mumID);
		
			SGenome dad = m_vecPop[dadID];
			SGenome mum = m_vecPop[mumID];
			
			SGenome baby1, baby2;
			Crossover(dad, mum, baby1, baby2);
			vecNewPop.push_back(baby1);
			vecNewPop.push_back(baby2);
		}
		else if(CParams::iGAType==1) {				// Asexual Reproduction
			for(i=0;i<(int)sqslots.size();++i) {
				if(roulettePointer<sqslots[i]) {
					dadID = i;
					break;
				}
			}
		}
		else {
			cout<<"Wrong GA Type!!"<<endl;
			exit(1);
		}

		//SGenome baby;
		//Crossover(dad, mum, baby);
		//vecNewPop.push_back(baby);
	}

	// Mutation
	double MutationScale = 0.1;
	for(i=0;i<(int)vecNewPop.size();++i) {
		Mutate(vecNewPop[i], MutationScale);
	}

	vector<SGenome> Elites;
	GrabNBest(NumRetain, Elites);
	for(i=0;i<(int)Elites.size();++i) {
		m_dRetainedAvgCatch += Elites[i].iCatch;
		m_dRetainedAvgFitness += Elites[i].dFitness;
	}
	m_dRetainedAvgCatch = m_dRetainedAvgCatch/Elites.size();
	m_dRetainedAvgFitness = m_dRetainedAvgFitness/Elites.size();
	//////////////////////////////////////////////////////////
	/*
	vector<SGenome> Parents, Children;
	//GrabNBest(NumRetain, Parents);
	Roulette(NumRetain, Parents);
	int i;

	for(i=0;i<(int)Parents.size();++i) {
		m_dRetainedAvgCatch += Parents[i].iCatch;
		m_dRetainedAvgFitness += Parents[i].dFitness;
	}
	m_dRetainedAvgCatch = m_dRetainedAvgCatch/Parents.size();
	m_dRetainedAvgFitness = m_dRetainedAvgFitness/Parents.size();

	int dadID, mumID, csize;
	csize = (int)Children.size();
	while(csize<m_iPopSize-NumRetain-m_iROG) {
		dadID = 0;
		mumID = int(floor(RandFloat()*((double)NumRetain - numeric_limits<double>::epsilon())));
		do {
			dadID = int(floor(RandFloat()*((double)NumRetain - numeric_limits<double>::epsilon())));
		}while(mumID==dadID);

		SGenome mum = Parents[mumID];
		SGenome dad = Parents[dadID];
		SGenome baby;

		if(mum.vecWeights.size()==dad.vecWeights.size())
			Crossover(mum, dad, baby);
		else {
			cout << "Different parent size!!" << endl;
			exit(1);
		}
		Children.push_back(baby);
		csize++;
	}

	for(i=0;i<m_iROG;++i) {
		SGenome randBaby = SGenome();
		for(int j=0;j<m_iChromoLength;++j)
			randBaby.vecWeights.push_back(RandomClamped());
		Children.push_back(randBaby);
	}

	double MutationScale = 0.1;
	for(i=0;i<NumRetain;++i) {
		Mutate(Parents[i], MutationScale);
		vecNewPop.push_back(Parents[i]);
	}

	//MutationScale = 1.0;
	for(i=0;i<(int)Children.size();++i) {
		Mutate(Children[i], MutationScale);
		vecNewPop.push_back(Children[i]);
	}
	*/
	//////////////////////////////////////////////////////////

	m_vecPop = vecNewPop;

	return m_vecPop;
}

void CGenAlg::FitnessScaleRank() {
	const int FitnessMultiplier = 1;
	for(int i=0;i<m_iPopSize;++i)
		m_vecPop[i].dFitness = i*FitnessMultiplier;
	
	CalculateBestWorstAvgTot();
}

void CGenAlg::GrabNBest(int NBest, vector<SGenome> &Pop) {
	while(NBest--)
		Pop.push_back(m_vecPop[(m_iPopSize-1)-NBest]);
}

void CGenAlg::Roulette(int NBest, vector<SGenome> &Pop) {

}

void CGenAlg::StoreFitAndCatch() {
	int i;
	for(i=0;i<m_iPopSize;++i) {
		fitnessVec[i].push_back(m_vecPop[i].dFitness);
		catchVec[i].push_back(m_vecPop[i].iCatch);
	}
}

void CGenAlg::CalculateBestWorstAvgTot() {
	m_dTotalFitness = 0;
	m_iTotalCatch = 0;
	m_iTotalMiss = 0;
	
	double HighestSoFar = 0;
	double LowestSoFar = 999999;
	int MostSoFar = 0;
	int LeastSoFar = 999999;

	for(int i=0;i<m_iPopSize;++i) {
		if(m_vecPop[i].dFitness>HighestSoFar) {
			HighestSoFar = m_vecPop[i].dFitness;
			m_iFittestGenome = i;
			m_dBestFitness = HighestSoFar;
		}
		if(m_vecPop[i].dFitness<LowestSoFar) {
			LowestSoFar = m_vecPop[i].dFitness;
			m_dWorstFitness = LowestSoFar;
		}
		if(m_vecPop[i].iCatch>MostSoFar) {
			MostSoFar = m_vecPop[i].iCatch;
			m_iBestCatch = MostSoFar;
		}
		if(m_vecPop[i].iCatch<LeastSoFar) {
			LeastSoFar = m_vecPop[i].iCatch;
		}
		
		m_dTotalFitness += m_vecPop[i].dFitness;
		m_iTotalCatch += m_vecPop[i].iCatch;
	}
	
	m_dAvgFitness = m_dTotalFitness/m_iPopSize;
	m_dAvgCatch = (double)m_iTotalCatch/m_iPopSize;
}

void CGenAlg::Reset() {
	m_dTotalFitness = 0;
	m_dBestFitness = 0;
	m_dWorstFitness = 999999;
	m_dAvgFitness = 0;
	m_dAvgCatch = 0;
	m_iBestCatch = 0;
}

