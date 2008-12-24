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


#ifndef __CONSTRAINT_PLANNER_H__
#define __CONSTRAINT_PLANNER_H__


/*** Included Header Files ***/
#include <Constraint/wgcsl.h>
#include <Constraint/constraint_node.h>
#include <Constraint/constraint_edge.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefinitions ***/
class WCBipartiteFlowNetwork;
//class WCConstraintNode;
//class WCConstraintNodeType;
//class WCConstraintEdge;
//class WCConstraintEdgeType;
class WCConstraintCluster;
class WCSketchFeature;
class WCSketchConstraint;


/***********************************************~***************************************************/


//Locally Defined Values
#define PLANSTATUS_UNDERCONSTRAINED				0
#define PLANSTATUS_WELLCONSTRAINED				1
#define PLANSTATUS_OVERCONSTRAINED				2
#define PLANSTATUS_ERROR						3

class WCPlanStatus {
private:
	WPUInt										_status;											//!< Hidden status
	WCPlanStatus();																					//!< Deny access to default constructor
	WCPlanStatus(const WPUInt &status) : _status(status) { }										//!< Hidden primary constructor
public:
	//Constructors and Destructors
	WCPlanStatus(const WCPlanStatus &status) : _status(status._status) { }							//!< Copy constructor
	~WCPlanStatus()								{ }													//!< Default destructor
	//Static Constructors
	static WCPlanStatus UnderConstrained(void)	{ return WCPlanStatus(PLANSTATUS_UNDERCONSTRAINED); }	//!< Under-constrained static method
	static WCPlanStatus WellConstrained(void)	{ return WCPlanStatus(PLANSTATUS_WELLCONSTRAINED); }	//!< Well-constrained static method
	static WCPlanStatus OverConstrained(void)	{ return WCPlanStatus(PLANSTATUS_OVERCONSTRAINED); }	//!< Over-constrained static method
	static WCPlanStatus Error(void)				{ return WCPlanStatus(PLANSTATUS_ERROR); }				//!< Error static method
	//Overridden Operators
	WCPlanStatus& operator=(const WCPlanStatus &status) { this->_status = status._status; return *this; }	//!< Equals operator
	bool operator==(const WCPlanStatus &status)	{ return this->_status == status._status; }			//!< Equality operator
	bool operator!=(const WCPlanStatus &status)	{ return this->_status != status._status; }			//!< In-equality operator
};


/***********************************************~***************************************************/


class WCConstraintPlan {
protected:
	WCPlanStatus								_status;											//!< Status of the plan
	std::list<WCConstraintCluster*>				_clusters;											//!< List of clusters in the decomposition plan

public:
	//Constructors and Destructors
	WCConstraintPlan() : _status(WCPlanStatus::WellConstrained()), _clusters() { }					//!< Default constructor
	~WCConstraintPlan();																			//!< Default destructor

	//Member Access Methods
	inline WCPlanStatus Status(void) const		{ return this->_status; }							//!< Get the status of the cluster
	inline void Status(const WCPlanStatus &status) { this->_status = status; }						//!< Set the status of the plan
	void AddCluster(WCConstraintCluster* cluster);													//!< Add a new cluster to the solution plan

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCConstraintPlan &plan);				//!< Overloaded output operator
};


/***********************************************~***************************************************/


class WCConstraintPlanner {
protected:
	std::list<WCConstraintNode*>				_nodeList;											//!< Planner adjacency list
	std::list<WCConstraintEdge*>				_edgeList;											//!< Planner edge list
	WCBipartiteFlowNetwork*						_network;											//!< BipartiteFlowNetwork representation
	bool										_isDirty;											//!< Dirty flag

	void ResetMarks(std::list<WCConstraintNode*> &list);											//!< Reset and sort node list
	WCConstraintCluster* FindCluster(const WPUInt &dConst);											//!< Find next dense subgraph and create cluster
	void ReduceByCluster(WCConstraintCluster *cluster);												//!< Reduce graph with the given cluster

	//Deny Access
	WCConstraintPlanner(const WCConstraintPlanner&);												//!< Deny access to copy constructor
	WCConstraintPlanner& operator=(const WCConstraintPlanner&);										//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCConstraintPlanner() : _nodeList(), _edgeList(), _network(NULL), _isDirty(false) { }			//!< Default constructor
	~WCConstraintPlanner();																			//!< Default destructor

	//Member Access Methods
	inline bool IsDirty(void)					{ return this->_isDirty; }							//!< Get the dirty flag
	inline void MarkDirty(void)					{ this->_isDirty = true; }							//!< Mark the planner as dirty

	//Primary Methods
	WCConstraintNode* AddNode(WCSketchFeature *feature, const WCConstraintNodeType &type,			//!< Add a new node into the constraint graph
												const WPUInt &weight, const WCVector4 &data);
	bool RemoveNode(WCConstraintNode *node);														//!< Remove a node (add all incident edges)
	WCConstraintEdge* AddEdge(WCSketchConstraint* constraint, WCConstraintNode *nodeA,				//!< Add a new constraint
												WCConstraintNode *nodeB, const WCConstraintEdgeType &type,
												const WPUInt &weight, const WCVector4 &data);
	bool RemoveEdge(WCConstraintEdge *edge);														//!< Remove an edge
	WPInt Density(void) const;																		//!< Get current density
	WCConstraintPlan* Solve(void);																	//!< Generate plan for constraint graph
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCConstraintPlanner &planner);			//!< Overloaded output operator
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__CONSTRAINT_PLANNER_H__

