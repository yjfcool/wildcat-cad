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


#ifndef __TOPOLOGY_TYPES_H__
#define __TOPOLOGY_TYPES_H__


/*** Included Header Files ***/
#include "Topology/wtpkl.h"


/*** Locally Defined Values ***/
enum WCTopologyType {
	SolidModel,
	WireFrame
};


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {
	
	
/*** Class Predefines ***/
class WCGeometricPoint;
class WCGeometricCurve;
class WCGeometricSurface;
class WCTopologyModel;
struct WSFaceUse;
struct WSLoopUse;
struct WSEdgeUse;
struct WSVertexUse;


/***********************************************~***************************************************/


struct WSTopologyShell {
	WCTopologyModel								*model;												//!< Parent topology model
	WSFaceUse									*faceUses;											//!< List of face uses (solid model)
	WSEdgeUse									*edgeUses;											//!< List of edge uses (wireframe model)
	WSVertexUse									*vertexUses;										//!< List of vertex uses (not sure)
	WSTopologyShell() : model(NULL), faceUses(NULL), edgeUses(NULL), 	//!< Default constructor
												vertexUses(NULL) { }
};


/***********************************************~***************************************************/


struct WSFaceUse {
	WCGeometricSurface							*surface;											//!< Associated geometric surface
	bool										orientation;										//!< Orientation compared to surface
	WSTopologyShell								*shell;												//!< Parent shell
	WSFaceUse									*next, *prev;										//!< Next and previous faces in the shell
	WSFaceUse									*mate;												//!< Other side of the face
	WSLoopUse									*loopUses;											//!< List of LoopUses
	WSFaceUse() : surface(NULL), orientation(true), shell(NULL), next(NULL), prev(NULL), mate(NULL), loopUses(NULL) { }	//!< Default constructor
};


/***********************************************~***************************************************/


struct WSLoopUse {
	WSFaceUse									*face;												//!< Parent face use
	WSLoopUse									*next, *prev;										//!< Next and previous loops in face
	WSLoopUse									*mate;												//!< Other side of the loop
	WSEdgeUse									*edgeUses;											//!< List of EdgeUses
	WSVertexUse									*vertexUses;										//!< List of VertexUses
	WSLoopUse() : face(NULL), next(NULL), prev(NULL), mate(NULL), edgeUses(NULL), vertexUses(NULL) { }	//!< Default constructor
};


/***********************************************~***************************************************/


struct WSEdgeUse {
	WCGeometricCurve							*curve;												//!< Associated geometric curve
	bool										orientation;										//!< Orientation compared to curve
	WSEdgeUse									*mate;												//!< Other side of the edge
	WSVertexUse									*vertexUse;											//!< Downward pointer to starting VertexUse
	//Elements if wireframe
	WSTopologyShell								*shell;												//!< Parent shell
	//Elements if solid model
	WSLoopUse									*loop;												//!< Parent loop
	WSEdgeUse									*cw, *ccw;											//!< Clock-wise and counter clockwise lists
	WSEdgeUse									*radial;											//!< Pointer to the next radial EdgeUse
	WSEdgeUse() : curve(NULL), orientation(true), mate(NULL), vertexUse(NULL), shell(NULL),			//!< Default constructor
												loop(NULL), cw(NULL), ccw(NULL), radial(NULL) { }
};


/***********************************************~***************************************************/


struct WSVertexUse {
	WCGeometricPoint							*point;												//!< Associated geometric point
	WSTopologyShell								*shell;												//!< Parent shell
	WSLoopUse									*loop;												//!< Parent loop
	WSEdgeUse									*edge;												//!< Parent edge
	WSVertexUse									*next, *prev;										//!< Next and previous vertices in parent
	WSVertexUse() : point(NULL), shell(NULL), loop(NULL), edge(NULL), next(NULL), prev(NULL) { }	//!< Default constructor
};


/***********************************************~***************************************************/

	
}	   // End Wildcat Namespace
#endif //__TOPOLOGY_TYPES_H__

