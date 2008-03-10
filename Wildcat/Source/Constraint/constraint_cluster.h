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


#ifndef __CONSTRAINT_CLUSTER_H__
#define __CONSTRAINT_CLUSTER_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"


/*** Locally Defined Values ***/
//None


/*** Class Predefinitions ***/
class WCConstraintNode;
class WCConstraintEdge;


/***********************************************~***************************************************/


class WCConstraintCluster {
protected:
	WCConstraintNode*							_core;												//!< The new core node created from the cluster
	std::list<WCConstraintNode*>				_frontier;											//!< List of nodes classified as frontier
	std::list<WCConstraintNode*>				_internal;											//!< List of nodes classified as internal
	std::list<WCConstraintNode*>*				_original;											//!< The original list of nodes in the cluster
	std::list<WCConstraintCluster*>				_clusters;											//!< List of subclusters that are part of the internal nodes
	std::list<WCConstraintEdge*>				_newEdges;											//!< List of new edges connecting core node to frontier nodes
	std::list<WCConstraintEdge*>				_boundryEdges;										//!< List of old edges connecting internal to frontier nodes
	std::list<WCConstraintEdge*>				_internalEdges;										//!< List of old edges connecting only internal nodes
	WPUInt										_depth;												//!< Depth of this cluster = max(_clusters.depth)

	//Private Methods
	void CategorizeNodesAndExpand(void);															//!< Expand the cluster and determine frontier or internal
	void CategorizeEdges(void);																		//!< Evaluate all internal node edges, return core weight

	//Friend Declarations
	friend class WCConstraintPlanner;																//!!< Make constraint planner a friend
public:
	//Constructors and Destructors
	WCConstraintCluster(std::list<WCConstraintNode*>* vertices, const WPUInt &dConst);				//!< Default constructor
	~WCConstraintCluster();																			//!< Default destructor

	//Member Access Methods
	int Density(void);																				//!< Get the density of this cluster
	inline WPUInt Depth(void)					{ return this->_depth; }							//!< Get the depth of the cluster
	inline bool IsValidCluster(void)			{ return (this->_core != NULL); }					//!< For now the cluster is only valid if there is a core
	bool DepthSort(const WCConstraintCluster* clusterA, const WCConstraintCluster* clusterB);		//!< Cluster comparison function for sorting a list of clusters								

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCConstraintCluster &cluster);			//!< Overloaded output operator
};


/***********************************************~***************************************************/


#endif //__CONSTRAINT_CLUSTER_H__

