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
#include "Constraint/constraint_planner.h"
#include "Constraint/constraint_node.h"
#include "Constraint/constraint_edge.h"
#include "Constraint/constraint_cluster.h"
#include "Constraint/bipartite_flow_network.h"


/*** Locally Defined Values ***/
#define CONSTRAINTPLANNER_DCONST_ZERO			0
#define CONSTRAINTPLANNER_DCONST_2D				3
#define CONSTRAINTPLANNER_DCONST_3D				6



/*****************************************************************************/


WCConstraintPlan::~WCConstraintPlan() {
	std::list<WCConstraintCluster*>::iterator cIter;
	//Delete all of the clusters within the plan
	for (cIter = this->_clusters.begin(); cIter != this->_clusters.end(); cIter++) {
		if ((*cIter) != NULL) delete (*cIter);
	}
}


void WCConstraintPlan::AddCluster(WCConstraintCluster* cluster) {
	//Add the new cluster to the list
	this->_clusters.push_back(cluster);
}


std::ostream& operator<<(std::ostream& out, const WCConstraintPlan &plan) {
	std::list<WCConstraintCluster*>::const_iterator cIter;
	out << "Constraint Plan(" << &plan << ": ";
	//Show plan status
	if (plan.Status() == WCPlanStatus::UnderConstrained()) out << "Under Constrained\n";
	if (plan.Status() == WCPlanStatus::WellConstrained()) out << "Well Constrained\n";
	if (plan.Status() == WCPlanStatus::OverConstrained()) out << "Over Constrained\n";
	//Print all of the clusters within the plan
	for (cIter = plan._clusters.begin(); cIter != plan._clusters.end(); cIter++) {
		if ((*cIter) != NULL) out << *(*cIter);
	}
	//Return
	return out;
}


/***********************************************~***************************************************/


void WCConstraintPlanner::ResetMarks(std::list<WCConstraintNode*> &list) {
	std::list<WCConstraintNode*>::iterator nIter;
	//Reset the marks for all of the list's nodes
	for (nIter = list.begin(); nIter != list.end(); nIter++)
		(*nIter)->ResetMark();
	//Sort and reverse the node list
	list.sort(WCConstraintNode::MarkSort);
}


/*** To find a cluster we repeatedly insert nodes into the bfNetwork and try to distribute the weight
*	of incident edges.  If not all weight can be distributed, then a cluster is found.  In this case,
*	create a new cluster, and add all nodes that are labeled in the bfNetwork.
*/
WCConstraintCluster* WCConstraintPlanner::FindCluster(const WPUInt &dConst) {
	//Copy the node list
	std::list<WCConstraintNode*> nodeList(this->_nodeList);
	WCConstraintCluster* newCluster = NULL;
	std::list<WCConstraintNode*>::iterator iter;
	WCConstraintNode* node;
	int retVal;
	
	//Convert the D geometric constant into the K constant
	int kConst = -1 * (dConst + 1);
	
	//Reset and sort the node list
	this->ResetMarks(nodeList);
	//Get a reference to the first node to be added into gPrime
	node = nodeList.front();
	nodeList.pop_front();
	//Mark all of this node's neighbors
	node->MarkNeighbors();
	nodeList.sort(WCConstraintNode::MarkSort);

	//Create a new bipartite flow network
	if (this->_network != NULL) delete this->_network;
	this->_network = new WCBipartiteFlowNetwork();
	//Add in the first node
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintPlanner::FindCluster - Adding Node:" << node);
	this->_network->AddVertexNode(node, node->Weight());
	
	//Go through the rest of the nodes until a node has an edge that can't be distributed
	while (nodeList.size() > 0) {
		//Get a reference to the next node to be added into gPrime
		node = nodeList.front();
		CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintPlanner::FindCluster - Adding Node:" << node);
		//Try to distribute the edges, if not we are done
		retVal = node->DistributeEdges(this->_network, kConst);
		if (retVal > 0) {
			CLOGGER_DEBUG(WCLogManager::RootLogger(), "Dense subgraph found...creating cluster...")
			//Create the new cluster from the labeled vertices in the bipartite flow network
			newCluster = new WCConstraintCluster( this->_network->LabeledVertices(), dConst);
			//Check to make sure that the new cluster is valid
			if (!newCluster->IsValidCluster()) {
				CLOGGER_ERROR(WCLogManager::RootLogger(), "CConstraintPlanner::FindCluster - Invalid cluster found.");
				//Make sure to delete invalid clusters
				delete newCluster;
				return NULL;
			}	
			return newCluster;
		}
		nodeList.pop_front();
		//Mark all of this node's neighbors
		node->MarkNeighbors();
		//Sort and reverse the list
		nodeList.sort(WCConstraintNode::MarkSort);
	}
	//No cluster found...graph must be under constrained...
	CLOGGER_INFO(WCLogManager::RootLogger(), "CConstraintPlanner::FindCluster - No cluster found.  Graph underconstrained");
	return NULL;
}


