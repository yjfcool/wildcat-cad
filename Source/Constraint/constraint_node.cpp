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


/***Included Header Files ***/
#include "Constraint/constraint_node.h"
#include "Constraint/constraint_edge.h"
#include "Constraint/bipartite_flow_network.h"
#include "Constraint/sketch_constraint.h"


/***********************************************~***************************************************/


WCConstraintNode::WCConstraintNode(WCSketchFeature *feature, const WCConstraintNodeType &type, 
	const WPUInt &weight, const WCVector4 &data) :
	_type(type), _data(data), _weight(weight), _mark(0), _feature(feature), _cluster(NULL), _flag(false) {
	//Nothing else to do for now
}


WCConstraintNode::WCConstraintNode(WCConstraintCluster* cluster, const WPUInt &weight) :
	_type(WCConstraintNodeType::Point()), _data(), _weight(weight), _mark(-1), _feature(NULL), _cluster(cluster), _flag(false) {
	//Nothing else to do for now
}


void WCConstraintNode::MarkNeighbors(void) {
	std::list<WCConstraintEdge*>::iterator eIter;
	//Loop through the edge list and call Mark() on all of the neighbor nodes
	for (eIter = this->_edgeList.begin(); eIter != this->_edgeList.end(); eIter++)
		(*eIter)->Node(this)->Mark();
}


void WCConstraintNode::ResetMark(void) {
	//Give a slight bias towards non-cluster nodes (they may have not been processed yet)
	if (this->_cluster != NULL) this->_mark = -1;
	else this->_mark = 0;
}


bool WCConstraintNode::MarkSort(WCConstraintNode* nodeA, WCConstraintNode* nodeB) {
	return (nodeA->_mark > nodeB->_mark);
}


/*** Add a new node into the bfNetwork and try to distribute its edges (and their weight)
*	out into the rest of the network.  This is taken from "Finding Solveable Subsets of
*	Constraint Graphs" Algorithm Dense on page 7.
*
*	Steps ---
*	1: Add the node into the network and add all incident edges that connect to nodes already in the network
*	2: Try to distribute edge weights into the network - see documentation on 3 paths below
***/
int WCConstraintNode::DistributeEdges(WCBipartiteFlowNetwork* bfNetwork, const WPUInt &kConst) {
	std::list<WCConstraintEdge*>::iterator iter;
	int count=0, residual;
	std::queue<WCBipartiteEdgeNode*> edgeQ;
	WCConstraintEdge* edge;
	WCBipartiteVertexNode *oldNode=NULL, *newNode=NULL;
	WCBipartiteEdgeNode *edgeNode=NULL;
	
	//Create the new vertex node in gPrime
	newNode = bfNetwork->AddVertexNode(this, this->_weight);
	//Sort the edges in ascending weight order (std only does ascending order)
	this->_edgeList.sort(WCConstraintEdge::EdgeSort);
	
	//Now add in all of the edges between this node and the current nodes in the flow network
	for (iter = this->_edgeList.begin(); iter != this->_edgeList.end(); iter++) {
		edge = *iter;
		//Check to see if the other node the vertex points to is already in gPrime
		oldNode = bfNetwork->VertexNodeByReference( edge->Node(this) );
		if (oldNode != NULL) {
			//Add the edge (as an edge node) into BFNetwork
			edgeNode = bfNetwork->AddEdgeNode(edge, edge->Weight());
			//Now connect this edge node with the new vertex node
			bfNetwork->AddEdge(edgeNode, newNode);
			//Next connect the edge node with the old vertex node
			bfNetwork->AddEdge(edgeNode, oldNode);
			//And add the edge to our edge list for later use
			edgeQ.push(edgeNode);
		}
	}

	//Three possible cases for node edge distribution, all based on the value of kConst and edge->weight
	//Depending on these values, one of the three cases will execute.  This allows for variable kConst values
	//Case 1 will never be encountered because our K values will always be < 0  (Because d = 0, 3, 6)
	int adjustment = this->Weight() + kConst;
	//Case 2: For if node->weight >= abs(kConst) - this is the primary case that will be encountered
	if (adjustment >= 0) {
		//Now try to distribute the weight for all of the edges
		newNode->Weight(adjustment);
		while(edgeQ.size() != 0) {
			//Get the next edge to be processed
			edgeNode = edgeQ.front();
			edgeQ.pop();
			//Try to distribute the edge's weight
			residual = bfNetwork->Distribute(edgeNode);
			if(residual > 0) {
				std::cout << *bfNetwork;
				//A dense subgraph has been found
				CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintNode::DistributeEdges - Case 2: Dense subgraph found");
				//Reset the node's weight
				newNode->Weight(this->_weight);
				return residual;
			}			
		}		
		//otherwise reset the weight and move on to the next node...
		newNode->Weight(this->_weight);
	}
	
	//Case 3: For if the node->weight < abs(kConst)
	else {
		for (; count > 0; count--) {
			//Get the next edge to be processed
			edgeNode = edgeQ.front();
			edgeQ.pop();
			//Set the node's weight to zero
			newNode->Weight(0);
			//Try to distribute the edge's weight
			residual = bfNetwork->Distribute(edgeNode);
			if(residual > 0) {
				//A dense subgraph has been found
				CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintNode::DistributeEdges - Case 3 - Type 1: Dense subgraph found");
				//Reset the node's weight value
				newNode->Weight(this->_weight);
				return residual;
			}
			else {
				bfNetwork->Snapshot();
				//Change the edge's weight and try again
				edgeNode->Weight(edgeNode->Weight() - adjustment);
				residual = bfNetwork->Distribute(edgeNode);
				if (residual > 0) {
					//A dense subgraph has been found
					CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintNode::DistributeEdges - Case 3 - Type 2: Dense subgraph found");
					//Return the edge's weight value
					edgeNode->Weight(edgeNode->Weight() - adjustment);
					//Reset the node's weight value
					newNode->Weight(this->_weight);
					return residual;					
				}
				//No dense subgraph found...
				bfNetwork->Restore();
				//Change the edge's weight back to its original value
				edgeNode->Weight(edgeNode->Weight() + adjustment);
			}			
		}
		//Reset the node's weight value before moving on to the next vertex node...
		newNode->Weight(this->_weight);
	}
	//No dense subgraph found, just move on to the next node...
	return residual;
}
	

/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCConstraintNode &node) {
	std::list<WCConstraintEdge*>::const_iterator iter;
	
	//Show some info about the node
	out << "ConstraintNode(" << &node << ")::(W:" << node._weight;
	//Only print the tag if it is non-NULL
	if (node._feature != NULL) {
		out << ", " << node._feature->GetName();
	}
	if (node._cluster != NULL) {
		out << ", Cluster:" << node._cluster;
	}
	out << ")\n";
	
	//Display a list of the edges
	for (iter = node._edgeList.begin(); iter != node._edgeList.end(); iter++) {
		out << "\t\t" << *iter << std::endl;;
	}
	//Return
	return out;
}


/***********************************************~***************************************************/

