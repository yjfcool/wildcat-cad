/*** Included Header Files ***/
#include "manifold_operators.h"


/*** Class Order ***/
//---CManifold::MakeEdgeVertex


/*****************************************************************************/


void CManifold::MakeEdgeVertex(CVertex *vertex, CFace *face, CEdge *newEdge, CVertex *newVertex, CEdge* edge) {
	CLoopUse* lu = NULL;
	CEdgeUse *tmpEU, *eu = NULL;
	//Create holders for the new EdgeUses
	CEdgeUse *euNewToOld, *euOldToNew;
	
	//Check to make sure the topology manager has been started
	if (!CTopologyManager::Started()) {
		cout << "CManifold::MakeEdgeVertex Error - TopologyManager not started";		
		//throw error
		return;
	}
	CLOGGER_INFO(CTopologyManager::Logger(), "CManifold::MakeEdgeVertex(V:" << vertex << ", F:" << face << ", E:new, V:new, RefE:" << edge << ")");
	//Do some basic checking to make sure the function is called properly
	if ((face == NULL) || (vertex == NULL) || (newEdge == NULL) || (newVertex == NULL)) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::MakeEdgeVertex Error - Invalid Face, Vertex, new Edge, or new Vertex passed");
		//throw error
		return;
	}
	//Check to make sure there is a valid LoopUse in the Face
	if (((lu = face->GetUses()->GetLoopUses()) == NULL) || ( ((tmpEU = lu->GetEdgeUses()) == NULL) && (lu->GetVertexUse() == NULL) )) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::MakeEdgeVertex Error - No valid LoopUse (or children) associated with the Face");
		//throw error
		return;	
	}
	
	//Check to see if there is a good eu in the lu
	if (tmpEU != NULL) {
		//Start at processing the loop
		do {
			//Does this eu have a vu that corresponds to the given vertex
			if (tmpEU->GetVertexUse()->GetOwner() == vertex) {
				if ((eu == NULL) || (tmpEU->GetOwner() == edge))
					eu = tmpEU;
			}
			//Move to the next eu in the loop
			tmpEU = tmpEU->GetClockwise();
		}
		while (tmpEU != lu->GetEdgeUses());
		//If no eu is found, then error out
		if (eu == NULL) {
			CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::MakeEdgeVertex - Passed Vertex and Edge do not fall on given Face");
			//throw error
			return;
		}
	}
	//Check to see if instead there is a vu in the lu, and that it matches the passed vertex
	else if (lu->GetVertexUse()->GetOwner() != vertex) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::MakeEdgeVertex - Passed Vertex is not on the given Face");
		//throw error
		return;
	}
	
	//Have the EdgeUse going from the New to the Old Vertex point to the old EdgeUse as the next CW EdgeUse
	euNewToOld = new(CTopologyManager::Pool()) CEdgeUse(newEdge, lu, eu, COrient::Opposite());		//Define in the opposite direction as the geometry
	//Have the EdgeUse going from the Old to the New Vertex point to the new EdgeUse as the next CW EdgeUse
	euOldToNew = new(CTopologyManager::Pool()) CEdgeUse(newEdge, lu, euNewToOld, COrient::Same());	//Define in the same direction as the geometry

	//Finally, create the two VertexUses for the two Vertices, remember, this may delete some existing VertexUses
	CVertexUse* newVertexUse1 = new(CTopologyManager::Pool()) CVertexUse(vertex, euOldToNew);		//Related to EU going from old Vertex to new Vertex	
	CVertexUse* newVertexUse2 = new(CTopologyManager::Pool()) CVertexUse(newVertex, euNewToOld);	//Related to EU going from new Vertex to old Vertex
	//Set Radial for both new EdgeUses
	euOldToNew->SetRadial(euNewToOld);
	euNewToOld->SetRadial(euOldToNew);
	//Exit the function
	return;
}


/*****************************************************************************/


