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
#include "Constraint/bipartite_flow_network.h"


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCBipartiteFlowEdge &edge) {
	//Print some info
	out << "BipartiteFlowEdge(" << &edge << ", Node:" << edge._node << ") - ";
	out << "Flow:" << edge._flow << std::endl;
	return out;
}


std::ostream& operator<<(std::ostream& out, const WCBipartiteFlowNode &node) {
	//Print some info
	out << "WCBipartiteFlowNode(" << &node << ") - ";
	out << "Weight:" << node._weight << ", Label: " << node._label << std::endl;
	return out;
}


/***********************************************~***************************************************/


WCBipartiteEdgeNode::WCBipartiteEdgeNode(WCConstraintEdge* ref, const WPInt &weight) :
	WCBipartiteFlowNode::WCBipartiteFlowNode(weight), _ref(ref), _edgeA(NULL), _edgeB(NULL) {
	//Do nothing else for now
}


void WCBipartiteEdgeNode::Label(const bool &label, std::queue<WCBipartiteEdgeNode*> &queue) {
	//If the label is being set to true, and the queue exists, put edge node onto the queue
	if (label) queue.push(this);
	//Now set the label value 
	this->_label = label;
}


bool WCBipartiteEdgeNode::AddEdge(WCBipartiteFlowEdge* edge) {
	//Add the edge to the node's edge list
	if (this->_edgeA == NULL)
		this->_edgeA = edge;
	else if (this->_edgeB == NULL)
		this->_edgeB = edge;
	else {
		//Change this to throwing an exception
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCBipartiteEdgeNode::AddEdge too many edges added");
//		throw WCException("WCBipartiteEdgeNode::AddEdge too many edges added", __LINE__, __FILE__);
		return false;
	}
	//Otherwise, all is good
	return true;
}


void WCBipartiteEdgeNode::LabelVertexNodes(std::queue<WCBipartiteVertexNode*> &queue) {
	WCBipartiteVertexNode* node;
	
	//Check to see if edge A needs to be labeled
	if (this->_edgeA != NULL) {
		node = this->_edgeA->Node();
		if (!node->Label()) {
			node->Label(true, queue);
			node->Predecessor(this);
			node->PathCapacity(this->_pathCapacity);
		}
	}
	
	//Now check edge B...
	if (this->_edgeB != NULL) {
		node = this->_edgeB->Node();
		if (!node->Label()) {
			node->Label(true, queue);
			node->Predecessor(this);
			node->PathCapacity(this->_pathCapacity);
		}
	}
}


WPInt WCBipartiteEdgeNode::Flows(void) {
	WPInt flows=0;
	
	//Get the amount of flow (if any) from the A edge
	if (this->_edgeA != NULL)
		flows = this->_edgeA->Flow();
	//Get the amount of flow (if any) from the B edge
	if (this->_edgeB != NULL)
		flows += this->_edgeB->Flow();

	return flows;
}


WPInt WCBipartiteEdgeNode::FlowToVertex(WCBipartiteVertexNode* vertex) {
	//Get the amount of flows (if any) going to the vertex node through edge A
	if ( (this->_edgeA != NULL) && (vertex == this->_edgeA->Node()) ) {
			return this->_edgeA->Flow();
	}
	//Get the amount of flows (if any) going to the vertex node through edge B
	if ( (this->_edgeB != NULL) && (vertex == this->_edgeB->Node()) ) {
			return this->_edgeB->Flow();	
	}
	return 0;
}


