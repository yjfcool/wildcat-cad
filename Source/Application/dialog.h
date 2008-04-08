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


#ifndef __DIALOG_H__
#define __DIALOG_H__


/*** Included Headers ***/
#include "Application/wildcat.h"


/*** Local Defines ***/
//None


/*** C++ Class Predefines ***/
class WCDialogManager;
class WCDialogController;


/***********************************************~***************************************************/


//Values for dialog modes
#define DIALOGMODE_DEFAULT						0
#define DIALOGMODE_CACHED						1
#define DIALOGMODE_DYNAMIC						2

class WCDialogMode {
private:
	unsigned int								_mode;												//!< Dialog mode
	//Hidden Constructors
	WCDialogMode();																					//!< Deny access to default constructor 
	WCDialogMode(const int &mode)				{ this->_mode = mode; }								//!< Hidden primary constructor
public:
	//Constructors and Destructors
	WCDialogMode(const WCDialogMode &mode) : _mode(mode._mode)	{ }									//!< Copy constructor
	static WCDialogMode Default(void)			{ return WCDialogMode(DIALOGMODE_DEFAULT); }		//!< Default mode
	static WCDialogMode Cached(void)			{ return WCDialogMode(DIALOGMODE_CACHED); }			//!< Cached mode
	static WCDialogMode Dynamic(void)			{ return WCDialogMode(DIALOGMODE_DYNAMIC); }		//!< Dynamic mode
	//Overridder Operators
	WCDialogMode& operator=(const WCDialogMode &mode)	{ this->_mode = mode._mode; return *this; }	//!< State equation				
	bool operator==(const WCDialogMode &mode) const { return this->_mode == mode._mode; }			//!< Equals operator
	bool operator!=(const WCDialogMode &mode) const { return this->_mode != mode._mode; }			//!< Inequality operator
};


/***********************************************~***************************************************/


class WCDialog {
private:
	std::string									_name;												//!< Both the reference name and filename
	unsigned int								_winWidth, _winHeight;								//!< Window width and height
	bool										_modal, _boundary, _isOpen;							//!< Is dialog modal flag
	WCDialogMode								_mode;												//!< Mode of the dialog
	WCDialogController							*_controller;										//!< Associated controller
	void										*_window;											//!< Void* to the underlying window
	//Friend Declarations
	friend class WCDialogManager;																	//!< Let DialogManager be a friend
	void Controller(WCDialogController *controller);												//!< Set the dialog controller
	//Hidden Constructors
	WCDialog();																						//!< Deny access to default constructor
	WCDialog(const WCDialog&);																		//!< Deny access to copy constructor
	WCDialog& operator=(const WCDialog&);															//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCDialog(const std::string &name, const unsigned int &width, const unsigned int &height,		//!< Primary constructor
												const bool &modal, const bool &boundary, const WCDialogMode &mode);
	~WCDialog();																					//!< Default destructor

	//Member Access Methods
	inline std::string Name(void)				{ return this->_name; }								//!< Get the name
	inline unsigned int Width(void)				{ return this->_winWidth; }							//!< Get the window width
	inline unsigned int Height(void)			{ return this->_winHeight; }						//!< Get the window height
	inline bool IsModal(void)					{ return this->_modal; }							//!< Get the modal flag
	inline bool IsBoundary(void)				{ return this->_boundary; }							//!< Get the boundary flag
	inline bool IsOpen(void)					{ return this->_isOpen; }							//!< Get the open flag
	inline WCDialogMode Mode(void)				{ return this->_mode; }								//!< Get the mode
	inline WCDialogController* Controller(void)	{ return this->_controller; }						//!< Get the controller

	//Window Controller
	void CloseWindow(const bool &fromWindow=false);													//!< Notice that the window is closing

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


class WCDialogController : public WCObject {
protected:
	WCDialog									*_dialog;											//!< Associated dialog
private:
	//Friend Declarations
	friend class WCDialogManager;																	//!< Let dialog manager be a friend
	void Dialog(WCDialog *dialog)				{ this->_dialog = dialog; }							//!< Let dialog manager set the associated dialog
public:
	WCDialogController() : _dialog(NULL)		{ }													//!< Primary constructor
	virtual ~WCDialogController()				{ }													//!< Default destructor

	//Required Methods
	virtual void ReceiveMessage(const std::string &message)=0;										//!< Receive message from a dialog
	virtual std::string GeneratePage(void)		{ return "<HTML></HTML>"; }							//!< Generate a page
	virtual void ReceiveNotice(WCObjectMsg msg, WCObject *sender) { }								//!< Receive message
};


/***********************************************~***************************************************/


#endif //__DIALOG_H__

