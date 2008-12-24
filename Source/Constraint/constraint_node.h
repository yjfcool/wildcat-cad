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


#ifndef __CONSTRAINT_NODE_H__
#define __CONSTRAINT_NODE_H__


/*** Included Header Files ***/
#include <Constraint/wgcsl.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefinitions ***/
class WCConstraintSolver;
class WCSketchFeature;
class WCConstraintEdge;
class WCConstraintCluster;
class WCBipartiteFlowNetwork;


/***********************************************~***************************************************/


//Locall Defined Values
#define CONSTRAINTNODETYPE_POINT				0
#define CONSTRAINTNODETYPE_LINE					1
#define CONSTRAINTNODETYPE_ARC					2
#define CONSTRAINTNODETYPE_CIRCLE				3

class WCConstraintNodeType {
private:
	WPUInt										_type;												//!< Type value
	//Deny Access
	WCConstraintNodeType();																			//!< Deny access to default constructor
	WCConstraintNodeType(const WPUInt &type) : _type(type) { }										//!< Primary hidden constructor
public:
	//Constructors and Destructors
	WCConstraintNodeType(const WCConstraintNodeType &type) : _type(type._type) { }					//!< Copy constructor
	~WCConstraintNodeType()						{ }													//!< Default destructor
	//Static Constructors
	static WCConstraintNodeType Point(void)		{ return WCConstraintNodeType(CONSTRAINTNODETYPE_POINT); }	//!< Static point constructor
	static WCConstraintNodeType Line(void)		{ return WCConstraintNodeType(CONSTRAINTNODETYPE_LINE); }	//!< Static line constructor
	static WCConstraintNodeType Arc(void)		{ return WCConstraintNodeType(CONSTRAINTNODETYPE_ARC); }	//!< Static arc constructor
	static WCConstraintNodeType Circle(void)	{ return WCConstraintNodeType(CONSTRAINTNODETYPE_CIRCLE); } //!< Static circle constructor
	//Overridde Operators
	bool operator==(const WCConstraintNodeType &type) { return this->_type == type._type; }			//!< Equality operator
	bool operator!=(const WCConstraintNodeType &type) { return this->_type != type._type; }			//!< Inequality operator
	WCConstraintNodeType& operator=(const WCConstraintNodeType &type) { this->_type = type._type; return *this; } //!< Equals operator
};


/***********************************************~***************************************************/


class WCConstraintNode {
protected:
	WCConstraintNodeType						_type;												//!< Type of constraint
	WCVector4									_data;												//!< Data values
	std::list<WCConstraintEdge*>				_edgeList;											//!< Edge list
	WPUInt										_weight;											//!< Node's weight
	WPInt										_mark;												//!< Node's mark counter
	WCSketchFeature*							_feature;											//!< Feature (only if not a cluster)
	WCConstraintCluster*						_cluster;											//!< Cluster reference (only if a cluster)
	bool										_flag;												//!< General use flag
private:
	//Deny Access
	WCConstraintNode();																				//!< Deny access to default constructor
	WCConstraintNode(const WCConstraintNode&);														//!< Deny access to copy constructor
	WCConstraintNode& operator=(const WCConstraintNode&);											//!< Deny access to equals operator
	//Friend Declaration
	friend class WCConstraintSolver;																//!< Declare constraint solver as a friend
public:
	//Constructors and Destructors
	WCConstraintNode(WCSketchFeature *feature, const WCConstraintNodeType &type,					//!< Non-cluster constructor (4 data)
												const WPUInt &weight, const WCVector4 &data);
	WCConstraintNode(WCConstraintCluster *cluster, const WPUInt &weight);							//!< Cluster constructor
	~WCConstraintNode()							{ }													//!< Default destructor

	//Member Access Methods
	inline WCVector4 Data(void)					{ return this->_data; }								//!< Get the data element
	inline void Data(const WCVector4 &data)		{ this->_data = data; }								//!< Set the data element
	inline WPUInt Weight(void) const			{ return this->_weight; }							//!< Get node's weight
	inline void Weight(const WPUInt &weight)	{ this->_weight = weight;}							//!< Set node's weight
	inline void IncrementMark(void)				{ this->_mark++; }									//!< Increment the node's mark value
	inline WPUInt Mark(void) const				{ return this->_mark; }								//!< Get node's mark value
	inline WCSketchFeature* Feature(void)		{ return this->_feature; }							//!< Get node's tag reference value
	inline WCConstraintCluster* Cluster(void)	{ return this->_cluster; }							//!< Get node's cluster reference value
	inline bool Flag(void)						{ return this->_flag;}								//!< Get node's flag value
	inline void Flag(const bool &flag)			{ this->_flag = flag;}								//!< Set node's flag value

	//Processing Methods
	void MarkNeighbors(void);																		//!< Mark all of the node's neighbors
	void ResetMark(void);																			//!< Reset the mark value of the node
	static bool MarkSort(WCConstraintNode* nodeA, WCConstraintNode* nodeB);							//!< Sorting function using the mark values
	inline void AddEdge(WCConstraintEdge *edge)	{ this->_edgeList.push_back(edge); }				//!< Add edge to list
	inline void RemoveEdge(WCConstraintEdge *edge) { this->_edgeList.remove(edge); }				//!< Remove edge from list
	std::list<WCConstraintEdge*>& EdgeList(void)	{return this->_edgeList; }						//!< Get the node's edge list
	int DistributeEdges(WCBipartiteFlowNetwork *network, const WPUInt &kConst);						//!< Attempt to distribute the edge's nodes

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCConstraintNode &node);				//!< Overloaded output operator
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__CONSTRAINT_NODE_H__

