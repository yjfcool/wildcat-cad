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
#include "Geometry/geometric_types.h"
#include "Geometry/geometric_line.h"
#include "Geometry/nurbs_curve.h"


/***********************************************~***************************************************/


void _LoadTopologyVertex(WSVertexUse* vertex, xercesc::DOMElement *element, WCSerialDictionary *dictionary) {
	//Set vertex attributes
	vertex->point = (WCGeometricPoint*)WCSerializeableObject::GetGUIDAttrib(element, "point", dictionary);
	vertex->shell = (WSTopologyShell*)WCSerializeableObject::GetGUIDAttrib(element, "shell", dictionary);
	vertex->loop = (WSLoopUse*)WCSerializeableObject::GetGUIDAttrib(element, "loop", dictionary);
	vertex->edge = (WSEdgeUse*)WCSerializeableObject::GetGUIDAttrib(element, "edge", dictionary);
	vertex->next = (WSVertexUse*)WCSerializeableObject::GetGUIDAttrib(element, "next", dictionary);
	vertex->prev = (WSVertexUse*)WCSerializeableObject::GetGUIDAttrib(element, "prev", dictionary);
}


void _LoadTopologyEdge(WSEdgeUse* edge, xercesc::DOMElement *element, WCSerialDictionary *dictionary) {
	//Set edge attributes
	edge->curve = (WCGeometricCurve*)WCSerializeableObject::GetGUIDAttrib(element, "curve", dictionary);
	edge->orientation = WCSerializeableObject::GetBoolAttrib(element, "orientation");
	edge->mate = (WSEdgeUse*)WCSerializeableObject::GetGUIDAttrib(element, "mate", dictionary);
	edge->shell = (WSTopologyShell*)WCSerializeableObject::GetGUIDAttrib(element, "shell", dictionary);
	edge->loop = (WSLoopUse*)WCSerializeableObject::GetGUIDAttrib(element, "loop", dictionary);
	edge->cw = (WSEdgeUse*)WCSerializeableObject::GetGUIDAttrib(element, "cw", dictionary);
	edge->ccw = (WSEdgeUse*)WCSerializeableObject::GetGUIDAttrib(element, "ccw", dictionary);
	edge->radial = (WSEdgeUse*)WCSerializeableObject::GetGUIDAttrib(element, "radial", dictionary);
	edge->vertexUse = (WSVertexUse*)WCSerializeableObject::GetGUIDAttrib(element, "vertexUse", dictionary);
}


void _LoadTopologyLoop(WSLoopUse* loop, xercesc::DOMElement *element, WCSerialDictionary *dictionary) {
	//Set loop attributes
	loop->face = (WSFaceUse*)WCSerializeableObject::GetGUIDAttrib(element, "face", dictionary);
	loop->next = (WSLoopUse*)WCSerializeableObject::GetGUIDAttrib(element, "next", dictionary);
	loop->prev = (WSLoopUse*)WCSerializeableObject::GetGUIDAttrib(element, "prev", dictionary);
	loop->mate = (WSLoopUse*)WCSerializeableObject::GetGUIDAttrib(element, "mate", dictionary);
	loop->edgeUses = (WSEdgeUse*)WCSerializeableObject::GetGUIDAttrib(element, "edgeUses", dictionary);
	loop->vertexUses = (WSVertexUse*)WCSerializeableObject::GetGUIDAttrib(element, "vertexUses", dictionary);
}


void _LoadTopologyFace(WSFaceUse* face, xercesc::DOMElement *element, WCSerialDictionary *dictionary) {
	//Set face attributes
	face->surface = (WCGeometricSurface*)WCSerializeableObject::GetGUIDAttrib(element, "surface", dictionary);
	face->orientation = WCSerializeableObject::GetBoolAttrib(element, "orientation");
	face->shell = (WSTopologyShell*)WCSerializeableObject::GetGUIDAttrib(element, "shell", dictionary);
	face->next = (WSFaceUse*)WCSerializeableObject::GetGUIDAttrib(element, "next", dictionary);
	face->prev = (WSFaceUse*)WCSerializeableObject::GetGUIDAttrib(element, "prev", dictionary);
	face->mate = (WSFaceUse*)WCSerializeableObject::GetGUIDAttrib(element, "mate", dictionary);
	face->loopUses = (WSLoopUse*)WCSerializeableObject::GetGUIDAttrib(element, "loopUses", dictionary);
}


