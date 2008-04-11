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


#ifndef __CONSTRAINT_EDGE_H__
#define __CONSTRAINT_EDGE_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefinitions ***/
class WCConstraintSolver;
class WCSketchConstraint;
class WCConstraintNode;


/***********************************************~***************************************************/


//Locall Defined Values
#define CONSTRAINTEDGETYPE_ADIST				0
#define CONSTRAINTEDGETYPE_VDIST				1
#define CONSTRAINTEDGETYPE_HDIST				2
#define CONSTRAINTEDGETYPE_ANGLE				3

class WCConstraintEdgeType {
private:
	WPUInt										_type;												//!< Type value
	//Deny Access
	WCConstraintEdgeType();																			//!< Deny access to default constructor
	WCConstraintEdgeType(const WPUInt &type) : _type(type) { }										//!< Primary hidden constructor
public:
	//Constructors and Destructors
	WCConstraintEdgeType(const WCConstraintEdgeType &type) : _type(type._type) { }					//!< Copy constructor
	~WCConstraintEdgeType()						{ }													//!< Default destructor
	//Static Constructors
	static WCConstraintEdgeType AbsoluteDistance(void) { return WCConstraintEdgeType(CONSTRAINTEDGETYPE_ADIST); } //!< Static a-dist constructor
	static WCConstraintEdgeType VerticalDistance(void) { return WCConstraintEdgeType(CONSTRAINTEDGETYPE_VDIST); } //!< Static v-dist constructor
	static WCConstraintEdgeType HorizontalDistance(void) { return WCConstraintEdgeType(CONSTRAINTEDGETYPE_HDIST); } //!< Static h-dist constructor
	static WCConstraintEdgeType Angle(void) { return WCConstraintEdgeType(CONSTRAINTEDGETYPE_ANGLE); } //!< Static angle constructor
	//Overridde Operators
	bool operator==(const WCConstraintEdgeType &type) { return this->_type == type._type; }			//!< Equality operator
	bool operator!=(const WCConstraintEdgeType &type) { return this->_type != type._type; }			//!< Inequality operator
	WCConstraintEdgeType& operator=(const WCConstraintEdgeType &type) { this->_type = type._type; return *this; } //!< Equals operator
};


/***********************************************~***************************************************/


class WCConstraintEdge {
protected:
	WCConstraintEdgeType						_type;												//!< Type of constraint
	WCVector4									_data;												//!< Data for the constraint
	WCConstraintNode							*_nodeA, *_nodeB;									//!< Related nodes
	WCSketchConstraint*							_constraint;										//!< Related sketch constraint
	WPUInt										_weight;											//!< Edge's weight
	bool										_flag;												//!< Utility flag
private:
	//Deny Access
	WCConstraintEdge();																				//!< Deny access to default constructor
	WCConstraintEdge(const WCConstraintEdge&);														//!< Deny access to copy constructor
	WCConstraintEdge& operator=(const WCConstraintEdge&);											//!< Deny access to equals operator
	//Friend Declaration
	friend class WCConstraintSolver;																//!< Declare constraint solver as a friend
public:
	//Constructors and Destructors
	WCConstraintEdge(WCSketchConstraint *constraint, WCConstraintNode *nodeA, WCConstraintNode *nodeB,//!< Default constructor
												const WCConstraintEdgeType &type, const WPUInt &weight, const WCVector4 &data);
	~WCConstraintEdge()							{ }													//!< Default destructor

	//Member Access Methods
	inline WCVector4 Data(void)					{ return this->_data; }								//!< Get the data element
	inline void Data(const WCVector4 &data)		{ this->_data = data; }								//!< Set the data element
	inline WCSketchConstraint* Feature(void)	{ return this->_constraint; }						//!< Get associated constraint
	inline WPUInt Weight(void) const			{ return this->_weight; }							//!< Get the edge's weight
	inline void Weight(const WPUInt &weight)	{ this->_weight = weight; }							//!< Set the edge's weight
	inline bool Flag(void) const				{ return this->_flag;}								//!< Get the edge's flag value
	inline void Flag(const bool &flag)			{ this->_flag = flag;}								//!< Set the edge's flag value

	//Processing Methods
	WCConstraintNode* Node(WCConstraintNode* thisNode);												//!< Get "other" node that the edge points to
	void Extract(void);																				//!< Remove the edge from the node's edge lists
	static bool EdgeSort(const WCConstraintEdge* edgeA, const WCConstraintEdge* edgeB);				//!< Comparison functino for edge list sorting

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCConstraintEdge &edge);			//!< Overloaded output operator
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__CONSTRAINT_EDGE_H__

