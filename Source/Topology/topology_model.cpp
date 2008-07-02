/*******************************************************************************
* Copyright (c) 2007, 2008, CerroKai Development
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of CerroKai Development nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY CerroKai Development ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL CerroKai Development BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/


/*** Included Header Files ***/
#include "Topology/topology_model.h"
#include "Topology/topology_types.h"


/***********************************************~***************************************************/

WSVertexUse* _DeleteTopologyVertex(WSVertexUse *vertexUse) {
	//Make sure not NULL
	if (!vertexUse) return NULL;
	
	WSVertexUse* retVal = NULL;
	//See if not only VU in list
	if (vertexUse->next != vertexUse) {
		//Remove vertexUse from list
		vertexUse->next->prev = vertexUse->prev;
		vertexUse->prev->next = vertexUse->next;
		//Set the return value
		retVal = vertexUse->next;
	}
	//Delete the vertexUse
	delete vertexUse;
	//Return the return value
	return retVal;
}


WSEdgeUse* _DeleteTopologyEdge(WSEdgeUse *edgeUse) {
	//Make sure not NULL
	if (!edgeUse) return NULL;
	//Delete all vertexUses
	WSVertexUse *vu = edgeUse->vertexUse;
	while (vu) vu = _DeleteTopologyVertex(vu);

	//Delete the edgeUse
	delete edgeUse;
	//Return the return value
	return NULL;
}


WSLoopUse* _DeleteTopologyLoop(WSLoopUse *loopUse) {
	//Make sure not NULL
	if (!loopUse) return NULL;
	//Delete all edgeUses
	WSEdgeUse *eu = loopUse->edgeUses;
	while (eu) eu = _DeleteTopologyEdge(eu);
	
	WSLoopUse* retVal = NULL;
	//See if not only LU in list
	if (loopUse->next != loopUse) {
		//Remove loopUse from list
		loopUse->next->prev = loopUse->prev;
		loopUse->prev->next = loopUse->next;
		//Set the return value
		retVal = loopUse->next;
	}
	//Delete the loopUse
	delete loopUse;
	//Return the return value
	return retVal;
}


WSFaceUse* _DeleteTopologyFace(WSFaceUse *faceUse) {
	//Make sure not NULL
	if (!faceUse) return NULL;
	//Delete all loopUses
	WSLoopUse *lu = faceUse->loopUses;
	while (lu) lu = _DeleteTopologyLoop(lu);

	WSFaceUse* retVal = NULL;
	//See if not only FU in list
	if (faceUse->next != faceUse) {
		//Remove faceUse from list
		faceUse->next->prev = faceUse->prev;
		faceUse->prev->next = faceUse->next;
		//Set the return value
		retVal = faceUse->next;
	}
	//Delete the faceUse
	delete faceUse;
	//Return the return value
	return retVal;
}


void _DeleteTopologyShell(WSTopologyShell *shell) {
	//Make sure not NULL
	if (!shell) return;

	//Try to delete all vertexUses
	WSVertexUse *vu = shell->vertexUses;
	while (vu) vu = _DeleteTopologyVertex(vu);

	//Try to delete all edgeUses
	WSEdgeUse *eu = shell->edgeUses;
	while (eu) eu = _DeleteTopologyEdge(eu);
	
	//Try to delete all faceUses
	WSFaceUse *fu = shell->faceUses;
	while (fu) fu = _DeleteTopologyFace(fu);
	
	//Delete the shell
	delete shell;
}


/***********************************************~***************************************************/


WCTopologyModel::~WCTopologyModel() {
	//Delete all shells
	std::list<WSTopologyShell*>::iterator listIter;
	for (listIter = this->_shellList.begin(); listIter != this->_shellList.end(); listIter++) {
		//Delete the shells
		_DeleteTopologyShell( *listIter );
	}
}


void WCTopologyModel::AddShell(WSTopologyShell* shell) {
	//Just add it into the list for now
	this->_shellList.push_back(shell);
}


xercesc::DOMElement* WCTopologyModel::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict) {
	return NULL;
}


/***********************************************~***************************************************/


void _PrintTopologyVertex(std::ostream &out, WSVertexUse *vertexUse, const unsigned int &depth) {
}


void _PrintTopologyEdge(std::ostream &out, WSEdgeUse *edgeUse, const unsigned int &depth) {
}


void _PrintTopologyLoop(std::ostream &out, WSLoopUse *loopUse, const unsigned int &depth) {
}


void _PrintTopologyFace(std::ostream &out, WSFaceUse *faceUse, const unsigned int &depth) {
}


void _PrintTopologyShell(std::ostream &out, WSTopologyShell *shell, const unsigned int &depth) {
	//Do the spacing
	for (unsigned int i=0; i<depth; i++) out << "\t";
	//Print the info
	out << "TopologyShell (" << shell << ")\n";
	//Print the faces
	WSFaceUse *start = shell->faceUses;
	WSFaceUse *face = NULL;
	while (face && (face != start)) {
		_PrintTopologyFace(out, face, depth+1);
	}
}


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCTopologyModel &model) {
	out << "Topology Model (" << &model << ")\n";
	//Print all shells
	std::list<WSTopologyShell*>::const_iterator listIter = model._shellList.begin();
	for (; listIter != model._shellList.end(); listIter++) {
		//Print the shells
		_PrintTopologyShell(out, *listIter, 1);
	}
	return out;
}


/***********************************************~***************************************************/

