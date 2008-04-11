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


#ifndef __OBJECT_H__
#define __OBJECT_H__


//Other headers
#include "Utility/wutil.h"


//Local Defines
#define OBJECT_NOTIFY_UPDATE					0x01
#define OBJECT_NOTIFY_DELETE					0x02


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
typedef	unsigned int							WCObjectMsg;


/***********************************************~***************************************************/


class WCObject {
protected:
	std::list<WCObject*>						_refSet;											//!< List of dependent objects
	bool										_debugFlag;											//!< Super secret debug flag
private:
/*** DEBUG ***/
	static std::list<WCObject*>					_debugList;											//!< Debug global list of all objects
/*** DEBUG ***/
	//Deny Access
//	WCObject();																						//!< Deny access to default constructor
	WCObject(const WCObject& obj);																	//!< Deny access to copy constructor
	WCObject& operator=(const WCObject& obj);														//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCObject(const bool &debugFlag=false);															//!< Default constructor
	virtual ~WCObject();																			//!< Default destructor 

	//Reference Dependency Methods
	virtual WPUInt Retain(WCObject &obj);															//!< Reference counting increment
	virtual WPUInt Release(WCObject &obj);															//!< Reference counting decrement
	virtual inline WPUInt RefCount(void) const	{ return this->_refSet.size(); }					//!< Return the reference count
	virtual inline std::list<WCObject*> RefSet(void) { return this->_refSet; }						//!< Return the reference set
	
	//Notification Methods
	virtual void SendBroadcastNotice(WCObjectMsg msg);												//!< Send message to all referenced objects
	virtual void SendNotice(WCObject* obj, WCObjectMsg msg);										//!< Send message to specific receiver
	virtual inline void ReceiveNotice(WCObjectMsg msg, WCObject *sender){ }							//!< Receive message
	
	//Additional Virtual Methods
	virtual void inline operator()(void)		{ }													//!< Functor interface
	
/*** DEBUG ***/
	static void DebugReport(void);																	//!< Report global debug list
/*** DEBUG ***/
};	


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__OBJECT_H__

