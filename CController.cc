#include "CController.h"

CController::CController(): m_NumDroppers(CParams::iNumDroppers),
			    m_NumGenerations(CParams::iNumGeneration),
			    m_pGA(NULL),
			    m_NumBalls(CParams::iNumBalls),
			    m_CountGenerations(0),
			    m_CountDroppers(0),
			    m_CountBalls(0),
			    maxBest(0.0),
				flag1(true), flag2(true), flag3(false), flag4(false),
				m_Displacement(CParams::iDisplacement)
{
	if(CParams::iMode==2) {
		m_NumDroppers = 1;
		m_NumGenerations = 1;
		m_NumBalls = 12;
	}
	if(CParams::iMode==1) {
		m_NumDroppers = 1;
		m_NumGenerations = 1;
		m_NumBalls = 200;
	}
	
	fileWriteBegin = false;
	int i;
	for(i=0;i<m_NumDroppers;++i) {
		// (a) These CDropper objects are only required to calculate the number of weights in the neural network
		m_vecDroppers.push_back(CDropper());
	}
	m_NumWeightsInNN = m_vecDroppers[0].GetNumberOfWeights();

	// (b) Actual weights are random-initialized from this CGenAlg object
	m_pGA = new CGenAlg(m_NumDroppers,
						CParams::dMutationRate,
						m_NumWeightsInNN,
						CParams::iMode);
	//printf( "GenAlg [%x] created\n", m_pGA );
	
	m_vecThePopulation = m_pGA->GetChromos();
	
	std::string wfname;
	
	for(i=0;i<m_NumDroppers;++i) {
		m_vecDroppers[i].PutWeights(m_vecThePopulation[i].vecWeights, 
									m_vecThePopulation[i].dBreadThreshold);
	}
	
	redisplacement(m_Displacement);
	
	//if(m_CountBalls<m_NumBalls) {	//////////////
		if(CParams::iMode==1) {
			RenewBalls(CParams::iBallLayout, bDisp[m_CountBalls]);
		//	RenewFixedBalls(CParams::iBallLayout, bDisp[m_CountBalls]);
		}
		else if(CParams::iMode==2) {
			LoadBalls(0);
		}
		else {
			RenewBalls(CParams::iBallLayout, bDisp[m_CountBalls]);
		}
	//}
}

CController::~CController() {
	if(m_pGA) {
		//printf( "GenAlg [%x] deleted\n", m_pGA );
		delete m_pGA;
	}
	m_vecDroppers.clear();
	m_vecAvgFitness.clear();
	m_vecBestFitness.clear();
	m_vecAvgCatch.clear();
	m_vecBestCatch.clear();
	m_vecRetainedAvgFitness.clear();
	m_vecRetainedAvgCatch.clear();
	for(int i=0;i<(int)m_vecThePopulation.size();++i)
		m_vecThePopulation[i].vecWeights.clear();
	m_vecThePopulation.clear();


}

