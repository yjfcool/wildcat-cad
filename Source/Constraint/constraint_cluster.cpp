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
#include "Constraint/constraint_cluster.h"
#include "Constraint/constraint_planner.h"
#include "Constraint/constraint_node.h"
#include "Constraint/constraint_node.h"


/***********************************************~***************************************************/

/*** Decription of the CategorizeNodesAndExpand algorithm
   * This function needs to take the original list of cluster nodes and edges (from Algorithm Dense)
   * and expand the cluster as much as possible and also categorize the nodes as either Frontier or Internal.
   * Three queues are used for this:
   *		fList: a queue of nodes that need to be evaluated for being frontier or not 
   *		eList: a queue of nodes that need to be evaluated to see if they are expansion nodes or not
   *        edgeList: a queue of edges that "implicated" a node as being a possible expansion node
   *
   * A map (hash) is also used to help speed the evaluation of whether the node is in the cluster already or not.  All of
   * the original nodes are placed into the hash.
   *
   * All original nodes are also put onto the fList for categorization, and their flag is set to true (i.e it is on the fList).
   * Now each node on the fList queue is processed to see if all of its edges point into the cluster (internal) or out of
   * the cluster (frontier).  
   *		If an edge points out of the cluster then the node it points to is put onto the eList and that
   *		edge is added to the edgeList.  The node is then put into the Frontier list.
   *
   *		If all edges point within the cluster then the node is put into the Internal list.  We must check to see if
   *		this new internal node is a simplifed cluster (by checking its Cluster value).  If so add it to the cluster list
   *		and check the cluster's depth value.  Our new cluster must have a greater value.  Increment depth if needed.
   * Remove the node from the fList.
   *
   * Once the node has been processed any nodes in the eList (possible expansions) are evaluated.  A check of all its edges
   * is performed to see if any lead back to the cluster.  If the weight of the "implicating" edge plus the new edge minus
   * the weight of the node equals zero (standard density calculation) then the node should be included as an extension.
   *		In this case we have to reevaluate some of the previous nodes (as they may no longer be frontier nodes).  We scan
   *		through the expansion node's edge again and mark all nodes that are in the cluster.  We removed those nodes from
   *		the frontier list and place them back onto the fList for evaluation.  The node's flag helps us keep track of only
   *		placing it onto the fList once.
   *
   * After both the fList and the eList are empty all possible expansion nodes have been evaluated and all cluster nodes
   * have been categorized as either Frontier or Internal.
   *
   * Finally, return the depth of the cluster.
   */
