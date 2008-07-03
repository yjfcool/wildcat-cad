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


#include "Utility/object.h"
#include "Utility/log_manager.h"


/*** DEBUG ***/
std::list<WCObject*> WCObject::_debugList;
/*** DEBUG ***/


/***********************************************~***************************************************/


WCObject::WCObject(const bool &debugFlag) : _refSet(), _debugFlag(debugFlag) {
/*** DEBUG ***/
	WCObject::_debugList.push_back(this);
//	std::cout << typeid(this).name() << " " << this << std::endl;
/*** DEBUG ***/	
}


WCObject::~WCObject() {
/*** DEBUG ***/
	WCObject::_debugList.remove(this);
/*** DEBUG ***/
	//Notify all dependent objects that this object has been deleted
	this->SendBroadcastNotice(OBJECT_NOTIFY_DELETE);
	//See how many retains are left
	if (this->_refSet.size() > 0) {
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCObject::~WCObject (" << this << ") - " << this->_refSet.size() << " reference remaining.");
		for (std::list<WCObject*>::iterator iter=this->_refSet.begin(); iter != this->_refSet.end(); iter++) std::cout << "Ref: " << *iter << std::endl;
	}
} 


WPUInt WCObject::Retain(WCObject &obj) {
	//See if the object is already in the list
	for (std::list<WCObject*>::iterator iter = this->_refSet.begin(); iter != this->_refSet.end(); iter++) {
		if (&obj == *iter) return (WPUInt)this->_refSet.size();
	}
/*** DEBUG ***/
	//Check secret debug flag
	if (this->_debugFlag) 
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "Object being retained:" << this->_refSet.size());
/*** DEBUG ***/
	//Put the object into the list
	this->_refSet.push_back(&obj);
	//Now just return the reference count
	return (WPUInt)this->_refSet.size();
}


WPUInt WCObject::Release(WCObject &obj) {
/*** DEBUG ***/
	//Check secret debug flag
	if (this->_debugFlag) 
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "Object being released:" << this->_refSet.size());
/*** DEBUG ***/
	//Otherwise, try to remove the obj from the refSet
	this->_refSet.remove(&obj);
	//Return the number of references
	return (WPUInt)this->_refSet.size();
}
	

void WCObject::SendBroadcastNotice(WCObjectMsg msg) {
	//Loop through all objects and send messages
	for (std::list<WCObject*>::iterator iter = this->_refSet.begin(); iter != this->_refSet.end(); iter++) {
		(*iter)->ReceiveNotice(msg, this);
	}
}


void WCObject::SendNotice(WCObject* object, WCObjectMsg msg) {
	//Check to make sure obj is valid
	if (object != NULL) object->ReceiveNotice(msg, this);
}
	
	
/***********************************************~***************************************************/


void WCObject::DebugReport(void) {
	int zeroCount = 0;
	std::cout << "************************* " << WCObject::_debugList.size() << " objects *************\n";
	std::list<WCObject*>::iterator iter;
	for (iter = WCObject::_debugList.begin(); iter != WCObject::_debugList.end(); iter++) {
		if ((*iter)->RefCount() == 0) zeroCount++;
//		if ((*iter)->RefCount() == 0) delete (*iter);
//		else std::cout << "****** (" << *iter << "): " << (*iter)->RefCount() << " references.\n";
		std::cout << "****** (" << *iter << "): " << (*iter)->RefCount() << " references.\n";
	}
	std::cout << "*** Zero Count: " << zeroCount << std::endl;
	std::cout << "************************* " << WCObject::_debugList.size() << " objects *************\n";
}


/***********************************************~***************************************************/