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
#include "Topology/wtpkl.h"


/*** Locally Defined Values ***/
//None


/*** Class Predefines ***/
class WCDocument;
class WCTopologyRegion;


/***********************************************~***************************************************/


class WCTopologyModel : public WCSerializeableObject {
private:
	WCDocument									*_document;											//!< Parent document object
	std::list<WCTopologyRegion*>				_regionList;										//!< List of child Regions of this model
private:
	WCTopologyModel();																				//!< Deny access to the default constructor
	WCTopologyModel(const WCTopologyModel&);														//!< Deny access to copy constructor
	WCTopologyModel& operator=(const WCTopologyModel&);												//!< Deny access to equals operator
public:
	WCTopologyModel(WCDocument *doc);																//!< Primary constructor
	~WCTopologyModel();																				//!< Default destructor

	//General Access Methods
	std::list<WCTopologyRegion*> Regions(void)	{ return this->_regionList; }						//!< Get the list of Regions
	
	//Boolean Methods
	WCTopologyModel* Intersect(WCTopologyModel *model);												//!< Intersect two topology models
	WCTopologyModel* Union(WCTopologyModel *model);													//!< Union two topology models
	WCTopologyModel* Subtract(WCTopologyModel *model);												//!< Subtract one model from another

	//Required Virtual Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document);									//!< Serialize the object
	static WCTopologyModel* Deserialize(xercesc::DOMElement* obj);									//!< Deserialize the object
	bool Validate(xercesc::DOMElement* obj);														//!< Check to see if obj is valid

	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCTopologyModel &model);				//!< Overloaded output operator
};


/***********************************************~***************************************************/


#endif //__TOPOLOGY_MODEL_H__

