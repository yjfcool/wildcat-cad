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


#ifndef __DRAWING_MODE_H__
#define __DRAWING_MODE_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCDocument;
class WCFeature;
class WCSelectionMode;
class WCWorkbench;


/***********************************************~***************************************************/


class WCDrawingMode {
protected:
	WCFeature									*_creator;											//!< Creating feature
	std::string									_name;												//!< Mode name
private:
	//Deny Access
	WCDrawingMode();																				//!< Deny access to default constructor
	WCDrawingMode(const WCDrawingMode &mode);														//!< Deny access to copy constructor
public:
	//Constructors and Destructors
	WCDrawingMode(WCFeature *creator, const std::string &name) : _creator(creator), _name(name) { }	//!< Primary constructor
	virtual ~WCDrawingMode()					{ }													//!< Default destructor
	
	//Member Access Methods
	inline std::string Name(void) const			{ return this->_name; }								//!< Get the mode name
	
	//Overloaded Operators
	bool operator==(const WCDrawingMode& mode)	{ return (typeid(*this) == typeid(mode)); }			//!< Type check
	
	//Virtual Methods
	virtual void OnEntry(void)									{ }									//!< Handle entry into mode
	virtual void OnExit(void)									{ }									//!< Handle exit from mode
	virtual void OnMouseDown(const WCMouseButton &button)		{ }									//!< Handle mouse button press
	virtual void OnMouseMove(const WPFloat &x, const WPFloat &y){ }									//!< Handle mouse movement
	virtual void OnMouseUp(const WCMouseButton &button)			{ }									//!< Handle mouse button release
	virtual void OnArrowKeyPress(const WCArrowKey &key)			{ }									//!< Handle arrow key press
	virtual void Render(void)									{ }									//!< Render the drawing mode

	//Staic Initializers
	static WCDrawingMode* Default(void)			{ return NULL; }									//!< Default controller
//	static WCDrawingMode* Selection(WCWorkbench* wb);												//!< Selection controller
};


/***********************************************~***************************************************/


#endif //__DRAWING_MODE_H__

