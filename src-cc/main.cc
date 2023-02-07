#include <stdlib.h>

#include "utils.h"
#include "CController.h"
#include "CParams.h"

// Controller class for simulation
CController* g_pController = NULL;

CParams g_Params;

void Cleanup(void) {
	if(g_pController)
		delete g_pController;
}

int main(void) {
	int controllerCount = 0;
	g_pController = new CController();
	controllerCount++;
	bool bDone = false;
	int currentResult;
	
	while(!bDone) {
		currentResult = g_pController->Update();
		if(currentResult==1)
			bDone = true;
		if(currentResult==-1) {
			remove("avgcatch.dat");
			remove("avgfitness.dat");
			remove("bestcatch.dat");
			remove("bestfitness.dat");
			remove("elitecatch.dat");
			remove("elitefitness.dat");
			delete g_pController;
			g_pController = new CController();
			controllerCount++;
		}
	}
	cout<<"Counter Number: "<<controllerCount;
	
	Cleanup();

	return 0;
}
