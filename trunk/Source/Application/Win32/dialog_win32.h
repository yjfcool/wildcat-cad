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


#ifndef __DIALOG_WIN32_H__
#define __DIALOG_WIN32_H__


/*** Included Header Files ***/
#include "Application/dialog.h"


/*** Locally Defined Values ***/
//None


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCDialog_Win32 {
private:
	__WILDCAT_NAMESPACE__::WCDialog				*_dialog;											//!< Pointer to dialog
public:
	//Constructors and Destructors
	WCDialog_Win32(WCDialog* dialog);																//!< Primary constructor
	~WCDialog_Win32();																				//!< Default destructor

	//Member Access Methods
	inline __WILDCAT_NAMESPACE__::WCDialog* Dialog(void)	{ return this->_dialog; }				//!< Get associated dialog

	//Script Get Object Methods
	std::string StringFromScript(const std::string &var);											//!< Get string script value
	WPFloat FloatFromScript(const std::string &var);												//!< Get float script value
	WPInt IntFromScript(const std::string &var);													//!< Get int script value
	WPUInt UnsignedIntFromScript(const std::string &var);											//!< Get unsigned int script value
	bool BoolFromScript(const std::string &var);													//!< Get bool script value

	//Script Set Object Methods
	void StringFromScript(const std::string &var, const std::string &value);						//!< Set string script value
	void FloatFromScript(const std::string &var, const WPFloat &value);								//!< Set float script value
	void IntFromScript(const std::string &var, const WPInt &value);									//!< Set int script value
	void UnsignedIntFromScript(const std::string &var, const WPUInt &value);						//!< Set unsigned int script value
	void BoolFromScript(const std::string &var, const bool &value);									//!< Set bool script value
};


/***********************************************~***************************************************/


#endif //__DIALOG_WIN32_H__

