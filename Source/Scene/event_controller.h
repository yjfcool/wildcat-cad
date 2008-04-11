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


#ifndef __EVENT_CONTROLLER_H__
#define __EVENT_CONTROLLER_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"
#include "Scene/selection_manager.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCTreeElement;


/***********************************************~***************************************************/


class WCEventController : virtual public WCSelectionObject {
protected:
	WCTreeElement								*_treeElement;										//!< Corresponding tree element
private:
	//Deny Access
	WCEventController();																			//!< Deny access to default constructor
	WCEventController(const WCEventController &);													//!< Deny access to copy constructor
	WCEventController& operator=(const WCEventController &);										//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCEventController(WCTreeElement *element) : ::WCObject(), _treeElement(element) { this->Retain(*this); }//!< Primary constructor
	virtual ~WCEventController()				{ this->Release(*this); }							//!< Default destructor

	//Virtual Methods
	virtual void OnContextClick(void)			{ std::cout << "WCEventController - No controller defined.\n"; }
	//Other methods...
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__EVENT_CONTROLLER_H__