int CController::Update() {
	int i, j, k, BallsCaught, OverallCatch, GenerationCatch, plateau;
	double DistanceNow, Distance3D;
	std::string wfname;

	BallsCaught = OverallCatch = GenerationCatch = plateau = 0;

	if(m_CountGenerations<m_NumGenerations) {
		if(flag1==true) {
			cout<<"Generation # "<<m_CountGenerations<<endl;
			flag1 = false;
		}
		if(m_CountDroppers<m_NumDroppers) {
			if(flag2==true) {
				//cout<<"Dropper # "<<m_CountDroppers<<endl;
				flag2 = false;
			}
			if(m_CountBalls<m_NumBalls) {
				if(!AllFallen()) {
					for(j=0;j<(int)m_vecBalls.size();++j) {
						if(!m_vecBalls[j]->isFallen()) {
							if(m_vecBalls[j]->getY()<0) {
								m_vecBalls[j]->markFallen();
								DistanceNow = Absol(m_vecDroppers[m_CountDroppers].Position() - m_vecBalls[j]->getX());
								m_vecDroppers[m_CountDroppers].AddFitness((double)(CParams::EnvWidth-DistanceNow));
								m_vecDroppers[m_CountDroppers].AddMiss();
								if(CParams::iMode>0) {
									cout<<"0";
								}
							}
							else {
								Distance3D = PtoP(m_vecDroppers[m_CountDroppers].Position(), 0, m_vecBalls[j]->getX(), m_vecBalls[j]->getY());
								if(Distance3D<(CParams::dAgentRadius + CParams::dBallRadius)) {
									DistanceNow = Absol(m_vecDroppers[m_CountDroppers].Position() - m_vecBalls[j]->getX());
									m_vecDroppers[m_CountDroppers].AddFitness((double)(CParams::EnvWidth-DistanceNow));
									m_vecDroppers[m_CountDroppers].AddCatch();
									m_vecBalls[j]->markCaught();
									m_vecBalls[j]->markFallen();
									if(CParams::iMode>0) {
										cout<<"1";
									}
								}
							}
						}
					}
					if(!m_vecDroppers[m_CountDroppers].Update(m_vecBalls, m_vecBreads)) {
						cout << "Wrong NN inputs!!" << endl;
						exit(1);
					}
					for(j=0;j<(int)m_vecBalls.size();++j) {
						m_vecBalls[j]->setY(m_vecBalls[j]->getY() - m_vecBalls[j]->getSpeed());
					}
				}
				else {
					BallsCaught = 0;
					for(k=0;k<(int)m_vecBalls.size();++k) {
						if(m_vecBalls[k]->isCaught())
							BallsCaught++;
					}
					OverallCatch += BallsCaught;
					++m_CountBalls;
					if(m_CountBalls<m_NumBalls) {
						if(CParams::iMode==2)
							LoadBalls(m_CountBalls);
						else
							RenewBalls(CParams::iBallLayout, bDisp[m_CountBalls]);
					}
					
					if((int)m_vecBreads.size()!=0) {
						for(int idx=0;idx<(int)m_vecBreads.size();++idx)
							delete m_vecBreads[idx];
					}
					m_vecBreads.clear();
					
					m_vecDroppers[m_CountDroppers].Reset();
				}
			}
			else {
				if(CParams::iMode>0) {
					wfname = "compareweights.dat";
					WriteOutput(wfname.c_str(), m_vecDroppers[m_CountDroppers].GetNetworkWeights(), true);
					WriteOutput(wfname.c_str(), m_vecDroppers[m_CountDroppers].GetThreshold());
					return 1;
				}
				GenerationCatch += OverallCatch;
				m_vecThePopulation[m_CountDroppers].dFitness = m_vecDroppers[m_CountDroppers].Fitness();
				m_vecThePopulation[m_CountDroppers].iCatch = m_vecDroppers[m_CountDroppers].Catch();

				if(m_vecDroppers[m_CountDroppers].Catch()==m_NumBalls*2) {
				//if(m_vecDroppers[m_CountDroppers].Catch()>2*(m_NumBalls-1)) {
					cout << "Successfully caught " << m_vecDroppers[m_CountDroppers].Catch() << endl;
					fileWriteBegin = true;
					WriteOutput("caught.dat", m_vecDroppers[m_CountDroppers].Catch());
					wfname = "weights.dat";
					WriteOutput(wfname.c_str(), m_vecDroppers[m_CountDroppers].GetNetworkWeights(), true);
					WriteOutput(wfname.c_str(), m_vecDroppers[m_CountDroppers].GetThreshold());
					WriteOutput("caught.dat", m_vecDroppers[m_CountDroppers].Catch());
					return returnMe(1);
				}
				else{
					//remove("weights.dat");
					wfname = "f_weights.dat";
					WriteOutput(wfname.c_str(), m_vecDroppers[m_CountDroppers].GetNetworkWeights(), true);
					//cout<<m_vecDroppers[m_CountDroppers].GetNetworkWeights().size()<<endl;
					//system("pause");
					WriteOutput(wfname.c_str(), m_vecDroppers[m_CountDroppers].GetThreshold());
				}

				if(m_vecDroppers[m_CountDroppers].Catch()>m_NumBalls*3/2) {
				//if(m_vecDroppers[m_CountDroppers].Catch()>=m_NumBalls/2&&m_CountGenerations==(m_NumGenerations-1)) {
					wfname = "gen_"+itos(m_CountGenerations)+"_id_"+itos(m_CountDroppers)+"_catch_"+itos(m_vecDroppers[m_CountDroppers].Catch())+".dat";
					WriteOutput(wfname.c_str(), m_vecDroppers[m_CountDroppers].GetNetworkWeights(), true);
					WriteOutput(wfname.c_str(), m_vecDroppers[m_CountDroppers].GetThreshold());
				}

				++m_CountDroppers;
				flag2 = true;
				//cout<<endl;
				OverallCatch = 0;
				m_CountBalls = 0;
				redisplacement(m_Displacement);
				//if(m_CountBalls<m_NumBalls) {	//////////////
					RenewBalls(CParams::iBallLayout, bDisp[m_CountBalls]);
				//}								//////////////
			}
		}
		else {
			// if to put ROG selectivity, it should be here
			m_vecThePopulation = m_pGA->Epoch(m_vecThePopulation);
			m_vecAvgFitness.push_back(m_pGA->AverageFitness());
			m_vecBestFitness.push_back(m_pGA->BestFitness());
			m_vecAvgCatch.push_back(m_pGA->AverageCatch());
			m_vecBestCatch.push_back(m_pGA->BestCatch());
			m_vecRetainedAvgFitness.push_back(m_pGA->GetRetainedAvgFitness());
			m_vecRetainedAvgCatch.push_back(m_pGA->GetRetainedAvgCatch());
			cout<<">>>>>Avg Fitness: "<<m_pGA->AverageFitness()<<endl;
			cout<<">>>>RAvg Fitness: "<<m_pGA->GetRetainedAvgFitness()<<endl;
			cout<<">>>>>Avg Catch: "<<m_pGA->AverageCatch()<<endl;
			cout<<">>>>RAvg Catch: "<<m_pGA->GetRetainedAvgCatch()<<endl;
			cout<<">>>>BEST CATCH: "<<m_pGA->BestCatch()<<endl;
			WriteOutput("bestcatch.dat", m_pGA->BestCatch());
			WriteOutput("bestfitness.dat", m_pGA->BestFitness());
			WriteOutput("avgcatch.dat", m_pGA->AverageCatch());
			WriteOutput("avgfitness.dat", m_pGA->AverageFitness());
			WriteOutput("elitecatch.dat", m_pGA->GetRetainedAvgCatch());
			WriteOutput("elitefitness.dat", m_pGA->GetRetainedAvgFitness());
			for(i=0;i<m_NumDroppers;++i) {
				m_vecDroppers[i].PutWeights(m_vecThePopulation[i].vecWeights, m_vecThePopulation[i].dBreadThreshold);
				m_vecDroppers[i].AllReset();
			}
			++m_CountGenerations;
			flag1 = true;
			GenerationCatch = 0;
			m_CountDroppers = 0;
		}
	}
	else {
		//////////////////
		/*fileWriteBegin = true;
		WriteOutput("caught.dat", m_vecDroppers[m_CountDroppers].Catch());
		wfname = "weights.dat";
		WriteOutput(wfname.c_str(), m_vecDroppers[m_CountDroppers].GetNetworkWeights(), true);
		WriteOutput(wfname.c_str(), m_vecDroppers[m_CountDroppers].GetThreshold());
		WriteOutput("caught.dat", m_vecDroppers[m_CountDroppers].Catch());*/
		if(CParams::iMode>0) {
			return returnMe(1);
		}
		else {
			return returnMe(-1);
			//return returnMe(1);
		}
	}

	return 0;
}