WSTopologyShell* _LoadTopologyShell(xercesc::DOMElement *element, WCSerialDictionary *dictionary) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "_LoadTopologyShell - NULL Element passed.");
		//throw error
		return NULL;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	//Create new shell
	WSTopologyShell *shell = new WSTopologyShell();
	//Insert into GUID dictionary
	dictionary->InsertGUID(guid, shell);
	//Set shell values
	shell->model = (WCTopologyModel*)WCSerializeableObject::GetGUIDAttrib(element, "model", dictionary);
	shell->faceUses = (WSFaceUse*)WCSerializeableObject::GetGUIDAttrib(element, "faceUses", dictionary);
	shell->edgeUses = (WSEdgeUse*)WCSerializeableObject::GetGUIDAttrib(element, "edgeUses", dictionary);
	shell->vertexUses = (WSVertexUse*)WCSerializeableObject::GetGUIDAttrib(element, "vertexUses", dictionary);
	
	//Return the shell
	return shell;
}


/***********************************************~***************************************************/


void _CatalogTopologyVertex(WSVertexUse* vertex, std::map<WSVertexUse*,WSVertexUse*> &vertices) {
	//Create a new vu
	WSVertexUse *newVU = new WSVertexUse();
	//Insert both vu into catalog
	vertices.insert( std::make_pair(vertex, newVU) );
}


void _CatalogTopologyEdge(WSEdgeUse* edge, std::map<WSEdgeUse*,WSEdgeUse*> &edges, std::map<WSVertexUse*,WSVertexUse*> &vertices) {
	//Create a new eu
	WSEdgeUse *newEU = new WSEdgeUse();
	//Insert both vu into catalog
	edges.insert( std::make_pair(edge, newEU) );
	//Catalog the child vertexUse
	_CatalogTopologyVertex(edge->vertexUse, vertices);
}


void _CatalogTopologyLoop(WSLoopUse* loop, std::map<WSLoopUse*,WSLoopUse*> &loops,
						  std::map<WSEdgeUse*,WSEdgeUse*> &edges, std::map<WSVertexUse*,WSVertexUse*> &vertices) {
	//Create a new lu
	WSLoopUse *newLU = new WSLoopUse();
	//Insert both lu into catalog
	loops.insert( std::make_pair(loop, newLU) );
	
	//See if children are vertices
	if (loop->vertexUses) {
		//Catalog all children objects
		WSVertexUse *firstVU = NULL, *vu = loop->vertexUses;
		while (firstVU != vu) {
			//Recursively get all children, starting with this vertex
			_CatalogTopologyVertex(vu, vertices);
			//Set first LU if necessary
			if (!firstVU) firstVU = vu;
			//Move to next vu
			vu = vu->next;
		}		
	}
	//Children must be edges
	else if (loop->edgeUses) {
		//Catalog all children objects
		WSEdgeUse *firstEU = NULL, *eu = loop->edgeUses;
		while (firstEU != eu) {
			//Recursively get all children, starting with this edge
			_CatalogTopologyEdge(eu, edges, vertices);
			//Set first EU if necessary
			if (!firstEU) firstEU = eu;
			//Move to next eu
			eu = eu->cw;			
		}
	}
}


void _CatalogTopologyFace(WSFaceUse* face, std::map<WSFaceUse*,WSFaceUse*> &faces,
						  std::map<WSLoopUse*,WSLoopUse*> &loops, std::map<WSEdgeUse*,WSEdgeUse*> &edges, std::map<WSVertexUse*,WSVertexUse*> &vertices) {
	//Create a new fu
	WSFaceUse *newFU = new WSFaceUse();
	//Insert both fu into map
	faces.insert( std::make_pair(face, newFU) );
	
	//Catalog all children objects
	WSLoopUse *firstLU = NULL, *lu = face->loopUses;
	while (firstLU != lu) {
		//Recursively get all children, starting with this loop
		_CatalogTopologyLoop(lu, loops, edges, vertices);
		//Set first LU if necessary
		if (!firstLU) firstLU = lu;
		//Move to next lu
		lu = lu->next;
	}
}