void WCBipartiteEdgeNode::AdjustFlows(WCBipartiteVertexNode* vertex, const WPUInt &adjustment) {
	WPInt delta;
	
	//Is the requesting vertex on edge A...
	if ((this->_edgeA != NULL) && (vertex == this->_edgeA->Node()) ) {
		//Check to see if we need to decrease the flow to edge B
		delta = this->_weight - this->Flows() - adjustment;
		if (delta < 0) {
			CLOGGER_INFO(WCLogManager::RootLogger(), "WCBipartiteEdgeNode::AdjustFlows - Reduction required (EdgeNode:" << this << ", VertexNode:" << this->_edgeB->Node() << ", Weight:" << delta << ")");
			//Need to remove delta flow from edge B
			this->_edgeB->IncrementFlow(delta);
			//Also make sure to tell the vertex node on edge B that you have adjusted it
			this->_edgeB->Node()->IncrementFlow(delta);
		}
		//Finally flow on edge A is increasing
		this->_edgeA->IncrementFlow(adjustment);
		vertex->IncrementFlow(adjustment);
	}
	//Is the requesting vertex on edge B...
	if ((this->_edgeB != NULL) && (vertex == this->_edgeB->Node()) ) {
		//Check to see if we need to decrease the flow to edge A
		delta = this->_weight - this->Flows() - adjustment;
		if (delta < 0) {
			CLOGGER_INFO(WCLogManager::RootLogger(), "WCBipartiteEdgeNode::AdjustFlows - Reduction required (EdgeNode:" << this << ", VertexNode:" << this->_edgeA->Node() << ", Weight:" << delta << ")");
			//Need to remove delta flow from edge A
			this->_edgeA->IncrementFlow(delta);
			//Also make sure to tell the vertex node on edge A that you have adjusted it
			this->_edgeA->Node()->IncrementFlow(delta);
		}
		//The flow on edge B is increasing
		this->_edgeB->IncrementFlow(adjustment);
		vertex->IncrementFlow(adjustment);
	}
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCBipartiteEdgeNode::AdjustFlows - Augmenting path found (EdgeNode:" << this << ", VertexNode:" << vertex << ", Weight:" << adjustment << ")");
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCBipartiteEdgeNode &edge) {
	//Show some info about the node
	out << "\t\tBipartiteEdgeNode(" << &edge << ")::(W:" << edge._weight << ", Ref:" << edge._ref << ")\n";
	
	//Display a list of the edges
	if (edge._edgeA != NULL)
		out << "\t\t\t" << *(edge._edgeA);
	if (edge._edgeB != NULL)
		out << "\t\t\t" << (*edge._edgeB);
	//Return
	return out;
}


/***********************************************~***************************************************/


WCBipartiteVertexNode::WCBipartiteVertexNode(WCConstraintNode* ref, const WPUInt &weight) : 
	WCBipartiteFlowNode::WCBipartiteFlowNode(weight), _ref(ref), _predecessor(NULL), _flows(0), _flowSnapshot(0) {
	//Nothing else to do
}
	
	
void WCBipartiteVertexNode::Label(const bool &label, std::queue<WCBipartiteVertexNode*> &queue) {
	//If the label is being set to true, and the queue exists
	if (label) queue.push(this);
	//Now set the label value 
	this->_label = label;
}
	
	
void WCBipartiteVertexNode::LabelEdgeNodes(std::queue<WCBipartiteEdgeNode*> &queue) {
	int i;
		
	//Check each inbound edges (with some amount of flow) that are labeled but not scanned
	for (i=0; i < (int)this->_inList.size(); i++) {
		if ( !this->_inList.at(i)->Label() && (this->_inList.at(i)->FlowToVertex(this) > 0) ) {
			this->_inList.at(i)->Label(true, queue);
		}
	}
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCBipartiteVertexNode &node) {	
	//Show some info about the node
	out << "\t\tBipartiteVertexNode(" << &node << ")::(W:" << node._weight << ", Ref:" << node._ref << ")\n";
	
	//Display a list of the edges
	out << "\t\t\t";
	for (int i=0; i < (int)node._inList.size(); i++) {
		out << "I:" << node._inList.at(i) << " ";
	}
	out << std::endl;
	//Return
	return out;
}


/***********************************************~***************************************************/


void WCBipartiteFlowNetwork::Reset(void) {
	std::list<WCBipartiteEdgeNode*>::iterator eIter;
	std::list<WCBipartiteVertexNode*>::iterator vIter;

	//Clear the two labeled but not processed queues
	while (!this->_vQueue.empty())
		this->_vQueue.pop();
	while (!this->_eQueue.empty())
		this->_eQueue.pop();
	
	//Go through each node and reset the label
	for (eIter = this->_eGroup.begin(); eIter != this->_eGroup.end(); eIter++) {
		(*eIter)->Label(false, this->_eQueue);
		(*eIter)->PathCapacity( (*eIter)->Weight() );
	}
	
	//Reset the vertices
	for (vIter = this->_vGroup.begin(); vIter != this->_vGroup.end(); vIter++) {
		(*vIter)->Label(false, this->_vQueue);
		(*vIter)->PathCapacity(0);
		(*vIter)->Predecessor(NULL);
	}
}


/***********************************************~***************************************************/


WCBipartiteFlowNetwork::~WCBipartiteFlowNetwork() {
	std::list<WCBipartiteEdgeNode*>::iterator eIter;
	std::list<WCBipartiteVertexNode*>::iterator vIter;
	std::list<WCBipartiteFlowEdge*>::iterator edgeIter;

	//Delete all of the edge nodes
	for (eIter = this->_eGroup.begin(); eIter != this->_eGroup.end(); eIter++) {
		delete (*eIter);
	}

	//Delete all of the vertex nodes
	for (vIter = this->_vGroup.begin(); vIter != this->_vGroup.end(); vIter++) {
		delete (*vIter);
	}
	
	//Delete all of the edges
	for (edgeIter = this->_edgeList.begin(); edgeIter != this->_edgeList.end(); edgeIter++) {
		delete (*edgeIter);
	}
}