bool CController::AllFallen() {
  for(int i=0;i<(int)m_vecBalls.size();++i) {
    if(!m_vecBalls[i]->isFallen())
      return false;
  }
  return true;
}

void CController::LoadBalls(int idx) {
	if(!m_vecBalls.empty()) {
		delete m_vecBalls[0];
		delete m_vecBalls[1];
	}
	m_vecBalls.clear();

	double defaultY = CParams::dRayLength + CParams::dBallRadius;
	double firstX, secondX, firstS, secondS;

	switch(idx) {
		case 0:
			firstX = -40.1365;
			firstS = 2.0;
			secondX = 45.7568;
			secondS = 1.0;
			break;
		case 1:
			firstX = -56.4532;
			firstS = 2.0;
			secondX = 52.8341;
			secondS = 1.0;
			break;
		case 2:
			firstX = -47.4664;
			firstS = 2.0;
			secondX = 39.2539;
			secondS = 1.0;
			break;
		case 3:
			firstX = -49.6291;
			firstS = 2.0;
			secondX = 39.3996;
			secondS = 1.0;
			break;
		case 4:
			firstX = -38.9922;
			firstS = 2.0;
			secondX = 39.2607;
			secondS = 1.0;
			break;
		case 5:
			firstX = -48.6965;
			firstS = 2.0;
			secondX = 42.3984;
			secondS = 1.0;
			break;
		case 6:
			firstX = -55.5663;
			firstS = 1.0;
			secondX = 49.9688;
			secondS = 2.0;
			break;
		case 7:
			firstX = -40.0893;
			firstS = 1.0;
			secondX = 42.5599;
			secondS = 2.0;
			break;
		case 8:
			firstX = -39.2142;
			firstS = 1.0;
			secondX = 45.2892;
			secondS = 2.0;
			break;
		case 9:
			firstX = -50.5595;
			firstS = 1.0;
			secondX = 45.2945;
			secondS = 2.0;
			break;
		case 10:
			firstX = -39.7242;
			firstS = 1.0;
			secondX = 39.8293;
			secondS = 2.0;
			break;
		case 11:
			firstX = -43.2492;
			firstS = 1.0;
			secondX = 43.5414;
			secondS = 2.0;
			break;
		default:
			break;
	}
	CObject *pObj1 = new CObject(firstX, defaultY, firstS);
	CObject *pObj2 = new CObject(secondX, defaultY, secondS);

	m_vecBalls.push_back(pObj1);
    m_vecBalls.push_back(pObj2);
}

