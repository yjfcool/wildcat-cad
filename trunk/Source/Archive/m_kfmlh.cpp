/*** Included Header Files ***/
#include "manifold_operators.h"


/*** Class Order ***/
//---CManifold::KillEdgeMakeFaceLoop


/*****************************************************************************/


void CManifold::KillFaceMakeLoopHole(CFace *oldFace, CFace *otherFace, CLoop *newLoop) {


	//Check to make sure the TopologyManager has been started
	if (!CTopologyManager::Started()) {
		cout << "CManifold::KillFaceMakeLoopHole Error - TopologyManager not started\n";		
		//throw error
		return;
	}
	CLOGGER_INFO(CTopologyManager::Logger(), "CManifold::KillFaceMakeLoopHole(F:" << oldFace << ", F:" << otherFace << ", L:new)");	
	//Do some basic error checking on passed values
	if ( (oldFace == NULL) || (otherFace == NULL) ){
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::KillFaceMakeLoopHole Error - Must pass valid Faces");
		//throw error
		return;
	}
}


/*****************************************************************************/