void WCConstraintCluster::CategorizeNodesAndExpand(void) {
	WPUInt depth=0;
	bool frontier, extension;
	WCConstraintNode* node;
	WCConstraintEdge* edge;
	std::map<WCConstraintNode*, bool> hash;
	std::queue<WCConstraintNode*> fList, eList;
	std::queue<WCConstraintEdge*> cList;
	std::list<WCConstraintEdge*> edgeList;
	std::list<WCConstraintEdge*>::iterator eIter;
	std::list<WCConstraintNode*>::iterator vIter;
	
	//Using the original cluster node list, load hash table for future lookups and load the frontier evaluation queue
	for (vIter = this->_original->begin(); vIter != this->_original->end(); vIter++) {
		hash[*vIter] = false;
		fList.push(*vIter);
		//Make note that the node is on the fList (will use this later)
		(*vIter)->Flag(true);
	}	

	//Continue processing until the frontier queue is empty
	while (fList.size() > 0) {
		//First process any frontier evaluations
		//Get the node to be processed
		node = fList.front();
		fList.pop();
		//Note that the node has been taken off of the vList
		node->Flag(false);
		frontier = false;
		//Get the edge list for the node
		edgeList = node->EdgeList();
		//Now look at every edge in the vertex to see if it points into or out of the cluster
		for (eIter = edgeList.begin(); eIter != edgeList.end(); eIter++) {
			//Check to see if the edge points into or out of the cluster
			if ( hash.find( (*eIter)->Node(node) ) == hash.end() ) {
				//Note that this node is a frontier node
				frontier = true;
				//Now add the node to the extension list
				eList.push( (*eIter)->Node(node) );
				//And add the edge to the companion list
				cList.push( *eIter );
			}
		}
			
		//If the node is a frontier node add it to the proper list
		if (frontier) {
			CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintCluster::CategorizeNodesAndExpand - Frontier node: " << node);
			this->_frontier.push_back(node);
		}
		//Otherwise it is an internal node.  Add it to the list
		else {
			CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintCluster::CategorizeNodesAndExpand - Internal node: " << node);
			this->_internal.push_back(node);
			//Check to see if the internal node is a cluster node
			if (node->Cluster() != NULL) {
				//If it is add it to the cluster list
				this->_clusters.push_back( node->Cluster() );
				//And check to see if the depth value needs to be changed
				depth = STDMAX( depth, node->Cluster()->Depth() );
			}
		}
		
		//Now consider extending the cluster with any possible extension nodes
		while (eList.size() > 0) {
			//Get the node to evaluate for extension
			node = eList.front();
			eList.pop();
			//Get the edge that caused addition to the extension list
			edge = cList.front();
			cList.pop();
			//Get the edge list for the node
			edgeList = node->EdgeList();
			extension = false;
			//Check all edges to see if they lead back to the cluster and meet the density requirements
			for (eIter = edgeList.begin(); eIter != edgeList.end(); eIter++) {
				//Don't check the referring edge and see if the edge leads back to the cluster
				if ( (edge != *eIter) && (hash.find( (*eIter)->Node(node) ) != hash.end()) ) {
					//If this node is an extension
					if (edge->Weight() + (*eIter)->Weight() - node->Weight() == 0) {
						CLOGGER_INFO(WCLogManager::RootLogger(), "WCConstraintCluster::CategorizeNodesAndExpand - Extension found: " << node);
						//Include the new node into the hash
						hash[node] = false;
						//Add the node for Frontier evaluation
						fList.push(node);
						node->Flag(true);
						extension = true;
					}
				}
			}
			//Now if this is an extension node, must find all nodes in the cluster and re-evaluate them
			if (extension) {
				for (eIter = edgeList.begin(); eIter != edgeList.end(); eIter++) {
					if ( (edge != *eIter) && (hash.find( (*eIter)->Node(node) ) != hash.end()) ) {
						//Remove the other cluster nodes from the frontier list and put it back on the fList
						this->_frontier.remove( (*eIter)->Node(node) );
						this->_frontier.remove( edge->Node(node) );
						//If the other node is not on the vList, add it
						if (!(*eIter)->Node(node)->Flag()) {
							fList.push( (*eIter)->Node(node) );
							(*eIter)->Node(node)->Flag(true);
						}
						//If the referring node is not on the list, add it too
						if (!edge->Node(node)->Flag()) {
							fList.push( edge->Node(node) );
							edge->Node(node)->Flag(true);
						}

					}
				}
			} //Done with adding the extension, process the next possible extension
		} //Done with this node...process the next
	}
	//Set the depth of the cluster
	this->_depth = depth;	
}


/*** Decription of the CategorizeNodesAndExpand algorithm
   * This function evaluates all of the edges from the Internal nodes.  Some will point to other Internal nodes,
   * the others will point to Frontier nodes.  There are two main objectives of this algorithm:
   * 
   *		Weight calculation: The weight of the new core node must be calculated. If an edge points to an 
   *		Internal node its weight needs to be subtracted from the total.  All Internal node's weights are
   *		added to the total.  (Iw - IEw = Cw)
   *
   * from the 
   *
   */
void WCConstraintCluster::CategorizeEdges(void) {
	WPUInt weight=0;
	WCConstraintEdge* edge;
	WCConstraintNode* node;
	std::list<WCConstraintEdge*>::iterator eIter;
	std::list<WCConstraintNode*>::iterator vIter;
	std::map<WCConstraintNode*, WCConstraintEdge*>::iterator hIter;
	std::list<WCConstraintEdge*> edgeList;
	std::map<WCConstraintNode*, WCConstraintEdge*> hash;
	
	//Add each of the frontier nodes into the hash and set their "new" edge to NULL
	for (vIter = this->_frontier.begin(); vIter != this->_frontier.end(); vIter++) {
		hash[*vIter] = NULL;
	}
	
	//Loop through each of the internal nodes to check their edges (completely internal, or internal to frontier(boundry))
	for (vIter = this->_internal.begin(); vIter != this->_internal.end(); vIter++) {
		node = *vIter;
		//Also, update the weight metric for later use in the core node
		weight += node->Weight();
		edgeList = node->EdgeList();
		//Now check each edge to see if it is internal or boundry
		for (eIter = edgeList.begin(); eIter != edgeList.end(); eIter++) {
			edge = *eIter;
			//Check to make sure the edge has not already been processed by another node
			if (!edge->Flag()) {
				hIter = hash.find( edge->Node(node) );
				//If the "other" node is in the list (i.e. a boundry edge) 
				if (hIter != hash.end()) {
					//Check to see if the new edge already exists
					if (hIter->second == NULL) {
						//If it doesn't, create a new edge
						hIter->second = new WCConstraintEdge(NULL, edge->Node(node), this->_core,
															 WCConstraintEdgeType::AbsoluteDistance(),
															 edge->Weight(), WCVector4(0.0));
						//Make sure to put the new edge into the new edge list
						this->_newEdges.push_back(hIter->second);
					}
					//Otherwise just add some weight to it
					else {
						//Increment the new edge's weight by the currently being evaluated edge's
						hIter->second->Weight( hIter->second->Weight() + edge->Weight() );
					}
					//This is a boundry edge so add it to the boundry edge list
					this->_boundryEdges.push_back(edge);
				}
				//Otherwise this is an internal edge
				else {
					//Adjust the core node's weight value by subtracting the weight of the removed internal edge
					weight -= edge->Weight();
					//Add the current edge to the internal list
					this->_internalEdges.push_back(edge);
				}
				//And make sure to set the flag on the edge so that it is not processed again
				edge->Flag(true);
			}
		}
	}
	//Finally, set the weight of the core node
	this->_core->Weight(weight);
}


