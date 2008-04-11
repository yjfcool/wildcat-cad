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


#ifndef __BIPARTITE_FLOW_NETWORK_H__
#define __BIPARTITE_FLOW_NETWORK_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCBipartiteVertexNode;
class WCConstraintNode;
class WCConstraintEdge;


/***********************************************~***************************************************/


class WCBipartiteFlowEdge {
protected:
	WCBipartiteVertexNode*						_node;												//!< What vertex node this edge points to
	WPInt										_flow, _flowSnapshot;								//!< What the current flow of the edge is

	//Deny Access
	WCBipartiteFlowEdge(const WCBipartiteFlowEdge&);												//!< Deny access to copy constructor
	WCBipartiteFlowEdge& operator=(const WCBipartiteFlowEdge&);										//!< Deny acess to equals constructor
public:
	//Constructors and Destructors
	WCBipartiteFlowEdge(WCBipartiteVertexNode* toNode) : _node(toNode), _flow(0),					//!< Default constructor
												_flowSnapshot(0) { }
	virtual ~WCBipartiteFlowEdge()				{ }													//!< Default destructor

	//Member Access Methods
	virtual inline WPInt Flow(void)				{ return this->_flow; }								//!< Returns the amount of flow through the edge
	virtual inline void Flow(const WPInt &flow){ this->_flow = flow; }								//!< Sets the amount of flow through the edge
	virtual inline void IncrementFlow(const WPInt &inc){ this->_flow += inc; }						//!< Increments (or decrements) the edge's flow value
	virtual inline WCBipartiteVertexNode* Node(void)	{ return this->_node; }						//!< Returns the node that the edge points to
	virtual inline void Node(WCBipartiteVertexNode* node){ this->_node = node; }					//!< Sets the node that the edge points to

	//Snapshot and Restore Methods
	virtual inline void Snapshot(void)			{ this->_flowSnapshot = this->_flow; }				//!< Preserves the current flows
	virtual inline void Restore(void)			{ this->_flow = this->_flowSnapshot; }				//!< Restores the snapshotted flow values

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCBipartiteFlowEdge &edge);			//!< Overloaded output operator
};


/***********************************************~***************************************************/


class WCBipartiteFlowNode {
protected:
	WPInt										_weight, _pathCapacity;								//!< Node weight and flow counts
	bool										_label;												//!< Markers for the Distribute algorithm

	//Deny Access
	WCBipartiteFlowNode(const WCBipartiteFlowNode&);												//!< Deny access to copy constructor
	WCBipartiteFlowNode& operator=(const WCBipartiteFlowNode&);										//!< Deny access equals constructor
public:
	//Constructors and Destructors
	WCBipartiteFlowNode(const WPInt &weight) : _weight(weight),	_label(false), _pathCapacity(0) { }	//!< Default constructor
	virtual ~WCBipartiteFlowNode()				{ }													//!< Default destructor

	//Member Access Methods
	virtual inline WPInt Weight(void)				{ return this->_weight; }						//!< Get the node's weight
	virtual inline void Weight(const WPInt &weight)	{ this->_weight = weight; }						//!< Set the node's weight
	virtual inline bool Label(void)					{ return this->_label; }						//!< Get the node's label value
	virtual inline WPInt PathCapacity(void)			{ return this->_pathCapacity; }					//!< Get the node's path capacity value
	virtual inline void PathCapacity(const WPInt &cap){ this->_pathCapacity = cap; }				//!< Set the node's path capacity value
		
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCBipartiteFlowNode &node);			//!< Overloaded output operator
};


/***********************************************~***************************************************/


class WCBipartiteEdgeNode : public WCBipartiteFlowNode {
protected:
	WCConstraintEdge							*_ref;												//!< Reference to a constaint edge
	WCBipartiteFlowEdge							*_edgeA, *_edgeB;									//!< A and B edge points for the edge node

	//Deny Access
	WCBipartiteEdgeNode(const WCBipartiteEdgeNode&);												//!< Disallow copy constructor
	WCBipartiteEdgeNode& operator=(const WCBipartiteEdgeNode&);										//!< Disallow = operator
public:
	//Constructors and Destructors
	WCBipartiteEdgeNode(WCConstraintEdge *ref, const WPInt &weight);								//!< Default constructor
	~WCBipartiteEdgeNode()						{ }													//!< Default destructor

	//Member Access Methods
	inline WCConstraintEdge* Reference(void)	{ return this->_ref; }								//!< Return the node's reference value
	inline bool Label(void)						{ return this->_label; }							//!< Get the label 
	void Label(const bool &label, std::queue<WCBipartiteEdgeNode*> &queue);							//!< Label the edge node
	bool AddEdge(WCBipartiteFlowEdge* edge);														//!< Returns the number of edges
	void LabelVertexNodes(std::queue<WCBipartiteVertexNode*> &queue);								//!< Labels the vertex nodes and sets info	
	WPInt Flows(void);																				//!< Returns total outgoing flows from edge node
	WPInt FlowToVertex(WCBipartiteVertexNode* vertex);												//!< Get amount of flow going to vertex node
	void AdjustFlows(WCBipartiteVertexNode* vertex, const WPUInt &adjustment);						//!< Adjust the flows through this edge node

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCBipartiteEdgeNode &node);			//!< Overloaded output operator
};