void _CatalogTopologyShell(WSTopologyShell *shell, std::map<WSTopologyShell*,WSTopologyShell*> &shells,
						   std::map<WSFaceUse*,WSFaceUse*> &faces, std::map<WSLoopUse*,WSLoopUse*> &loops, std::map<WSEdgeUse*,WSEdgeUse*> &edges,
						   std::map<WSVertexUse*,WSVertexUse*> &vertices) {
	//Check for shell existance
	ASSERT(shell);
	//Create new shell
	WSTopologyShell* newShell = new WSTopologyShell();
	//Insert into catalog
	shells.insert( std::make_pair(shell,newShell) );
	
	//See if vertexUses is non-NULL
	if (shell->vertexUses) {
		//Catalog the vertex
		_CatalogTopologyVertex(shell->vertexUses, vertices);
	}
	
	//See if edgeUses is non-NULL
	if (shell->edgeUses) {
		//Catalog all children objects
		WSEdgeUse *firstEU = NULL, *eu = shell->edgeUses;
		while (firstEU != eu) {
			//Recursively get all children, starting with this edge
			_CatalogTopologyEdge(eu, edges, vertices);
			//Set first EU if necessary
			if (!firstEU) firstEU = eu;
			//Move to next eu
			eu = eu->cw;
		}
	}
	
	//See if faceUses if non-NULL
	if(shell->faceUses) {
		//Catalog all children objects
		WSFaceUse *firstFU = NULL, *fu = shell->faceUses;
		while (firstFU != fu) {
			//Recursively get all children, starting with this face
			_CatalogTopologyFace(fu, faces, loops, edges, vertices);
			//Set first FU if necessary
			if (!firstFU) firstFU = fu;
			//Move to next fu
			fu = fu->next;
		}
	}	
}


/***********************************************~***************************************************/


void _CopyTopologyVertex(WSVertexUse *oldVertex, WSVertexUse *newVertex, std::map<WSTopologyShell*,WSTopologyShell*> &shells,
	std::map<WSFaceUse*,WSFaceUse*> &faces, std::map<WSLoopUse*,WSLoopUse*> &loops, std::map<WSEdgeUse*,WSEdgeUse*> &edges,
	std::map<WSVertexUse*,WSVertexUse*> &vertices) {
	//Copy all of the references
	newVertex->point	= oldVertex->point;
	newVertex->shell	= shells.find(oldVertex->shell)->second;
	newVertex->loop		= loops.find(oldVertex->loop)->second;
	newVertex->edge		= edges.find(oldVertex->edge)->second;
	newVertex->next		= vertices.find(oldVertex->next)->second;
	newVertex->prev		= vertices.find(oldVertex->prev)->second;
}


void _CopyTopologyEdge(WSEdgeUse *oldEdge, WSEdgeUse *newEdge, std::map<WSTopologyShell*,WSTopologyShell*> &shells,
	std::map<WSFaceUse*,WSFaceUse*> &faces, std::map<WSLoopUse*,WSLoopUse*> &loops, std::map<WSEdgeUse*,WSEdgeUse*> &edges,
	std::map<WSVertexUse*,WSVertexUse*> &vertices) {
	//Copy all of the references
	newEdge->curve		= oldEdge->curve;
	newEdge->orientation= oldEdge->orientation;
	newEdge->mate		= edges.find(oldEdge->mate)->second;
	newEdge->vertexUse	= vertices.find(oldEdge->vertexUse)->second;
	newEdge->shell		= shells.find(oldEdge->shell)->second;
	newEdge->loop		= loops.find(oldEdge->loop)->second;
	newEdge->cw			= edges.find(oldEdge->cw)->second;
	newEdge->ccw		= edges.find(oldEdge->ccw)->second;
	newEdge->radial		= edges.find(oldEdge->radial)->second;
}


