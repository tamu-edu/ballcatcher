#include "CDropper.h"

CDropper::CDropper():m_dPosition(0.0),
		     m_dFitness(1.0),
			 //m_dFitness(0.0),
		     m_NNType(CParams::iNNType),
		     m_dPosDifference(0.0),
		     m_iCatch(0),
		     m_iMiss(0),
			 m_dMaxInput(1.0)
{}

CDropper::~CDropper() {
	vCatch.clear();
	m_dDistances.clear();
	inputs.clear();
	outputs.clear();
}

void CDropper::Reset() {
	m_dPosition = 0.0;
}

void CDropper::AllReset() {
	m_dPosition = 0.0;
	m_dFitness = 1.0;
	//m_dFitness = 0.0;
	m_iCatch = 0;
	m_iMiss = 0;
}

bool CDropper::Update(vector<CObject*> &balls, vector<CObject*> &breads) {
	vector<double> toWrite;
	inputs = GetInputs(m_dPosition, balls, breads);
	
	//if(CParams::iMode==1) {
	//	WriteOutput("input_state.dat", inputs, false);
	//}
	
	outputs = m_ItsBrain.Update(inputs);
	
	if(CParams::iMode>0) {
		WriteOutput("output_state.dat", outputs, false);
	}
	
	if(m_NNType==2||m_NNType==3) {
		/*if(outputs[2]>m_dBreadUpperThreshold||outputs[2]<m_dBreadLowerThreshold) {
			ThrowBread(m_dPosition, breads);
		}*/
		
		//////////////////////////////////////
		if(outputs[2]>m_dBreadThreshold) {
			ThrowBread(m_dPosition, breads);
		}
	}

	if(CParams::iMode>0) {
		wfname = "dropper_location.dat";
		WriteOutput(wfname.c_str(), m_dPosition);
		for(int i=0;i<(int)balls.size();++i) {
			if(!(balls[i]->isCaught()) && !(balls[i]->isFallen())) {
				toWrite.push_back(balls[i]->getX());
				toWrite.push_back(balls[i]->getY());
			}
		}
		wfname = "balls_location.dat";
		WriteOutput(wfname.c_str(), toWrite, false);
		toWrite.clear();
		/*for(int i=(int)toWrite.size()-1;i>-1;--i) {
			delete toWrite[i];
			toWrite.erase(toWrite.begin()+i);
		}*/


		wfname = "breads_location.dat";
		for(int i=0;i<(int)breads.size();++i) {
			toWrite.push_back(breads[i]->getX());
		}
		WriteOutput(wfname.c_str(), toWrite, false);
		toWrite.clear();
		/*for(int i=(int)toWrite.size()-1;i>-1;--i) {
			delete toWrite[i];
			toWrite.erase(toWrite.begin()+i);
		}*/
	}
	
	double prevPosition = m_dPosition;
	double eps = numeric_limits<double>::epsilon();
	m_dPosDifference = outputs[0] - outputs[1];
	////////////////////////////////////////////////// Static movement regardless of the degree of output difference
	if(m_dPosDifference<0) {						// IF output[0]<output[1] THEN move right
	//m_dPosition -= CParams::dAgentSpeed*m_dPosDifference;//*outputs[1];
		m_dPosition += CParams::dAgentSpeed;
		prevPosition += 0.000001;
	}
	else if(m_dPosDifference>0) {					// IF output[0]>output[1] THEN move left
		//m_dPosition += CParams::dAgentSpeed*m_dPosDifference;//*outputs[0];
		m_dPosition -= CParams::dAgentSpeed;
		prevPosition -= 0.000001;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////// Movement proportional to output difference
	/*if(m_dPosDifference<0) {
	m_dPosition += CParams::dAgentSpeed;
	prevPosition += 0.000001;
	}
	else {
	m_dPosition -= CParams::dAgentSpeed;
	prevPosition -= 0.000001;
	}*/
	/////////////////////////////////////////////////////////////////////////////////////////////
	
	if(m_dPosition>(CParams::EnvWidth/2 - CParams::dAgentRadius))
		m_dPosition = CParams::EnvWidth/2 - CParams::dAgentRadius;

	if(m_dPosition<(-CParams::EnvWidth/2 + CParams::dAgentRadius))
		m_dPosition = -CParams::EnvWidth/2 + CParams::dAgentRadius;
	
	GatherBread(prevPosition, m_dPosition, breads);
	
	return true;
}

vector<double> CDropper::GetInputs(double pos, vector<CObject*> &balls, vector<CObject*> &breads) {
	vector<double> NewInputs;
	vector<double> RayLengths;
	
	int RayNum = CParams::iNumRays;
	int i;
	//double penalty = 2.0*CParams::dRayLength;
	
	for(i=0;i<RayNum;++i)
		RayLengths.push_back(CParams::dRayLength);
	
	double CurrentDistance;
	for(i=0;i<RayNum;++i) {
		for(int j=0;j<(int)balls.size();++j) {
			CurrentDistance = GetDistance(pos, i, balls[j]);
			//cout<<CurrentDistance<<"\t";
			if(CurrentDistance<0) {
				cout << "Negative 'GetDistance' Error!!" << endl;
				exit(1);
			}
			
			if(CurrentDistance<RayLengths[i])
				RayLengths[i] = CurrentDistance;
		}
	}
	//cout<<"\n";
	
	if(m_NNType==2||m_NNType==3) {
		RayNum += 2;
		vector<double> BreadDistances = GetBreadDistances(pos, breads);
		for(i=0;i<(int)BreadDistances.size();++i){
			if(BreadDistances[i]<0) {
				cout<<"Negative bread distance!!"<<endl;
				exit(1);
			}
			RayLengths.push_back(BreadDistances[i]);
			//RayLengths.push_back(penalty*BreadDistances[i]);
		}
	}
	
	//m_dDistances = RayLengths;
	
	if(CParams::iMode>0) {
		wfname = "rays.dat";
		WriteOutput(wfname.c_str(), RayLengths, false);
	}

	for(i=0;i<RayNum;++i) {					// new input ranges from 0 to m_dMaxInput
		if(m_NNType==2||m_NNType==3) {
			if(i<RayNum-2)
				NewInputs.push_back(m_dMaxInput*(CParams::dRayLength - RayLengths[i])/CParams::dRayLength);
			else
				NewInputs.push_back(m_dMaxInput*(CParams::dBRayLength - RayLengths[i])/CParams::dBRayLength);
				//NewInputs.push_back(m_dMaxInput*(CParams::dRayLength/CParams::dBreadSensorDivisor - RayLengths[i])/(CParams::dRayLength/CParams::dBreadSensorDivisor));
		}
		else {
			NewInputs.push_back(m_dMaxInput*(CParams::dRayLength - RayLengths[i])/CParams::dRayLength);
		}

		//NewInputs.push_back(m_dMaxInput*(1/RayLengths[i]));		
		//cout<<m_dMaxInput*(CParams::dRayLength - RayLengths[i])/CParams::dRayLength<<"\t";
	}
	//cout<<"\n";
	
	return NewInputs;
}

vector<double> CDropper::GetBreadDistances(double myX, vector<CObject*> &breads) {
	vector<double> returnVec;
	int i;
	double left, right;
	//left = right = CParams::dRayLength;			// By default, bread sensors have the same length as ball sensors
	//left = right = CParams::dRayLength/CParams::dBreadSensorDivisor;											// Simulations varying this value might be intersting
	left = right = CParams::dBRayLength;
												// To vary this, last parts of GetInputs function also need to be changed
	double objX;
	
	for(i=0;i<(int)breads.size();++i) {
		objX = breads[i]->getX();
		if(objX<myX) {
			if((myX-objX)<left)
				left = (myX-objX);
		}
		else {
			if((objX-myX)<right)
				right = (objX-myX);
		}
		if(left<0) {
			cout<<"left negative: ";
			cout<<myX;
			cout<<",";
			cout<<objX;
		}
		if(right<0) {
			cout<<"right negative: ";
			cout<<myX;
			cout<<",";
			cout<<objX;
		}
	}
	returnVec.push_back(left);
	returnVec.push_back(right);
	return returnVec;
}

double CDropper::GetDistance(double myX, int RayIndex, CObject* ball) {
	if(ball->isFallen())
		return CParams::dRayLength;

	double RayInterval = (double)CParams::iRayWidth/(CParams::iNumRays-1);
	double Theta = CParams::dPi*(90+CParams::iRayWidth/2 - RayIndex*RayInterval)/180;
	double A, B, C, RootFormula;
	double tmp1, tmp2, intersectX, intersectY;
	
	if(RayIndex*RayInterval!=20) {
		A = 1 + pow(tan(Theta), 2.0);
		B = -2*(ball->getX()+tan(Theta)*(tan(Theta)*myX + ball->getY()));
		C = pow(ball->getX(), 2.0) + pow(tan(Theta)*myX + ball->getY(), 2.0) - pow(CParams::dBallRadius, 2.0);
		RootFormula = pow(B, 2.0) - 4*A*C;
		
		if(RootFormula<0)
			return CParams::dRayLength;
		else if(RootFormula==0) {
			intersectX = -B/(2*A);
			intersectY = tan(Theta)*(intersectX-myX);
			return PtoP(myX, 0.0, intersectX, intersectY);
		}
		else {
			intersectX = (-B + sqrt(RootFormula))/(2*A);
			intersectY = tan(Theta)*(intersectX - myX);
			tmp1 = PtoP(myX, 0.0, intersectX, intersectY);
			intersectX = (-B - sqrt(RootFormula))/(2*A);
			intersectY = tan(Theta)*(intersectX - myX);
			tmp2 = PtoP(myX, 0.0, intersectX, intersectY);
			if(tmp1<tmp2)
				return tmp1;
			else
				return tmp2;
		}
	}
	else {
		A = 1.0;
		B = -2*ball->getY();
		C = pow(ball->getY(), 2.0)+pow((myX - ball->getX()), 2.0) - pow(CParams::dBallRadius, 2.0);
		RootFormula = pow(B, 2.0) - 4*A*C;
		
		if(RootFormula<0)
			return CParams::dRayLength;
		else if(RootFormula==0) {
			tmp1 = -B/(2*A);
			if(tmp1>CParams::dRayLength)
				return CParams::dRayLength;
			else
				return tmp1;
		}
		else {
			tmp1 = (-B - sqrt(RootFormula))/(2*A);
			if(tmp1>CParams::dRayLength)
				return CParams::dRayLength;
			else
				return tmp1;
		}
	}
}

void CDropper::ThrowBread(double pos, vector<CObject*> &breads) {
	bool alreadyThere = false;
	for(int i=0;i<(int)breads.size();++i) {
		if(breads[i]->getX()==pos)
			alreadyThere = true;
	}
	
	if(!alreadyThere) {
		CObject *newBread = new CObject();
		newBread->setX(pos);
		breads.push_back(newBread);
	}
}

void CDropper::GatherBread(double prevpos, double curpos, vector<CObject*> &breads) {
	double from, to;
	if(prevpos<curpos) {
		from = prevpos;
		to = curpos;
	}
	else {
		from = curpos;
		to = prevpos;
	}
	
	for(int i=(int)breads.size()-1;i>-1;--i) {
		if(breads[i]->getX()>=from&&breads[i]->getX()<=to) {
			delete breads[i];
			breads.erase(breads.begin()+i);
		}
	}
}

void CDropper::PutWeights(vector<double> &w) {
	m_ItsBrain.PutWeights(w);
	//m_dBreadUpperThreshold = 0.0;
	//m_dBreadLowerThreshold = 0.0;
	m_dBreadThreshold = 0.0;
}

void CDropper::PutWeights(vector<double> &w, double bth) {
	m_ItsBrain.PutWeights(w);
	m_dBreadThreshold = bth;
	//m_dBreadThreshold = 0.0;
}

bool WriteOutput(const char* filename, vector<CObject*> &toWrite) {
	FILE* output;
	if((output=fopen(filename, "w"))==NULL) {
		cout << "WriteOutput Error(1)!!" << endl;
	}
	for(int i=0;i<(int)toWrite.size();++i) {
		if(!(toWrite[i]->isCaught()) && !(toWrite[i]->isFallen()))
			fprintf(output, "%f\t%f", toWrite[i]->getX(), toWrite[i]->getY());
	}
	//return true;
}

/*
void CDropper::PutWeights(vector<double> &w, double bth1, double bth2) {
	m_ItsBrain.PutWeights(w);
	m_dBreadUpperThreshold = bth1;
	m_dBreadLowerThreshold = bth2;
}
*/
