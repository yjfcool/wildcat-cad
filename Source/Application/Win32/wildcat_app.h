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


#ifndef __WILDCAT_APP_H__
#define __WILDCAT_APP_H__


#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif
/*** Included Header Files ***/
#include "Application/Win32/resource.h"       // main symbols


/*** Class Predefines ***/
class WCWildcatApp;


/*** Extern Variables ***/
extern WCWildcatApp theApp;


/***********************************************~***************************************************/


class WCWildcatApp : public CWinApp {
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;
public:
	//Constructors and Destructors
	WCWildcatApp();																					//!< Default constructor
	virtual ~WCWildcatApp();																		//!< Default destructor

	//Other messages
	afx_msg void OnAppAbout();																		//!< About message
	afx_msg void OnFileNew();																		//!< File new message
	DECLARE_MESSAGE_MAP()																			//!< Default declare message map

	//Overrides	
	virtual BOOL InitInstance();																	//!< Overridden WinApp method
	virtual int ExitInstance();																		//!< Overridden WinApp method
};


/***********************************************~***************************************************/


#endif //__WILDCAT_APP_H__