void _CopyTopologyLoop(WSLoopUse *oldLoop, WSLoopUse *newLoop, std::map<WSTopologyShell*,WSTopologyShell*> &shells,
	std::map<WSFaceUse*,WSFaceUse*> &faces, std::map<WSLoopUse*,WSLoopUse*> &loops, std::map<WSEdgeUse*,WSEdgeUse*> &edges,
	std::map<WSVertexUse*,WSVertexUse*> &vertices) {
	//Copy all of the references
	newLoop->face		= faces.find(oldLoop->face)->second;
	newLoop->next		= loops.find(oldLoop->next)->second;
	newLoop->prev		= loops.find(oldLoop->prev)->second;
	newLoop->mate		= loops.find(oldLoop->mate)->second;
	newLoop->edgeUses	= edges.find(oldLoop->edgeUses)->second;
	newLoop->vertexUses	= vertices.find(oldLoop->vertexUses)->second;
}


void _CopyTopologyFace(WSFaceUse* oldFace, WSFaceUse *newFace, std::map<WSTopologyShell*,WSTopologyShell*> &shells,
   std::map<WSFaceUse*,WSFaceUse*> &faces, std::map<WSLoopUse*,WSLoopUse*> &loops, std::map<WSEdgeUse*,WSEdgeUse*> &edges,
   std::map<WSVertexUse*,WSVertexUse*> &vertices) {
	//Copy all of the references
	newFace->surface	= oldFace->surface;
	newFace->orientation= oldFace->orientation;
	newFace->shell		= shells.find(oldFace->shell)->second;
	newFace->next		= faces.find(oldFace->next)->second;
	newFace->prev		= faces.find(oldFace->prev)->second;
	newFace->mate		= faces.find(oldFace->mate)->second;
	newFace->loopUses	= loops.find(oldFace->loopUses)->second;
}


void _CopyTopologyShell(WSTopologyShell *oldShell, WSTopologyShell *newShell, WCTopologyModel *model,
	std::map<WSFaceUse*,WSFaceUse*> &faces, std::map<WSLoopUse*,WSLoopUse*> &loops, std::map<WSEdgeUse*,WSEdgeUse*> &edges,
	std::map<WSVertexUse*,WSVertexUse*> &vertices) {
	//Copy all of the references
	newShell->model		= model;
	newShell->faceUses	= faces.find(oldShell->faceUses)->second;
	newShell->edgeUses	= edges.find(oldShell->edgeUses)->second;
	newShell->vertexUses= vertices.find(oldShell->vertexUses)->second;
}


void _CopyTopologyModel(WCTopologyModel* destination, const std::list<WSTopologyShell*> &source) {
	//Create maps for all children objects
	std::map<WSTopologyShell*,WSTopologyShell*> shells;
	std::map<WSFaceUse*,WSFaceUse*> faces;
	std::map<WSLoopUse*,WSLoopUse*> loops;
	std::map<WSEdgeUse*,WSEdgeUse*> edges;
	std::map<WSVertexUse*,WSVertexUse*> vertices;
	
	//Go through each shell and catalog it
	std::list<WSTopologyShell*>::const_iterator shellIter;
	for (shellIter = source.begin(); shellIter != source.end(); shellIter++) {
		//Catalog the shells and children
		_CatalogTopologyShell(*shellIter, shells, faces, loops, edges, vertices);
	}

	//Loop through all of the shells
	std::map<WSTopologyShell*,WSTopologyShell*>::iterator shellsIter;
	for (shellsIter = shells.begin(); shellsIter != shells.end(); shellsIter++) {
		//Copy the old to the new
		_CopyTopologyShell( (*shellsIter).first, (*shellsIter).second, destination, faces, loops, edges, vertices );
		//Insert the shell into the topology model
		destination->AddShell( (*shellsIter).first );
	}

	//Loop through all of the faces
	std::map<WSFaceUse*,WSFaceUse*>::iterator facesIter;
	for (facesIter = faces.begin(); facesIter != faces.end(); facesIter++) {
		//Copy the old to the new
		_CopyTopologyFace( (*facesIter).first, (*facesIter).second, shells, faces, loops, edges, vertices );
	}

	//Loop through all of the loops
	std::map<WSLoopUse*,WSLoopUse*>::iterator loopsIter;
	for (loopsIter = loops.begin(); loopsIter != loops.end(); loopsIter++) {
		//Copy the old to the new
		_CopyTopologyLoop( (*loopsIter).first, (*loopsIter).second, shells, faces, loops, edges, vertices );
	}

	//Loop through all of the edges
	std::map<WSEdgeUse*,WSEdgeUse*>::iterator edgesIter;
	for (edgesIter = edges.begin(); edgesIter != edges.end(); edgesIter++) {
		//Copy the old to the new
		_CopyTopologyEdge( (*edgesIter).first, (*edgesIter).second, shells, faces, loops, edges, vertices );
	}

	//Loop through all of the vertices
	std::map<WSVertexUse*,WSVertexUse*>::iterator verticesIter;
	for (verticesIter = vertices.begin(); verticesIter != vertices.end(); verticesIter++) {
		//Copy the old to the new
		_CopyTopologyVertex( (*verticesIter).first, (*verticesIter).second, shells, faces, loops, edges, vertices );
	}
}


