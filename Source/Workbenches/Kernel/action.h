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


#ifndef __ACTION_H__
#define __ACTION_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCFeature;
class WCDocument;


/***********************************************~***************************************************/


class WCAction : virtual public WCSerializeableObject {
protected:
	std::string									_name;												//!< Action name
	WCFeature									*_creator;											//!< Creating feature
	WCGUID										_guid;												//!< GUID for self
	WCSerialDictionary							*_dictionary;										//!< Serialization dictionary
	bool										_rollback;											//!< Rollback flag
private:
	//Deny Access
	WCAction();																						//!< Deny access to default constructor
public:
	//Constructors and Destructors
	WCAction(std::string actionName, WCFeature *creator);											//!< Primary constructor
	WCAction(xercesc::DOMElement *element, WCSerialDictionary *dictionary);							//!< Persistance constructor
	virtual ~WCAction()							{ }													//!< Default destructor
	
	//Member Access Methods
	std::string Name(void)						{ return this->_name; }								//!< Get the action name
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	//Required Methods
	virtual WCFeature* Execute(void)=0;																//!< Execute the action
	virtual bool Merge(WCAction *action)=0;															//!< Try to merge two actions
	virtual bool Rollback(void)=0;																	//!< Try to roll back the action

	/*** Static Serialization Methods ***/
	static bool Deserialize(xercesc::DOMElement* actionElement, WCSerialDictionary *dictionary);	//!< Master deserialization list

	//Feature Tree Methods
	friend std::ostream& operator<<(std::ostream& out, const WCAction &action);						//!< Overloaded output operator
};


/***********************************************~***************************************************/


class WCMacroAction : public WCAction {
private:
	std::vector<WCAction*>						_actionVector;										//!< Vector of sub-actions
	//Deny Access
	WCMacroAction();																				//!< Deny access to defailt constructor
	WCMacroAction(const WCMacroAction& ma);															//!< Deny access to copy constructor
	WCMacroAction& operator=(const WCMacroAction& ma);												//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCMacroAction(WCFeature *creator) : ::WCAction("Macro Action", creator) { }						//!< Primary constructor
	~WCMacroAction();																				//!< Default destructor
	
	//Subaction Methods
	void AddAction(WCAction *action);																//!< Add a sub-action
	
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action);																	//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to roll back the action
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__ACTION_H__

