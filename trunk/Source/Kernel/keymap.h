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


#ifndef __KEYMAP_H__
#define __KEYMAP_H__


/*** Included Header Files ***/
#include <Application/wildcat.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCKeyMap;


/***********************************************~***************************************************/


class WCKeyEvent {
private:
	char										_key;												//!< The actual key pressed
	bool										_control, _option, _alt, _command, _esc;			//!< Modifier keys
	//Deny Access
	WCKeyEvent();																					//!< Deny access to default constructor
	//Friend Classes
	friend class WCKeyMap;																			//!< Declare keymap a friend class
public:
	//Constructors
	WCKeyEvent(const WCKeyEvent& event) : _key(event._key), _control(event._control),				//!< Copy constructor
												_option(event._option), _alt(event._alt),
												_command(event._command), _esc(event._esc) { }
	WCKeyEvent(const char &key) : _key(key), _control(false), _option(false), _alt(false),			//!< Primary constructor 
												_command(false), _esc(false) { }
	WCKeyEvent(const char &key, const bool &control, const bool &option, const bool &alt,			//!< Alternate constructor
												const bool &command, const bool escape) : 
												_key(key), _control(control), _option(option), _alt(alt), 
												_command(command), _esc(escape) { }

	//Operator Overloads
	WCKeyEvent& operator=(const WCKeyEvent& event) { this->_key = event._key;						//!< Equals operator overload
													 this->_control = event._control;
													 this->_option = event._option;
													 this->_alt = event._alt;
													 this->_command = event._command;
													 this->_esc = event._esc; 
													return *this; }
	bool operator==(const WCKeyEvent& event)	{ if ( (this->_key == event._key) && (this->_control == event._control) && 
													   (this->_option == event._option) && (this->_alt == event._alt) && 
													   (this->_command == event._command) && (this->_esc == event._esc) ) return true;
												  else return false; }
};


/***********************************************~***************************************************/


class WCKeyMap {
private:
	std::multimap<char, std::pair<WCKeyEvent,WCUserMessage> > _keyMap;								//!< Mapping from key to message
	
public:
	//Constructors and Destructors
	WCKeyMap() : _keyMap()						{ }													//!< Default constructor
	~WCKeyMap()									{ }													//!< Default destructor

	//KeyEvent Access Methods
	void AddMapping(const WCKeyEvent &event, const WCUserMessage &message);							//!< Add key mapping
	WCUserMessage MessageFromKeyEvent(const WCKeyEvent &event);										//!< Get the user message from the key event
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__KEYMAP_H__