/***********************************************~***************************************************/


xercesc::DOMElement* _SerializeTopologyVertex(WSVertexUse *vertex, xercesc::DOMDocument *document,
	WCSerialDictionary *dictionary) {
	//Make sure not NULL
	if (!vertex) return NULL;
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(vertex);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("VertexUse");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	
	//Add model attributes
	WCSerializeableObject::AddGUIDAttrib(element, "point", vertex->point, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "shell", vertex->shell, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "loop", vertex->loop, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "edge", vertex->edge, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "next", vertex->next, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "prev", vertex->prev, dictionary);
	//Return the element
	return element;
}


xercesc::DOMElement* _SerializeTopologyEdge(WSEdgeUse *edge, xercesc::DOMDocument *document,
	WCSerialDictionary *dictionary) {
	//Make sure not NULL
	if (!edge) return NULL;
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(edge);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("EdgeUse");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	
	//Add model attributes
	WCSerializeableObject::AddGUIDAttrib(element, "curve", edge->curve, dictionary);
	WCSerializeableObject::AddBoolAttrib(element, "orientation", edge->orientation);
	WCSerializeableObject::AddGUIDAttrib(element, "mate", edge->mate, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "shell", edge->shell, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "loop", edge->loop, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "cw", edge->cw, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "ccw", edge->ccw, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "radial", edge->radial, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "vertexUse", edge->vertexUse, dictionary);
	
	//Add the vertexUse child
	xercesc::DOMElement* child = _SerializeTopologyVertex(edge->vertexUse, document, dictionary);
	//If there is a child, append it
	if (child) element->appendChild(child);
	//Return the element
	return element;
}


xercesc::DOMElement* _SerializeTopologyLoop(WSLoopUse *loop, xercesc::DOMDocument *document,
											WCSerialDictionary *dictionary) {
	//Make sure not NULL
	if (!loop) return NULL;
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(loop);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("LoopUse");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	
	//Add model attributes
	WCSerializeableObject::AddGUIDAttrib(element, "face", loop->face, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "next", loop->next, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "prev", loop->prev, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "mate", loop->mate, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "edgeUses", loop->edgeUses, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "vertexUses", loop->vertexUses, dictionary);
	
	//List all edgeUses
	xercesc::DOMElement* child;
	WSEdgeUse *firstEU = NULL, *eu = loop->edgeUses;
	while (eu != firstEU) {
		//Try to serialize the child topology edge
		child = _SerializeTopologyEdge(eu, document, dictionary);
		//Move to the next edge
		if (!firstEU) firstEU = eu;
		eu = eu->cw;
		//If there is a child, append it
		if (child) element->appendChild(child);
	}
	
	//List all vertexUses
	WSVertexUse *firstVU = NULL, *vu = loop->vertexUses;
	while (vu != firstVU) {
		//Try to serialize the child topology vertex
		child = _SerializeTopologyVertex(vu, document, dictionary);
		//Move to the next vertex
		if (!firstVU) firstVU = vu;
		vu = vu->next;
		//If there is a child, append it
		if (child) element->appendChild(child);
	}
	//Return the element
	return element;
}


