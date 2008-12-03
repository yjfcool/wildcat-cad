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
#include "Topology/topology_model_internal.h"
#include "Topology/topology_types.h"


/***********************************************~***************************************************/



void _CatalogEdges(WSEdgeUse* edge, std::list<WSEdgeUse*> &edges, std::list<WSVertexUse*> &vertices) {
	//Catalog edgeUse
	ASSERT(edge);
	edges.push_back( edge );
	//Catalog the child vertexUse
	if( edge->vertexUse ) vertices.push_back( edge->vertexUse );
}


void _CatalogLoops(WSLoopUse* loop, std::list<WSLoopUse*> &loops, std::list<WSEdgeUse*> &edges, std::list<WSVertexUse*> &vertices) {
	//Catalog the loopUse
	ASSERT( loop );
	loops.push_back( loop );
	
	//See if children are vertices
	if (loop->vertexUses) {
		//Catalog all children objects
		WSVertexUse *firstVU = NULL, *vu = loop->vertexUses;
		while (firstVU != vu) {
			//Recursively get all children, starting with this vertex
			vertices.push_back( vu );
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
			_CatalogEdges(eu, edges, vertices);
			//Set first EU if necessary
			if (!firstEU) firstEU = eu;
			//Move to next eu
			eu = eu->cw;			
		}
	}
}


void _CatalogFaces(WSFaceUse* face, std::list<WSFaceUse*> &faces, std::list<WSLoopUse*> &loops, 
	std::list<WSEdgeUse*> &edges, std::list<WSVertexUse*> &vertices) {
	//Catalog the faceUse
	ASSERT( face );
	faces.push_back( face );
	
	//Catalog all children objects
	WSLoopUse *firstLU = NULL, *lu = face->loopUses;
	while (firstLU != lu) {
		//Recursively get all children, starting with this loop
		_CatalogLoops(lu, loops, edges, vertices);
		//Set first LU if necessary
		if (!firstLU) firstLU = lu;
		//Move to next lu
		lu = lu->next;
	}
}


void _CatalogShells(WSTopologyShell *shell, std::list<WSTopologyShell*> &shells,
	std::list<WSFaceUse*> &faces, std::list<WSLoopUse*> &loops, std::list<WSEdgeUse*> &edges, std::list<WSVertexUse*> &vertices) {
	//Catalog the shell
	ASSERT( shell );
	shells.push_back( shell );
	
	//See if vertexUses is non-NULL
	if (shell->vertexUses) {
		//Catalog the vertex
		vertices.push_back( shell->vertexUses );
	}
	
	//See if edgeUses is non-NULL
	if (shell->edgeUses) {
		//Catalog all children objects
		WSEdgeUse *firstEU = NULL, *eu = shell->edgeUses;
		while (firstEU != eu) {
			//Recursively get all children, starting with this edge
			_CatalogEdges(eu, edges, vertices);
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
			_CatalogFaces(fu, faces, loops, edges, vertices);
			//Set first FU if necessary
			if (!firstFU) firstFU = fu;
			//Move to next fu
			fu = fu->next;
		}
	}	
}


/***********************************************~***************************************************/


WCTopologyModel* WCTopologyModel::Union(WCTopologyModel *model) {
	//See if no shells are in the shell list
	if (this->_shellList.empty()) {
		//Just copy the model in
		_CopyTopologyModel(this, model->_shellList);
		return this;
	}
	//Otherwise, need to do complex union
	else {
		std::cout << "Union!!!\n";
		//Create lists for all children objects
		std::list<WSTopologyShell*> leftShells, rightShells;
		std::list<WSFaceUse*> leftFaces, rightFaces;
		std::list<WSLoopUse*> leftLoops, rightLoops;
		std::list<WSEdgeUse*> leftEdges, rightEdges;
		std::list<WSVertexUse*> leftVertices, rightVertices;

		//Go through each shell in the LEFT model and catalog it
		std::list<WSTopologyShell*>::const_iterator shellIter;
		for (shellIter = this->_shellList.begin(); shellIter != this->_shellList.end(); shellIter++) {
			//Catalog the shells and children
			_CatalogShells(*shellIter, leftShells, leftFaces, leftLoops, leftEdges, leftVertices);
		}

		std::cout << "Left Shells: " << leftShells.size() << std::endl;
		std::cout << "Left Faces: " << leftFaces.size() << std::endl;
		std::cout << "Left Loops: " << leftLoops.size() << std::endl;
		std::cout << "Left Edges: " << leftEdges.size() << std::endl;
		std::cout << "Left Vertices: " << leftVertices.size() << std::endl;

		//Go through each shell in the RIGHT model and catalog it
		for (shellIter = model->_shellList.begin(); shellIter != model->_shellList.end(); shellIter++) {
			//Catalog the shells and children
			_CatalogShells(*shellIter, rightShells, rightFaces, rightLoops, rightEdges, rightVertices);
		}
		
		std::cout << "Right Shells: " << rightShells.size() << std::endl;
		std::cout << "Right Faces: " << rightFaces.size() << std::endl;
		std::cout << "Right Loops: " << rightLoops.size() << std::endl;
		std::cout << "Right Edges: " << rightEdges.size() << std::endl;
		std::cout << "Right Vertices: " << rightVertices.size() << std::endl;
	}

	//Return this for now
	return this;
}


/***********************************************~***************************************************/

