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
#include <Constraint/constraint_edge.h>
#include <Constraint/constraint_node.h>
#include <Constraint/sketch_constraint.h>


/***********************************************~***************************************************/


WCConstraintEdge::WCConstraintEdge(WCSketchConstraint *constraint, WCConstraintNode* nodeA, WCConstraintNode* nodeB,
	const WCConstraintEdgeType &type, const WPUInt &weight, const WCVector4 &data) : _type(type), _data(data),
	_nodeA(nodeA), _nodeB(nodeB), _constraint(constraint), _weight(weight), _flag(false) {
	//Nothing else to do for now
}


WCConstraintNode* WCConstraintEdge::Node(WCConstraintNode* thisNode) {
	//If the passed edge is equal to nodeA then return nodeB
	if (thisNode == this->_nodeA) 
		return this->_nodeB;
	//If the passed edge is equal to nodeB then return nodeA
	else if (thisNode == this->_nodeB)
		return this->_nodeA;
	//Otherwise return nothing
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCConstraintEdge::Node Unknown edge passes as reference");
	return NULL;
}


void WCConstraintEdge::Extract(void) {
	//If there is a node on A, remove this edge from its edge list
	if (this->_nodeA != NULL) this->_nodeA->RemoveEdge(this);
	//If there is a node on B, remove this edge from its edge list
	if (this->_nodeB != NULL) this->_nodeB->RemoveEdge(this);
}


bool WCConstraintEdge::EdgeSort(const WCConstraintEdge* edgeA, const WCConstraintEdge* edgeB) {
	//Return the bool comparison between the weights of the two edges
	return (edgeA->_weight > edgeB->_weight);
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCConstraintEdge &edge) {
	//Print some info
	out << "ConstraitEdge(" << &edge << ")(W:" << edge._weight << ", nA:" << edge._nodeA << ", nB: " << edge._nodeB << ") ";
	if (edge._constraint != NULL)
		out << edge._constraint->GetName();
	out << std::endl;
	//Return
	return out;
}


/***********************************************~***************************************************/