void CController::RenewFixedBalls(int Mode, int seed) {
  if(!m_vecBalls.empty()) {
    delete m_vecBalls[0];
    delete m_vecBalls[1];
  }
  m_vecBalls.clear();

  double maxDistance, defaultY, margin;
  margin = 20.0;
  maxDistance = CParams::dRayLength*sin(1.0/9.0*CParams::dPi)-10;
  /*switch(Mode) {
    case 0:
      minDistance = 0;
      maxDistance = CParams::dRayLength*sin(1.0/9.0*CParams::dPi);
      break;
    case 1:
      minDistance = CParams::dRayLength*sin(1.0/9.0*CParams::dPi);
      maxDistance = CParams::dRayLength*sin(1.0/9.0*CParams::dPi)-10;
      break;
    case 2:
      minDistance = 0;
      maxDistance = CParams::dRayLength*sin(1.0/9.0*CParams::dPi);
      break;
    default: break;
  }*/
  defaultY = CParams::dRayLength + CParams::dBallRadius;
  //range = maxDistance - minDistance;

  if(seed==0) {			// left-right, left falls first
    CObject *pObj1 = new CObject(-40.0, defaultY, 2.0);
    CObject *pObj2 = new CObject(40.0, defaultY, 1.0);

    m_vecBalls.push_back(pObj1);
    m_vecBalls.push_back(pObj2);

    //ballSetting--;
  }
  else if(seed==1) {	// right-left, right falls first
    CObject *pObj1 = new CObject(-40.0, defaultY, 1.0);
    CObject *pObj2 = new CObject(40.0, defaultY, 2.0);

    m_vecBalls.push_back(pObj1);
    m_vecBalls.push_back(pObj2);

    //ballSetting++;
  }
  else if(seed==2) {	// left-left
	  CObject *pObj1 = new CObject(-40.0, defaultY, 2.0);
	  CObject *pObj2 = new CObject(-60.0, defaultY, 1.0);

	  m_vecBalls.push_back(pObj1);
	  m_vecBalls.push_back(pObj2);
  }
  else if(seed==3) {	// right-right
	  CObject *pObj1 = new CObject(40.0, defaultY, 2.0);
	  CObject *pObj2 = new CObject(60.0, defaultY, 1.0);

	  m_vecBalls.push_back(pObj1);
	  m_vecBalls.push_back(pObj2);
  }
}

