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


#ifndef __PART_ACTIONS_H__
#define __PART_ACTIONS_H__


/*** Included Header Files ***/
#include <Kernel/wftrl.h>
#include <Kernel/action.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCPart;
class WCPartFeature;


/***********************************************~***************************************************/


class WCActionPartCreate : public WCAction {
private:
	WCPart										*_part;												//!< Associated part
	std::string									_partName;											//!< Part name
	std::string									_filename;											//!< Part filename
	//Deny Access
	WCActionPartCreate();																			//!< Deny access to default constructor
	WCActionPartCreate(const WCActionPartCreate &action);											//!< Deny access to copy constructor
	WCActionPartCreate& operator=(const WCActionPartCreate &action);								//!< Deny access to equals operator
	WCActionPartCreate(const std::string &partName, const std::string &filename);					//!< Hidden primary constructor
	//Friend Declarations
	friend class WCPart;																			//!< Make WCPart a friend
public:
	//Constructors and Destructors
	WCActionPartCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary);				//!< Persistance constructor
	~WCActionPartCreate()						{ }													//!< Default destructor
	
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void)							{ return false; }									//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionPartDelete : public WCAction {
private:
	WCPart										*_part;												//!< Associated part
	std::list<WCPartFeature*>					_features;											//!< List of features
	//Hidden Constructors
	WCActionPartDelete();																			//!< Deny access to default constructor
	WCActionPartDelete(const WCActionPartDelete &action);											//!< Deny access to copy constructor
	WCActionPartDelete& operator=(const WCActionPartDelete& action);								//!< Deny access to equals operator
	WCActionPartDelete(WCPart *part, std::list<WCPartFeature*> features);							//!< Primary constructor
	//Friend Declarations
	friend class WCPart;																			//!< Make part a friend
public:
	//Constructors and Destructors
	WCActionPartDelete(xercesc::DOMElement *element, WCSerialDictionary *dictionary);				//!< Persistance constructor
	~WCActionPartDelete()						{ }													//!< Default destructor
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void)							{ return false; }									//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__PART_ACTIONS_H__

