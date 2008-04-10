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


#ifndef __VIS_LISTENER_ACTION_H__
#define __VIS_LISTENER_ACTION_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/action.h"
#include "RTVisualization/vis_listener_types.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCVisListener;


/***********************************************~***************************************************/


class WCActionVisListenerCreate : public WCAction {
private:
	WCGUID										_visGUID;											//!< GUID for associated objects
	WCVisListenerType							_type;												//!< Listener type
	std::string									_listenerName;										//!< Listener name
	unsigned int								_port;												//!< Listener port
	WCVisListener								*_listener;											//!< Post-creation object
	//Hidden Constructors
	WCActionVisListenerCreate();																	//!< Deny access to default constructor
	WCActionVisListenerCreate(const WCActionVisListenerCreate&);									//!< Deny access to copy constructor
	WCActionVisListenerCreate& operator=(const WCActionVisListenerCreate&);							//!< Deny access to equals operator
	WCActionVisListenerCreate(WCFeature *creator, const std::string &listenerName,					//!< Primary constructor
												const WCVisListenerType &type, const unsigned int &port);
	//Friend Declarations
	friend class WCVisListener;																		//!< Listener is a friend
public:
	//Constructors and Destructors
	WCActionVisListenerCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary);		//!< Persistance constructor
	~WCActionVisListenerCreate()				{ }													//!< Default destructor

	//Member Access Methods
	inline WCVisListener* Listener(void)		{ return this->_listener; }							//!< Get the listener

	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionVisListenerModify : public WCAction {
private:
	WCVisListener								*_listener;											//!< Post-creation object
	WCVisListenerType							_type;												//!< Listener type
	std::string									_listenerName;										//!< Listener name
	unsigned int								_port;												//!< Listener port
	//Hidden Constructors
	WCActionVisListenerModify();																	//!< Deny access to default constructor
	WCActionVisListenerModify(const WCActionVisListenerModify&);									//!< Deny access to copy constructor
	WCActionVisListenerModify& operator=(const WCActionVisListenerModify&);							//!< Deny access to equals operator
	WCActionVisListenerModify(WCVisListener *listener, const std::string &listenerName,				//!< Primary constructor
												const WCVisListenerType &type, const unsigned int &port);
	//Friend Declarations
	friend class WCVisListener;																		//!< Listener is a friend
public:
	//Constructors and Destructors
	WCActionVisListenerModify(xercesc::DOMElement *element, WCSerialDictionary *dictionary);		//!< Persistance constructor
	~WCActionVisListenerModify()				{ }													//!< Default destructor

	//Member Access Methods
	inline WCVisListener* Listener(void)		{ return this->_listener; }							//!< Get the listener

	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


#endif //__VIS_LISTENER_ACTION_H__

