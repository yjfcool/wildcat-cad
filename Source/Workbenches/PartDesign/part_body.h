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


#ifndef __PART_BODY_H__
#define __PART_BODY_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "PartDesign/part_feature.h"
#include "PartDesign/part_body_actions.h"
#include "PartDesign/part_body_controller.h"


/*** Local Defines ***/
#define BODY_CLASSNAME							"Body"
#define BODY_DEFAULT_VISIBILITY					false


/*** Class Predefines ***/
class WCPart;


/***********************************************~***************************************************/


class WCPartBody : public WCPartFeature {
private:
	void Initialize(void);																			//!< Initialization method
	//Hidden Constructors
	WCPartBody();																					//!< Deny access to default constructor
	WCPartBody(const WCPartBody& WCPartBody);														//!< Deny access to copy constructor
	WCPartBody& operator=(const WCPartBody &body);													//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCPartBody(WCPart *part, const std::string &name);												//!< Primary constructor
	WCPartBody(xercesc::DOMElement *element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	virtual ~WCPartBody();																			//!< Default destructor
	
	//Required Inherited Methods
	virtual std::string RootName(void) const	{ return BODY_CLASSNAME; }							//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from point or curve
	bool Regenerate(void)						{ return true; }									//!< Validate and rebuild
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Actions ***/
	static WCActionPartBodyCreate* ActionCreate(WCPart *creator, const std::string &bodyName);		//!< Primary creation action
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCPartBody &body);						//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__PART_BODY_H__