WCBipartiteEdgeNode* WCBipartiteFlowNetwork::AddEdgeNode(WCConstraintEdge* ref, const WPUInt &weight) {
	//Create the new node
	WCBipartiteEdgeNode* node = new WCBipartiteEdgeNode(ref, weight);
	//Now add the node into the edge group
	this->_eGroup.push_back(node);
	//Return the new node
	return node;
}


WCBipartiteVertexNode* WCBipartiteFlowNetwork::AddVertexNode(WCConstraintNode* ref, const WPUInt &weight) {
	//Create the new node
	WCBipartiteVertexNode* node = new WCBipartiteVertexNode(ref, weight);
	//Now add the node into the vertex group	
	this->_vGroup.push_back(node);
	//Return the new node
	return node;
}


bool WCBipartiteFlowNetwork::AddEdge(WCBipartiteEdgeNode* fromNode, WCBipartiteVertexNode* toNode) {
	//If neither of the edges is NULL
	if ((fromNode != NULL) && (toNode != NULL)) {
		//Create a new edge
		WCBipartiteFlowEdge* edge = new WCBipartiteFlowEdge(toNode);
		//Add it to the edge node
		fromNode->AddEdge(edge);
		toNode->AddIncomingEdge(fromNode);
		//Add the edge to the edge list
		this->_edgeList.push_back(edge);
		return true;
	}
	//Otherwise there is an error.  Log it and throw an exception
	else {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCBipartiteFlowNetwork::AddEdge invalid nodes passed");
//		throw CException("CBipartiteFlowNetwork::AddEdge invalid nodes passed", __LINE__, __FILE__);
		return false;
	}
	//Otherwise things are not good
	return false;
}


WCBipartiteVertexNode* WCBipartiteFlowNetwork::VertexNodeByReference(const void* ref) {
	std::list<WCBipartiteVertexNode*>::iterator vIter;
	
	//Look through all of the vertex nodes for one with a given reference
	for (vIter =this->_vGroup.begin(); vIter != this->_vGroup.end(); vIter++) {
		if ( ref == (*vIter)->Reference() ) return (*vIter);
	}
	return NULL;
}


WPUInt WCBipartiteFlowNetwork::LabeledDensity(void) {
	WPUInt vSum=0, eSum;
	std::list<WCBipartiteEdgeNode*>::iterator eIter;
	std::list<WCBipartiteVertexNode*>::iterator vIter;
	
	//Add up the weight for all of the labeled vertex nodes
	for (vIter =this->_vGroup.begin(); vIter != this->_vGroup.end(); vIter++) {
		if ( (*vIter)->Label() ) 
			vSum += (*vIter)->Weight();
	}
	//Add up the weight for all of the labeled edge nodes
	for (eIter =this->_eGroup.begin(); eIter != this->_eGroup.end(); eIter++) {
		if ( (*eIter)->Label() ) 
			eSum += (*eIter)->Weight();
	}
	//Return the difference the labeled edge weights minus the labled vertex weights
	return eSum - vSum;
}


WPUInt WCBipartiteFlowNetwork::Distribute(WCBipartiteEdgeNode *edge) { //WCConstraintEdge* ref
	std::list<WCBipartiteEdgeNode*>::iterator eIter;
	std::list<WCBipartiteVertexNode*>::iterator vIter;
	WCBipartiteVertexNode *vert=NULL, *v=NULL;
	WCBipartiteEdgeNode *e=NULL; //*edge=NULL, 

	//The edge vertex will be labeled shortly
	WPUInt capvert=0, minimum;

	//Get the e vertex from the edge reference
//	for (eIter = this->_eGroup.begin(); eIter != this->_eGroup.end(); eIter++) {
//		if (ref == (*eIter)->Reference())
//			edge = (*eIter);
//	}

	//Reset variables needed for the algorithm
	this->Reset();
	edge->Label(1, this->_eQueue);

//	try {
		while (edge->PathCapacity() > 0) {
			//If there is capacity left to distribute, but no nodes not scanned, we are done...
			if ( (this->_eQueue.empty()) && (this->_vQueue.empty()) ) {
				//Return the amount of flow that could not be distributed
				return edge->PathCapacity();
			}
		
			//Look for all labeled edge nodes that have not been processed (i.e. are on the eQueue)
			while (!this->_eQueue.empty()) {
				//Get the next edge node to be processed
				e = this->_eQueue.front();
				//Label all of the edge's neighbors (that have not already been labeled)
				e->LabelVertexNodes( this->_vQueue );
				//Remove this edge node from the queue
				this->_eQueue.pop();
			}

			//Look for all labeled vertex nodes that have not been scanned
			while (!this->_vQueue.empty()) {
				//Get the next vertex node to be processed
				v = this->_vQueue.front();
				minimum = std::min( v->Weight() - v->Flows(), v->PathCapacity() );
				if (minimum > capvert) {
					vert = v;
					capvert = minimum;
				} else {
					//Label all edges with flow > 0 into this vertex node
					v->LabelEdgeNodes( this->_eQueue );
				}
				//Remove the vertex node from the queue
				this->_vQueue.pop();
			}
		
			//If there is a vertex...
			if (vert != NULL) {
				//An augmenting path has been found
				vert->Predecessor()->AdjustFlows(vert, capvert);
				//Reset the label and scan variables
				this->Reset();
				vert = NULL;
				capvert = 0;
				edge->PathCapacity(edge->Weight() - edge->Flows());
				//Label the main edge and put it onto the queue
				edge->Label(true, this->_eQueue );
			}
		} //End of while loop
//	} //End of try
//	catch(WCException &e) {
//		CLOGGER_ERROR(CLogger::RootLogger(), "CBipartiteFlowNetwork::Distribute exception caught");
//		e.Print();
//		return -1;
//	}
	//Edge weight is completely distributable, return 0
	return 0;
}


