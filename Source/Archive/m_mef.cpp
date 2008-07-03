/*** Included Header Files ***/
#include "manifold_operators.h"


/*** Class Order ***/
//---CManifold::MakeEdgeFace


/*****************************************************************************/


void CManifold::MakeEdgeFace(CVertex *v1, CVertex *v2, CFace *face, CEdge *newEdge, CLoop *newLoop, CFace *newFace) {
	CEdgeUse *eu1, *eu2, *tmpEU;
	bool inCut=false;
	CLoopUse *oldLoopUse;

	//Check to make sure TopologyManager has been started
	if (!CTopologyManager::Started()) {
		cout << "CManifold::MakeEdgeFace Error - TopologyManager not started\n";		
		//throw error
		return;
	}
	CLOGGER_INFO(CTopologyManager::Logger(), "CManifold::MakeEdgeFace(V1:" << v1 << ", V2:" << v2 << ", Face:" << face << ", Edge:new, Loop:new, Face:new)");
	//Do some basic checking to make sure the function is called properly
	if ((face == NULL) || (v1 == NULL) || (v2 == NULL) || (newEdge == NULL) || (newLoop == NULL) || (newFace == NULL)) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::MakeEdgeFace Error - Invalid argument passed");
		//throw error
		return;
	}
	//Check to see if the two vertices are the same
	if (v1 == v2) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::MakeEdgeFace Error - Not currently handling having both vertices be the same");
		//throw error
		return;
	}
	//Check to make sure that v1 and v2 are in the same loop on face, and only once each
	CVertexUse* tmpVU = v1->GetUses();
	//Cycle through all of the vu's for v1 until one on face is found
	do {
		tmpVU = tmpVU->GetNext();
		//Make sure the vu's eu is valid and points to a valid lu, which points to the correct fu
		if ( ((tmpEU = tmpVU->GetEdgeUse()) != NULL) && (tmpEU->GetLoopUse() != NULL) && (tmpEU->GetLoopUse()->GetParent() == face->GetUses()) ) {
			//cout << "Found EU: " << tmpEU << endl;
			eu1 = tmpEU;
			oldLoopUse = eu1->GetLoopUse();
			//Now, exit the loop
			tmpVU = v1->GetUses();
		}
	}
	while (tmpVU != v1->GetUses());
	//If eu1 is NULL, then v1 is not part of face
	if (eu1 == NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::MakeEdgeFace Error - Vertex 1 is not part of the passed Face");
		//throw error
		return;
	}
	
	//Now find the EdgeUse for v2
	tmpEU = eu1->GetClockwise();
	//Work clockwise through the Loop's EdgeUses looking for the one that points to v2
	while (tmpEU != eu1) {
		//Set the indicator (true if old LoopUse points into the cut, false if not in the cut)
		if (tmpEU == oldLoopUse->GetEdgeUses()) inCut = true;
		//Check to see if this EdgeUse points to the correct Vertex
		if (tmpEU->GetVertexUse()->GetOwner() == v2) {
			//Capture which EdgeUse
			eu2 = tmpEU;
			//Exit the while statement
			tmpEU = eu1;
			//cout << "EU 2 found: " << eu2 << endl;
		}
		//Move to the next EdgeUse in the Loop
		else {
			tmpEU = tmpEU->GetClockwise();
		}
	}
	//If eu2 is NULL, then v2 is not part of face
	if (eu2 == NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::MakeEdgeFace Error - Vertex 2 is not part of the passed Face");
		//throw error
		return;
	}
	
	//Create the new FaceUse
	CFaceUse* newFaceUse = new(CTopologyManager::Pool()) CFaceUse(newFace, face->GetUses()->GetParent(), COrient::Same());
	//Create the new LoopUse
	CLoopUse* newLoopUse = new(CTopologyManager::Pool()) CLoopUse(newLoop, newFaceUse);	
	//Create holders for the new EdgeUses
	CEdgeUse *euNewLoop, *euOldLoop;
	
	//cout << "EU1: " << eu1 << ", EU2: " << eu2 << endl;
	//If not "InCut" then reverse the eu1 and eu2 pointers
	if (!inCut) {
		tmpEU = eu1;
		eu1 = eu2;
		eu2 = tmpEU;
	}
	
	//Sew the two ends of the old Loop together
	eu1->Splice(eu2->GetCounterClockwise());
	//Add in the new EdgeUse into the old LoopUse
	euOldLoop = new(CTopologyManager::Pool()) CEdgeUse(newEdge, oldLoopUse, eu1, COrient::Opposite());
	//And, add in a new vertex use going from the same point as eu2 to eu1
	CVertexUse* newVertexUse1 = new(CTopologyManager::Pool()) CVertexUse(eu2->GetVertexUse()->GetOwner(), euOldLoop);

	//Have the EdgeUse going from the v1 to v2 point nothing as the next eu
	euNewLoop = new(CTopologyManager::Pool()) CEdgeUse(newEdge, newLoopUse, NULL, COrient::Same());
	tmpEU = euNewLoop;
	//Loop through all of the "new" Loop eu's and add them to the new Loop
	while (eu2 != eu1) {
		eu2 = eu2->Reinsert(newLoopUse, tmpEU);
		tmpEU = tmpEU->GetClockwise();
	}
	//Finally, create the two VertexUses for the two Vertices, remember, this may delete some existing VertexUses
	CVertexUse* newVertexUse2 = new(CTopologyManager::Pool()) CVertexUse(eu1->GetVertexUse()->GetOwner(), euNewLoop);
	
	//Set Radial for both new EdgeUses
	euOldLoop->SetRadial(euNewLoop);
	euNewLoop->SetRadial(euOldLoop);
}


/*****************************************************************************/


