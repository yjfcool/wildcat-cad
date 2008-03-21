/*** Included Header Files ***/
#include "manifold_operators.h"


/*** Class Order ***/
//---CManifold::MakeModel
//---Support Functions
void CascadeModelDelete(CModelUse *mu);
void CascadeRegionDelete(CRegionUse *ru);
void CascadeShellDelete(CShellUse *su);
void CascadeFaceDelete(CFaceUse *fu);
void CascadeLoopDelete(CLoopUse *lu);
void CascadeEdgeDelete(CEdgeUse *eu);
//---CManifold::KillModel


/*****************************************************************************/


void CManifold::MakeModel(CModel *newModel, CRegion *newRegion, CShell *newShell, CFace *newFace, CLoop *newLoop, CVertex *newVertex) {
	//Check to make sure the topology manager has been started
	if (!CTopologyManager::Started()) {
		cout << "CManifold::MakeModel Error - TopologyManager not started\n";		
		//throw error
		return;
	}
	CLOGGER_INFO(CTopologyManager::Logger(), "CManifold::MakeModel(M:new, R:new, S:new, F:new, L:new, V:new)");
	if ((newModel == NULL) || (newRegion == NULL) || (newShell == NULL) || (newFace == NULL) || (newLoop == NULL) || (newVertex == NULL)) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::MakeModel Error - No NULL parameters allowed");
		//throw error
		return;
	}
	//Create the new ModelUse
	CModelUse* newModelUse = new(CTopologyManager::Pool()) CModelUse(newModel);
	//Create the new RegionUse
	CRegionUse* newRegionUse = new(CTopologyManager::Pool()) CRegionUse(newRegion, newModelUse);	
	//Create the new ShellUse
	CShellUse* newShellUse = new(CTopologyManager::Pool()) CShellUse(newShell, newRegionUse);
	//Create the new FaceUse
	CFaceUse* newFaceUse = new(CTopologyManager::Pool()) CFaceUse(newFace, newShellUse, COrient::Same());
	//Create the new LoopUse
	CLoopUse* newLoopUse = new(CTopologyManager::Pool()) CLoopUse(newLoop, newFaceUse);	
	//Create the new VertexUse
	CVertexUse* newVertexUse = new(CTopologyManager::Pool()) CVertexUse(newVertex, newLoopUse);	
}


/*****************************************************************************/

void CascadeModelDelete(CModelUse *mu) {
	CRegionUse* tmp;

	//Continue deleting RegionUses until they are all gone
	while ((tmp = mu->GetRegionUses()) != NULL) {
		//Cascade delete all of the objects below this RegionUse
		CascadeRegionDelete(tmp);
		//Remove the RegionUse from the list for this model
		tmp->Remove();
		//Now delete the RegionUse itself
		delete tmp;
	}
}


void CascadeRegionDelete(CRegionUse *ru) {
	CShellUse* tmp;
	
	//Continue deleting ShellUses until they are all gone
	while ((tmp = ru->GetShellUses()) != NULL) {
		//Cascade delete all of the objects below this ShellUse
		CascadeShellDelete(tmp);
		//Remove the ShellUse from the list for this model
		tmp->Remove();
		//Now delete the Shell itself
		delete tmp;	
	}
}


void CascadeShellDelete(CShellUse *su) {
	CFaceUse* fu;
	CEdgeUse* eu;
	CVertexUse* vu;
	
	//Continue deleting FaceUses until they are all gone
	while ((fu = su->GetFaceUses()) != NULL) {
		//Cascade delete all of the objects below this FaceUse
		CascadeFaceDelete(fu);
		//Remove the FaceUse from the list for this model
		fu->Remove();
		//Now delete the FaceUse itself
		delete fu;	
	}
	//Deleting EdgeUse, if present
	if ((eu = su->GetEdgeUse()) != NULL) {
		//Cascade delete all of the objects below this EdgeUse
		CascadeEdgeDelete(eu);
		//Remove the EdgeUse from the list for this model
		eu->Remove();
		//Now delete the EdgeUse itself
		delete eu;	
	}
	//Deleting VertexUse, if present
	if ((vu = su->GetVertexUse()) != NULL) {
		//Remove the VertexUse from the list for its vertex
		vu->Remove();
		//Now delete the VertexUse itself
		delete vu;	
	}
}


void CascadeFaceDelete(CFaceUse *fu) {
	CLoopUse* lu;
	
	//Continue deleting LoopUses until they are all gone
	while ((lu = fu->GetLoopUses()) != NULL) {
		//Cascade delete all of the objects below this LoopUse
		CascadeLoopDelete(lu);
		//Remove the LoopUse from the list for this FaceUse
		lu->Remove();
		//Now delete the LoopUse itself
		delete lu;	
	}
}


void CascadeLoopDelete(CLoopUse *lu) {
	CEdgeUse *eu;
	CVertexUse* vu;
	
	//Continue deleting LoopUses until they are all gone
	while ((eu = lu->GetEdgeUses()) != NULL) {
		//Cascade delete all of the objects below this EdgeUse
		CascadeEdgeDelete(eu);
		//Remove the EdgeUse from the list for this FaceUse
		eu->Remove();
		//Now delete the EdgeUse itself
		delete eu;	
	}
	//Deleting VertexUse, if present
	if ((vu = lu->GetVertexUse()) != NULL) {
		//Remove the VertexUse from the list for its vertex
		vu->Remove();
		//Now delete the VertexUse itself
		delete vu;	
	}
}


void CascadeEdgeDelete(CEdgeUse *eu) {
	//All we have to do is delete the associated vertex use (if there is one)
	if (eu->GetVertexUse() != NULL)
		delete eu->GetVertexUse();
}


/*****************************************************************************/


void CManifold::KillModel(CModel *model) {
	CModelUse *modelUse;
	
	//Check to make sure the topology manager has been started
	if (!CTopologyManager::Started()) {
		cout << "CManifold::KillModel Error - TopologyManager not started";		
		//throw error
		return;
	}
	CLOGGER_INFO(CTopologyManager::Logger(), "CManifold::KillModel(M:" << model << ")");
	//Do some basic error checking
	if ( (model == NULL) || (model->GetUses() == NULL) ) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CManifold::KillModel Error - Model or ModelUse is NULL");
		//throw error
		return;	
	}
	
	//Handle the ModelUse
	modelUse = model->GetUses();
	//Call a cascaded delete on the objects below the modelUse
	CascadeModelDelete(modelUse);
	//Now delete the modelUse itself
	delete modelUse;
	//Make sure to clean up the model
	model->ResetUses();
}

/*****************************************************************************/