/*** The cluster has been created to make reducing the main graph somewhat straightforward.
   * First we add all of the new elements into the graph (core node, new edges).  Then we
   * remove all of the old elements.  The internal nodes are removed.  Internal edges are handled
   * slightly differently than boundry edges.  Internal edges are just removed from the graph's
   * master list.  They are not removed from the edge lists of the internal nodes.  This should ease
   * the solution computation for the cluster.  Boundry nodes are removed from the graph's master
   * list and from the edge lists of both the internal and boundry nodes.
   */
void WCConstraintPlanner::ReduceByCluster(WCConstraintCluster* newCluster) {
	std::list<WCConstraintNode*>::iterator nIter;
	std::list<WCConstraintEdge*>::iterator eIter;

	//Add the new core node into the graph
	this->_nodeList.push_back(newCluster->_core);
	//Add the new edges into the graph
	for (eIter = newCluster->_newEdges.begin(); eIter != newCluster->_newEdges.end(); eIter++) {
		//Add the new edge to the master list
		this->_edgeList.push_back( *eIter );
		//Add the new edge to the new core node
		newCluster->_core->AddEdge( *eIter );
		//Add the new edge to the old frontier node
		(*eIter)->Node(newCluster->_core)->AddEdge( *eIter);
	}

	//Remove all of the internal nodes
	for (nIter = newCluster->_internal.begin(); nIter != newCluster->_internal.end(); nIter++) {
		this->_nodeList.remove( *nIter );
	}
	//Remove all of the internal edges (but only from the master list)
	for (eIter = newCluster->_internalEdges.begin(); eIter != newCluster->_internalEdges.end(); eIter++) {
		this->_edgeList.remove( *eIter );
	}
	//Remove all of the boundry edges (but also from the edge lists of the boundry and internal nodes)
	for (eIter = newCluster->_boundryEdges.begin(); eIter != newCluster->_boundryEdges.end(); eIter++) {
		( *eIter )->Extract();
	}
	
}


/***********************************************~***************************************************/


WCConstraintPlanner::~WCConstraintPlanner() {
	std::list<WCConstraintNode*>::iterator nIter;
	std::list<WCConstraintEdge*>::iterator eIter;

	//Delete all of the nodes
	for (nIter = this->_nodeList.begin(); nIter != this->_nodeList.end(); nIter++) {
		delete (*nIter);
	}
	//Delete all of the edges
	for (eIter = this->_edgeList.begin(); eIter != this->_edgeList.end(); eIter++) {
		delete (*eIter);
	}
	//Finally, delete the child Bipartite Flow network (if it exists)
	if (this->_network != NULL)
		delete this->_network;
}
	
	
WCConstraintNode* WCConstraintPlanner::AddNode(WCSketchFeature *feature, const WCConstraintNodeType &type,
	const WPUInt &weight, const WCVector4 &data) {
	//Create the new constraint node
	WCConstraintNode* node = new WCConstraintNode(feature, type, weight, data);
	//Add the node into the adjacency list
	this->_nodeList.push_back(node);
	//Mark the planner as dirty
	this->_isDirty = true;
	//Return the new node
	return node;
}


bool WCConstraintPlanner::RemoveNode(WCConstraintNode *node) {
	return false;
}