/***********************************************~***************************************************/


WCConstraintCluster::WCConstraintCluster(std::list<WCConstraintNode*>* vertices, const WPUInt &dConst) : _original(vertices), _depth(0) {
	//Now try to expand the cluster and get the frontier/internal categorization
	this->CategorizeNodesAndExpand();
	//Check to make sure that there is at least one internal node in the cluster
	if (this->_internal.size() == 0) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintCluster::WCConstraintCluster - No internal nodes found.");
		//If no internal nodes, then we do nothing for the core node
		this->_core = NULL;
	}
	//Check to see if the only internal node is a cluster node (ignore these cases...for now)
	else if ((this->_internal.size() == 1) && (this->_internal.front()->Cluster() != NULL)) {
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCConstraintCluster::WCConstraintCluster - One internal node, and it is a cluster node");
		this->_core = NULL;
	}
	else {
		//Create the new core node, don't worry about the weight for now (it is calculated in CategorizeEdges)
		this->_core = new WCConstraintNode(this, 0);
		//Categorize all of the internal edges for creation of new edges from new Core node to Frontier nodes
		this->CategorizeEdges();
		//Now check to make sure that there are some frontier vertices (if not, we have collapsed the entire graph)
		if (this->_frontier.size() == 0) {
			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCConstraintCluster::WCConstraintCluster - Entire graph collapsed");
		}
	}
}


WCConstraintCluster::~WCConstraintCluster() {
	//Make sure to delete the original list of nodes passed in during creation
	if (this->_original != NULL) delete this->_original;
}
	

bool WCConstraintCluster::DepthSort(const WCConstraintCluster* clusterA, const WCConstraintCluster* clusterB) {
	return (clusterA->_depth < clusterB->_depth);
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCConstraintCluster &cluster) {
	std::list<WCConstraintNode*>::const_iterator nIter;
	std::list<WCConstraintCluster*>::const_iterator cIter;
	std::list<WCConstraintEdge*>::const_iterator eIter;

	//Print some basic information about the cluster
	out << "Cluster(" << &cluster << ", Depth:" << cluster._depth << ")\n";

	//Print the original node list
	out << "\tOriginal Nodes: ";
	for (nIter = cluster._original->begin(); nIter != cluster._original->end(); nIter++) {
		out << *nIter << " ";		
	}
	out << std::endl;
	
	//Print the frontier node list
	out << "\tFrontier Nodes: ";
	for (nIter = cluster._frontier.begin(); nIter != cluster._frontier.end(); nIter++) {
		out << *nIter << " ";		
	}
	out << std::endl;
	
	//Print the internal node list
	out << "\tInternal Nodes: ";
	for (nIter = cluster._internal.begin(); nIter != cluster._internal.end(); nIter++) {
		out << *nIter << " ";		
	}
	out << std::endl;
	
	//Print the cluster list
	out << "\tClusters: ";
	for (cIter = cluster._clusters.begin(); cIter != cluster._clusters.end(); cIter++) {
		out << *cIter << " ";		
	}
	out << std::endl;

	//Print the information about the new core node (if any)
	out << "\tNew Core Node:\n";
	if (cluster._core != NULL)
		out << cluster._core;

	//Print the boundry edge list
	out << "\tBoundry Edges Being Removed: ";
	for (eIter = cluster._boundryEdges.begin(); eIter != cluster._boundryEdges.end(); eIter++) {
		out << *eIter << " ";		
	}
	out << std::endl;

	//Print the internal edge list
	out << "\tInternal Edges Being Removed: ";
	for (eIter = cluster._internalEdges.begin(); eIter != cluster._internalEdges.end(); eIter++) {
		out << *eIter << " ";		
	}
	out << std::endl;

	//Print the new edge list
	out << "\tEdges Being Added:\n";
	for (eIter = cluster._newEdges.begin(); eIter != cluster._newEdges.end(); eIter++) {
		out << *eIter;
	}
	//Return
	return out;
}


/***********************************************~***************************************************/

