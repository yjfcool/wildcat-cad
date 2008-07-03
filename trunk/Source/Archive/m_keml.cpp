/*** Included Header Files ***/
#include "manifold_operators.h"


/*** Class Order ***/
//---CManifold::KillEdgeMakeFaceLoop


/*****************************************************************************/


void CManifold::KillEdgeMakeLoop(CEdge *edge, CVertex *vertex, CLoop *newLoop) {
	CEdgeUse *eu1, *eu2, *cutStart, *cutEnd;
	int i=0;
	
	//Check to make sure the TopologyManager has been started
	if (!CTopologyManager::Started()) {
		cout << "CManifold::KillEdgeMakeLoop Error - TopologyManager not started\n";		
		//throw error
		return;
	}
	CLOGGER_INFO(CTopologyManager::Logger(), "CManifold::KillEdgeMakeLoop(E:" << edge << ", V:" << vertex << ", L:new)");	
	//Do some basic error checking on passed values
	if (edge == NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::KillEdgeMakeFaceLoop Error - Must pass valid edge");
		//throw error
		return;
	}
	//Make sure that the given edge is used twice in the given face, and has no other EdgeUses
	eu1 = edge->GetUses();
	eu2 = eu1->GetRadial();
	if ( (eu2->GetRadial() != eu1) || (eu1->GetLoopUse() != eu2->GetLoopUse()) ){
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::KillEdgeMakeFaceLoop Error - Passed edge has more than two uses");
		//throw error
		return;
	}
	if ( (eu1->GetClockwise() == eu2) && (eu2->GetClockwise() == eu1) ) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::KillEdgeMakeFaceLoop Error - Edge's loop consists of only edge");
		//throw error
		return;
	}
	//Does the vertex associated with eu1 match the passed vertex
	if (eu1->GetVertexUse()->GetOwner() == vertex) {
		cutStart = eu2;
		cutEnd = eu1;
	}
	//Or, does the vertex associated with eu2 match the passed vertex
	else if (eu2->GetVertexUse()->GetOwner() == vertex) {
		cutStart = eu1;
		cutEnd = eu2;
	}
	//If neither, then we have an error
	else {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::KillEdgeMakeFaceLoop Error - Given Vertex is not at an end of the Edge");
		//throw error
		return;	
	}
	//cout << "CS: " << cutStart << ", CE: " << cutEnd << endl;
	//Create the new LoopUse (add it to the EdgeUse's LoopUse's FaceUse)
	CLoopUse *newLoopUse = new(CTopologyManager::Pool()) CLoopUse(newLoop, eu1->GetLoopUse()->GetParent());
	//Loop through the "to-be-cut" objects and add them to newLoopUse
	eu1 = cutStart->GetClockwise();
	eu2 = eu1;
	while (eu1 != cutEnd) {
		//cout << "Adding " << eu1 << " to LoopUse, eu2: " << eu2 << "\n";
		//Add the eu to the newLoopUse
		eu1 = eu1->Reinsert(newLoopUse, eu2);
		//Go to the next eu in the cut portion of the loop
		eu2 = eu2->GetClockwise();
		//Make sure to count the number of edges added (for later use)
		i++;
	}
	//If no EdgeUses are added, add in the vertex associated with cutEnd eu to just the loop
	if (i == 0) {
		CVertexUse *newVertexUse = new(CTopologyManager::Pool()) CVertexUse(vertex, newLoopUse);
	}
	//Make sure the loop points to the start of the cut
	if (i > 0) {
		newLoopUse->SetEdgeUses(cutStart->GetClockwise());
	}
	//Close off the old loop
	cutEnd->GetClockwise()->Splice(cutStart->GetCounterClockwise());
	//Now delete the un-needed EdgeUses
	delete cutStart;
	delete cutEnd;
}


/*****************************************************************************/