WCConstraintEdge* WCConstraintPlanner::AddEdge(WCSketchConstraint* constraint, WCConstraintNode* nodeA, WCConstraintNode* nodeB,
	const WCConstraintEdgeType &type, const WPUInt &weight, const WCVector4 &data) {
	//Make sure that non-null nodes were passed
	if ((nodeA != NULL) && (nodeB != NULL)) {
		//Create the new edge
		WCConstraintEdge* edge = new WCConstraintEdge(constraint, nodeA, nodeB, type, weight, data);
		//Add the edge to the edge list and with the nodes
		nodeA->AddEdge(edge);
		nodeB->AddEdge(edge);
		this->_edgeList.push_back(edge);
		//this->_isDirty = true;
		return edge;
	}
	else {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintPlanner::AddConstraint - Invalid node passed");
//		throw WCException("WCConstraintPlanner::AddConstraint - Invalid node passed", __LINE__, __FILE__);
		return NULL;
	}
}


bool WCConstraintPlanner::RemoveEdge(WCConstraintEdge *edge) {
	return false;
}


/*	This function calculates the density of the entire graph.
*	The density is the sum of the edge weights
*	minus the sum of the vertex weights (see Density definition
*	on p4 of "Finding Solvable Subsets of Constraint Graphs"
*/
WPInt WCConstraintPlanner::Density(void) const {
	WPInt sum = 0;
	std::list<WCConstraintNode*>::const_iterator nIter;
	std::list<WCConstraintEdge*>::const_iterator eIter;
	
	//Get the total weight of the edges in the graph
	for (eIter = this->_edgeList.begin(); eIter != this->_edgeList.end(); eIter++)
		sum += (*eIter)->Weight();
	//Subtract the weights of the nodes
	for (nIter = this->_nodeList.begin(); nIter != this->_nodeList.end(); nIter++)
		sum -= (*nIter)->Weight();
	//Return the density of the graph
	return sum;
}


WCConstraintPlan* WCConstraintPlanner::Solve(void) {
	WCConstraintPlan* plan = new WCConstraintPlan();
	WCConstraintCluster* newCluster;
	
	//Check to see if the overall constraint graph is over constrained
	WPInt density = this->Density();
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintPlanner::Solve - Density:" << density);
	if (density > CONSTRAINTPLANNER_DCONST_ZERO) {
		CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintPlanner::Solve - Primary over-constrained check failed.");
		//Set the plan status as over constrained
		plan->Status(WCPlanStatus::OverConstrained());
		return plan;
	}
	//Check for single node case
	//...
	//Continue to iterate until only one node is left in the graph
	while (this->_nodeList.size() > 1) {
		//Get a dense minimal cluster from the graph
		newCluster = this->FindCluster(CONSTRAINTPLANNER_DCONST_ZERO);
		//Check to see if a valid cluster was found
		if (newCluster == NULL) {
			plan->Status(WCPlanStatus::UnderConstrained());
			CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintPlanner::Solve - No additional dense subgraphs found - underconstrained");
			return plan;
		}
		//Add the new cluster into the overall solution plan
		plan->AddCluster(newCluster);
		//Apply the Extended Dense Minimal Graph (EDMG) back to the clone
		this->ReduceByCluster(newCluster);
		std::cout << "WCConstraintPlanner::Solve - " << *this;
	}
	//Set the plan status to well constrained
	plan->Status(WCPlanStatus::WellConstrained());
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintPlanner::Solve - Complete solution has been found");
	//Mark as clean
	this->_isDirty = false;
	//Return the plan
	return plan;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCConstraintPlanner &planner) {
	std::list<WCConstraintNode*>::const_iterator nodeIter;
	std::list<WCConstraintEdge*>::const_iterator edgeIter;
	WPInt density = planner.Density();
		
	//Print some info about this graph
	out << "ConstraintPlanner(" << &planner << ") Density: " << density << " ---\n";
	
	//Now print the Node list
	out << "\t--- Nodes ---\n";
	for (nodeIter = planner._nodeList.begin(); nodeIter != planner._nodeList.end(); nodeIter++) {
		out << "\t" << *(*nodeIter);
	}
	//Now print the Edge list
	out << "\t--- Edges ---\n";
	for (edgeIter = planner._edgeList.begin(); edgeIter != planner._edgeList.end(); edgeIter++) {
		out << "\t" << *(*edgeIter);
	}

	//Return the stream
	return out;
}


/***********************************************~***************************************************/