void CController::RenewBalls(int Mode, int seed) {
  if(!m_vecBalls.empty()) {
    delete m_vecBalls[0];
    delete m_vecBalls[1];
  }
  m_vecBalls.clear();

  double maxDistance, defaultY, margin;
  margin = 20.0;
  maxDistance = CParams::dRayLength*sin(1.0/9.0*CParams::dPi)-10;
  /*switch(Mode) {
    case 0:
      minDistance = 0;
      maxDistance = CParams::dRayLength*sin(1.0/9.0*CParams::dPi);
      break;
    case 1:
      minDistance = CParams::dRayLength*sin(1.0/9.0*CParams::dPi);
      maxDistance = CParams::dRayLength*sin(1.0/9.0*CParams::dPi)-10;
      break;
    case 2:
      minDistance = 0;
      maxDistance = CParams::dRayLength*sin(1.0/9.0*CParams::dPi);
      break;
    default: break;
  }*/
  defaultY = CParams::dRayLength + CParams::dBallRadius;
  //range = maxDistance - minDistance;

  if(seed==0) {			// left-right, left falls first
    CObject *pObj1 = new CObject(-(maxDistance - margin*RandFloat()), defaultY, 2.0);
    CObject *pObj2 = new CObject((maxDistance - margin*RandFloat()), defaultY, 1.0);

    m_vecBalls.push_back(pObj1);
    m_vecBalls.push_back(pObj2);

    //ballSetting--;
  }
  else if(seed==1) {	// right-left, right falls first
    CObject *pObj1 = new CObject(-(maxDistance - margin*RandFloat()), defaultY, 1.0);
    CObject *pObj2 = new CObject((maxDistance - margin*RandFloat()), defaultY, 2.0);

    m_vecBalls.push_back(pObj1);
    m_vecBalls.push_back(pObj2);

    //ballSetting++;
  }
  else if(seed==2) {	// left-left
	  CObject *pObj1 = new CObject(-(maxDistance - margin*RandFloat()), defaultY, 2.0);
	  CObject *pObj2 = new CObject(-(3/2)*(maxDistance - margin*RandFloat()), defaultY, 1.0);

	  m_vecBalls.push_back(pObj1);
	  m_vecBalls.push_back(pObj2);
  }
  else if(seed==3) {	// right-right
	  CObject *pObj1 = new CObject((maxDistance - margin*RandFloat()), defaultY, 2.0);
	  CObject *pObj2 = new CObject((3/2)*(maxDistance - margin*RandFloat()), defaultY, 1.0);

	  m_vecBalls.push_back(pObj1);
	  m_vecBalls.push_back(pObj2);
  }
}

double CController::getMax(vector<double> vec) {
  double returnMax = 0.0;
  for(int i=0;i<(int)vec.size();++i) {
    if(vec[i]>returnMax)
      returnMax = vec[i];
  }
  return returnMax;
}

int CController::returnMe(int toreturn) {
  std::string fname;
  fname = "NNtype"+itos(CParams::iNNType)+"gen"+itos(m_CountGenerations)+"fit.dat";
  WriteOutput(fname.c_str(), m_pGA->GetFitnesses());

  fname = "NNtype"+itos(CParams::iNNType)+"gen"+itos(m_CountGenerations)+"cat.dat";
  WriteOutput(fname.c_str(), m_pGA->GetCatches());

  return toreturn;
}

void CController::redisplacement(int iseed) {
	bool homo = true;
	int tmpSum, i;
	while(homo) {
		tmpSum = 0;
		bDisp.clear();
		if((int)bDisp.size()>m_Displacement) {
			cout<<"ERROR DISPLACEMENT SIZE"<<endl;
			exit(1);
		}
		for(i=0;i<m_NumBalls;++i) {
			if(iseed==0) {
				if(i<(double)m_NumBalls/(double)2)
					bDisp.push_back(1);
				else
					bDisp.push_back(0);
			}
			else if(iseed==1) {
				bDisp.push_back(RandInt(0,1));
			}
			else if(iseed==2) {
				if(m_NumBalls%4!=0) {
					cout<<"Number of balls must be a multiple of 4"<<endl;
					exit(1);
				}
				if(i<(double)m_NumBalls/(double)4)
					bDisp.push_back(3);
				else if(i<(double)m_NumBalls/(double)2)
					bDisp.push_back(2);
				else if(i<(double)3*m_NumBalls/(double)4)
					bDisp.push_back(1);
				else
					bDisp.push_back(0);
			
			}
			else if(iseed==3) {
				if(m_NumBalls%6!=0) {
					cout<<"Number of balls must be a multiple of 6"<<endl;
					exit(1);
				}
				if(i<(double)m_NumBalls/(double)3)
					bDisp.push_back(0);
				else if(i<(double)2*m_NumBalls/(double)3)
					bDisp.push_back(1);
				else if(i<(double)5*m_NumBalls/(double)6)
					bDisp.push_back(2);
				else
					bDisp.push_back(3);
			}
			tmpSum += bDisp[i];
		}
		if(tmpSum==0||tmpSum==m_NumBalls)
			homo = true;
		else
			homo = false;
	}
	/*for(i=0;i<(int)bDisp.size();++i)
		cout<<bDisp[i];
	cout<<endl;*/
}