xercesc::DOMElement* _SerializeTopologyFace(WSFaceUse *face, xercesc::DOMDocument *document,
											WCSerialDictionary *dictionary) {
	//Make sure not NULL
	if (!face) return NULL;
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(face);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("FaceUse");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	
	//Add model attributes
	WCSerializeableObject::AddGUIDAttrib(element, "surface", face->surface, dictionary);
	WCSerializeableObject::AddBoolAttrib(element, "orientation", face->orientation);
	WCSerializeableObject::AddGUIDAttrib(element, "shell", face->shell, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "next", face->next, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "prev", face->prev, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "mate", face->mate, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "loopUses", face->loopUses, dictionary);
	
	//Serialize all loopUses
	xercesc::DOMElement* child;
	WSLoopUse *firstLU = NULL, *lu = face->loopUses;
	while (lu != firstLU) {
		//Try to serialize the child topology loop
		child = _SerializeTopologyLoop(lu, document, dictionary);
		//Move to the next loop
		if (!firstLU) firstLU = lu;
		lu = lu->next;
		//If there is a child, append it
		if (child) element->appendChild(child);
	}
	
	//Return the element
	return element;
}


xercesc::DOMElement* _SerializeTopologyShell(WSTopologyShell *shell, xercesc::DOMDocument *document,
											 WCSerialDictionary *dictionary) {
	//Make sure not NULL
	if (!shell) return NULL;
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(shell);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("TopologyShell");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::DOMElement* child;
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add model attributes
	WCSerializeableObject::AddGUIDAttrib(element, "model", shell->model, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "faceUses", shell->faceUses, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "edgeUses", shell->edgeUses, dictionary);
	WCSerializeableObject::AddGUIDAttrib(element, "vertexUses", shell->vertexUses, dictionary);
	
	//List all faceUses
	WSFaceUse *firstFU = NULL, *fu = shell->faceUses;
	while (fu != firstFU) {
		//Try to serialize the child topology face
		child = _SerializeTopologyFace(fu, document, dictionary);
		//Move to the next face
		if (!firstFU) firstFU = fu;
		fu = fu->next;
		//If there is a child, append it
		if (child) element->appendChild(child);
	}
	
	//List all edgeUses
	WSEdgeUse *firstEU = NULL, *eu = shell->edgeUses;
	while (eu != firstEU) {
		//Try to serialize the child topology edge
		child = _SerializeTopologyEdge(eu, document, dictionary);
		//Move to the next edge
		if (!firstEU) firstEU = eu;
		eu = eu->cw;
		//If there is a child, append it
		if (child) element->appendChild(child);
	}
	
	//List all vertexUses
	WSVertexUse *firstVU = NULL, *vu = shell->vertexUses;
	while (vu != firstVU) {
		//Try to serialize the child topology vertex
		child = _SerializeTopologyVertex(vu, document, dictionary);
		//Move to the next vertex
		if (!firstVU) firstVU = vu;
		vu = vu->next;
		//If there is a child, append it
		if (child) element->appendChild(child);
	}
	//Return the element
	return element;
}


/***********************************************~***************************************************/


void _PrintTopologyVertex(std::ostream &out, WSVertexUse *vertexUse, const unsigned int &depth) {
	//Do the spacing
	for (unsigned int i=0; i<depth; i++) out << "\t";
	//Print the info
	out << "TopologyVertex (" << vertexUse << ")\n";
}


void _PrintTopologyEdge(std::ostream &out, WSEdgeUse *edgeUse, const unsigned int &depth) {
	//Do the spacing
	for (unsigned int i=0; i<depth; i++) out << "\t";
	//Print the info
	out << "TopologyEdge (*: " << edgeUse << ", Curve: " << edgeUse->curve << ", Orient: " << edgeUse->orientation;
	out << ", CCW: " << edgeUse->ccw << ", CW: " << edgeUse->cw << ", Mate: " << edgeUse->mate << ", Radial: " << edgeUse->radial << ")\n";
	//If there is a curve
	if (edgeUse->curve) {
		//Do sub-spacing
		for (unsigned int i=0; i<=depth; i++) out << "\t";
		out << "Begin: " << edgeUse->curve->Evaluate(0.0) << ", End: " << edgeUse->curve->Evaluate(1.0) << std::endl;
		//		WCGeometricLine *line = dynamic_cast<WCGeometricLine*> (edgeUse->curve);
		//		if (line) out << *line;
		//		else {
		//			WCNurbsCurve *curve = dynamic_cast<WCNurbsCurve*> (edgeUse->curve);
		//			out << *curve;
		//		}
	}
}