/***********************************************~***************************************************/


class WCBipartiteVertexNode : public WCBipartiteFlowNode {
protected:
	WCConstraintNode							*_ref;												//!< Reference to a constraint node
	WPInt										_flows, _flowSnapshot;								//!< Current amount of flow and the flow snapshot value
	std::vector<WCBipartiteEdgeNode*>			_inList;											//!< List of incoming flow edge nodes
	WCBipartiteEdgeNode*						_predecessor;										//!< The predecessor edge node in the flow

	//Deny Access
	WCBipartiteVertexNode(const WCBipartiteVertexNode&);											//!< Deny access to copy constructor
	WCBipartiteVertexNode& operator=(const WCBipartiteVertexNode&);									//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCBipartiteVertexNode(WCConstraintNode* ref, const WPUInt &weight);								//!< Default constructor
	~WCBipartiteVertexNode()					{ }													//!< Default destructor

	//Member Access Methods
	inline WCConstraintNode* Reference(void)	{ return this->_ref; }								//!< Return the node's reference value
	inline bool Label(void)						{ return this->_label; }							//!< Get the label 
	void Label(const bool &label, std::queue<WCBipartiteVertexNode*> &queue);						//!< Label the edge node
	inline WCBipartiteEdgeNode* Predecessor(void)		{ return this->_predecessor; }				//!< Get the predecessor of the vertex node (is an edge node)
	inline void Predecessor(WCBipartiteEdgeNode* ref)	{ this->_predecessor = ref; }				//!< Set the predecessor of the vertex node
	inline WPInt Flows(void)					{ return this->_flows; }							//!< Get the amount of flow into the vertex node
	inline void Flows(const WPInt &flows)		{ this->_flows = flows; }							//!< Set the amount of flow into the vertex node
	inline void IncrementFlow(const WPInt &inc){ this->_flows += inc; }								//!< Make an adjustment to the flow into the vertex node
	
	inline void Snapshot(void)					{ this->_flowSnapshot = this->_flows; }				//!< Preserves the current flows into the vertex node
	inline void Restore(void)					{ this->_flows = this->_flowSnapshot; }				//!< Restores the snapshotted flow values
	void AddIncomingEdge(WCBipartiteEdgeNode* node) { this->_inList.push_back(node); }				//!< Add a new incoming edge to the vertex node
	void LabelEdgeNodes(std::queue<WCBipartiteEdgeNode*> &queue);									//!< Label all adjacent edge nodes with flow to the vertex node
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCBipartiteVertexNode &node);			//!< Overloaded output operator
};


/***********************************************~***************************************************/


class WCBipartiteFlowNetwork {
protected:
	std::list<WCBipartiteEdgeNode*>				_eGroup;											//!< Group of edge nodes
	std::list<WCBipartiteVertexNode*>			_vGroup;											//!< Group of vertex nodes
	std::list<WCBipartiteFlowEdge*>				_edgeList;											//!< Master list of all edges
	std::queue<WCBipartiteVertexNode*>			_vQueue;											//!< Queue of labeled but not scanned vertex nodes
	std::queue<WCBipartiteEdgeNode*>			_eQueue;											//!< Queue of labeled but not scanned edge nodes

	//Protected Methods
	void Reset(void);																				//!< Reset all of the node scan and label markers

	//Deny Access
	WCBipartiteFlowNetwork(const WCBipartiteFlowNetwork&);											//!< Disallow the use of copy constructor
	WCBipartiteFlowNetwork& operator=(const WCBipartiteFlowNetwork&);								//!< Disallow the use of the = operator
public:
	//Constructors and Destructors
	WCBipartiteFlowNetwork() : _eGroup(), _vGroup(), _edgeList(), _vQueue(), _eQueue() { }			//!< Default constructor
	~WCBipartiteFlowNetwork();																		//!< Default destructor

	//Member Access Methods
	WCBipartiteEdgeNode* AddEdgeNode(WCConstraintEdge *edge, const WPUInt &weight);					//!< Add an edge node to the network
	WCBipartiteVertexNode* AddVertexNode(WCConstraintNode *node, const WPUInt &weight);				//!< Add a vertex node to the network
	bool AddEdge(WCBipartiteEdgeNode* fromNode, WCBipartiteVertexNode* toNode);						//!< Add an edge between two nodes
	
	WCBipartiteVertexNode* VertexNodeByReference(const void* ref);									//!< Return a vertex node using ref value as lookup
	WPUInt LabeledDensity(void);																	//!< Return the density of the labeled vertex and edge nodes
	WPUInt Distribute(WCBipartiteEdgeNode *edge);													//!< Distribute an edge, return undistributable flow amount
	std::list<WCConstraintNode*>* LabeledVertices(void);											//!< Return the list of labeled (and scanned) vertex nodes
	std::list<WCConstraintEdge*>* LabeledEdges(void);												//!< Return the list of labeled (and scanned) edge nodes
	void Snapshot(void);																			//!< Preserves the current flows
	void Restore(void);																				//!< Restores the snapshotted flow values
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCBipartiteFlowNetwork &network);			//!< Overloaded output operator
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__BIPARTITE_FLOW_NETWORK_H__

