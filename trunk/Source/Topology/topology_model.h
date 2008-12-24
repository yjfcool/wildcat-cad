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


#ifndef __TOPOLOGY_MODEL_H__
#define __TOPOLOGY_MODEL_H__


/*** Included Header Files ***/
#include <Topology/wtpkl.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
struct WSTopologyShell;


/***********************************************~***************************************************/


class WCTopologyModel : public WCSerializeableObject {
private:
	std::list<WSTopologyShell*>					_shellList;											//!< List of child Shells in this model
private:
	//Hidden Constructors
	WCTopologyModel& operator=(const WCTopologyModel&);												//!< Deny access to equals operator
public:
	WCTopologyModel() : ::WCSerializeableObject(), _shellList() { }									//!< Default constructor
	WCTopologyModel(const WCTopologyModel& model);													//!< Copy constructor
	WCTopologyModel(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	~WCTopologyModel();																				//!< Default destructor

	//General Access Methods
	void AddShell(WSTopologyShell* shell);															//!< Add a shell to the model
	inline std::list<WSTopologyShell*> ShellList(void) { return this->_shellList; }					//!< Get the list of shells
	
	//Boolean Methods
	WCTopologyModel* Slice(const WCMatrix4 &plane, const bool &retainBottom);						//!< Slice the model using the plane
	WCTopologyModel* Intersect(WCTopologyModel *model);												//!< Intersect two topology models
	WCTopologyModel* Union(WCTopologyModel *model);													//!< Union two topology models
	WCTopologyModel* Subtract(WCTopologyModel *model);												//!< Subtract one model from another

	//Required Virtual Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCTopologyModel &model);				//!< Overloaded output operator
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__TOPOLOGY_MODEL_H__