void _PrintTopologyLoop(std::ostream &out, WSLoopUse *loopUse, const unsigned int &depth) {
	//Do the spacing
	for (unsigned int i=0; i<depth; i++) out << "\t";
	//Print the info
	out << "TopologyLoop (*: " << loopUse << ", Prev: " << loopUse->prev << ", Next: " << loopUse->next << ", Mate: " << loopUse->mate;
	out << ", E: " << loopUse->edgeUses << ", V: " << loopUse->vertexUses << ")\n";
	//Print the edges, if appropriate
	if (loopUse->edgeUses) {
		WSEdgeUse *sEdge = loopUse->edgeUses, *cEdge = sEdge;
		do {
			//Print the info
			_PrintTopologyEdge(out, cEdge, depth+1);
			//Go to the next one
			cEdge = cEdge->cw;
		} while (cEdge != sEdge);
	}
	//Print the vertices, if appropriate
	if (loopUse->vertexUses) {
		WSVertexUse *sVertex = loopUse->vertexUses, *cVertex = sVertex;
		do {
			//Print the info
			_PrintTopologyVertex(out, cVertex, depth+1);
			//Go to the next one
			cVertex = cVertex->next;
		} while (cVertex != sVertex);		
	}
}


void _PrintTopologyFace(std::ostream &out, WSFaceUse *faceUse, const unsigned int &depth) {
	//Do the spacing
	for (unsigned int i=0; i<depth; i++) out << "\t";
	//Print the info
	out << "TopologyFace (*: " << faceUse << ", Shell: " << faceUse->shell << " Surf: " << faceUse->surface << ", Orient: ";
	out << faceUse->orientation << ", Prev: " << faceUse->prev << ", Next: " << faceUse->next << ", Mate: " << faceUse->mate;
	out << ", L: " << faceUse->loopUses << ")\n";
	//Print the loops, if appropriate
	if (faceUse->loopUses) {
		WSLoopUse *sLoop = faceUse->loopUses, *cLoop = sLoop;
		do {
			//Print the info
			_PrintTopologyLoop(out, cLoop, depth+1);
			//Go to the next one
			cLoop = cLoop->next;
		} while (cLoop != sLoop);
	}
}


void _PrintTopologyShell(std::ostream &out, WSTopologyShell *shell, const unsigned int &depth) {
	//Do the spacing
	for (unsigned int i=0; i<depth; i++) out << "\t";
	//Print the info
	out << "TopologyShell (*: " << shell << ", M: " << shell->model << ", F: " << shell->faceUses << ", E: ";
	out << shell->edgeUses << ", V: " << shell->vertexUses << ")\n";
	//Print the faces, if appropriate
	if (shell->faceUses) {
		WSFaceUse *sFace = shell->faceUses, *cFace = sFace;
		do {
			//Print the info
			_PrintTopologyFace(out, cFace, depth+1);
			//Go to the next one
			cFace = cFace->next;
		} while (cFace != sFace);
	}
	//Print the edges, if appropriate
	if (shell->edgeUses) {
		WSEdgeUse *sEdge = shell->edgeUses, *cEdge = sEdge;
		do {
			//Print the info
			_PrintTopologyEdge(out, cEdge, depth+1);
			//Go to the next one
			cEdge = cEdge->cw;
		} while (cEdge != sEdge);
	}
	//Print the vertices, if appropriate
	if (shell->vertexUses) {
		WSVertexUse *sVertex = shell->vertexUses, *cVertex = sVertex;
		do {
			//Print the info
			_PrintTopologyVertex(out, cVertex, depth+1);
			//Go to the next one
			cVertex = cVertex->next;
		} while (cVertex != sVertex);		
	}
}


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

