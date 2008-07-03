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


#ifndef __VIS_LISTENER_TYPES_H__
#define __VIS_LISTENER_TYPES_H__


/*** Included Header Files ***/
#include "RTVisualization/wrtv.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


//Values for visualization state
#define VISLISTENER_UDP							0
#define VISLISTENER_TCP							1

class WCVisListenerType {
private:
	unsigned int								_type;												//!< Listener type
	//Hidden Constructors
	WCVisListenerType();																			//!< Deny access to default constructor 
	WCVisListenerType(const int &type)			{ this->_type = type; }								//!< Hidden primary constructor
public:
	//Constructors and Destructors
	WCVisListenerType(const WCVisListenerType &type) : _type(type._type)	{ }						//!< Copy constructor
	static WCVisListenerType UDP(void)			{ return WCVisListenerType(VISLISTENER_UDP); }		//!< Stopped state
	static WCVisListenerType TCP(void)			{ return WCVisListenerType(VISLISTENER_TCP); }		//!< Running state
	//Overridder Operators
	WCVisListenerType& operator=(const WCVisListenerType &type) { this->_type = type._type; return *this; }//!< State equation				
	bool operator==(const WCVisListenerType &type) const { return this->_type == type._type; }		//!< Equals operator
	bool operator!=(const WCVisListenerType &type) const { return this->_type != type._type; }		//!< Inequality operator
	//Serialization Methods
	void ToElement(xercesc::DOMElement *element)	{ WCSerializeableObject::AddFloatAttrib(element, "type", this->_type); }
	void FromElement(xercesc::DOMElement *element)	{ this->_type = (unsigned int)WCSerializeableObject::GetFloatAttrib(element, "type"); }
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__VIS_LISTENER_TYPES_H__