std::list<WCConstraintNode*>* WCBipartiteFlowNetwork::LabeledVertices(void) {
	std::list<WCConstraintNode*>* list = new std::list<WCConstraintNode*>();
	std::list<WCBipartiteVertexNode*>::iterator vIter;
	
	//Look at each of the vertex nodes
	for (vIter = this->_vGroup.begin(); vIter != this->_vGroup.end(); vIter++) {
		//Check to see if the vertex node is labeled
		if ( (*vIter)->Label() )
			list->push_back( (WCConstraintNode*)((*vIter)->Reference()) );
	}
	return list;
}


std::list<WCConstraintEdge*>* WCBipartiteFlowNetwork::LabeledEdges(void) {
	std::list<WCConstraintEdge*>* list = new std::list<WCConstraintEdge*>();
	std::list<WCBipartiteEdgeNode*>::iterator eIter;

	//Look at each of the edge nodes
	for (eIter = this->_eGroup.begin(); eIter != this->_eGroup.end(); eIter++) {
		//Check to see if the edge node is labeled
		if ( (*eIter)->Label() )
			list->push_back( (WCConstraintEdge*)((*eIter)->Reference()) );
	}	
	return list;
}


void WCBipartiteFlowNetwork::Snapshot(void) {
	std::list<WCBipartiteVertexNode*>::iterator vIter;
	std::list<WCBipartiteFlowEdge*>::iterator eIter;
	
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCBipartiteFlowNetwork::Snapshot -- Taking Flow Snapshot");
	//Only need to snapshot vertex nodes and edges because edge nodes derive their flow values
	//Issue the snapshot call to all of the vertex nodes
	for (vIter = this->_vGroup.begin(); vIter != this->_vGroup.end(); vIter++) {
		(*vIter)->Snapshot();
	}
	//Issue the snapshot call to all of the edges
	for (eIter = this->_edgeList.begin(); eIter != this->_edgeList.end(); eIter++) {
		(*eIter)->Snapshot();
	}
}


void WCBipartiteFlowNetwork::Restore(void) {
	std::list<WCBipartiteVertexNode*>::iterator vIter;
	std::list<WCBipartiteFlowEdge*>::iterator eIter;

	CLOGGER_INFO(WCLogManager::RootLogger(), "WCBipartiteFlowNetwork::Restore -- Restoring Flow");
	//Only need to restore vertex nodes and edges because edge nodes derive their flow values
	//Issue the restore call to all of the vertex nodes
	for (vIter = this->_vGroup.begin(); vIter != this->_vGroup.end(); vIter++) {
		(*vIter)->Restore();
	}
	//Issue the snapshot call to all of the edges
	for (eIter = this->_edgeList.begin(); eIter != this->_edgeList.end(); eIter++) {
		(*eIter)->Restore();
	}
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCBipartiteFlowNetwork &network) {
	std::list<WCBipartiteEdgeNode*>::const_iterator eIter;
	std::list<WCBipartiteVertexNode*>::const_iterator vIter;

	//Print some general BFN info...
	out << "BipartiteFlowNetwork(" << &network << ")\n";
	
	//Print some V and E group info
	out << "\tBFN Vertex Group(" << network._vGroup.size() << "):\n";
	for (vIter = network._vGroup.begin(); vIter != network._vGroup.end(); vIter++) {
		out << *(*vIter);
	}
	
	out << "\tBFN Edge Group(" << network._eGroup.size() << "):\n";
	for (eIter = network._eGroup.begin(); eIter != network._eGroup.end(); eIter++) {
		out << *(*eIter);
	}
	//Return
	return out;
}


/***********************************************~***************************************************/

