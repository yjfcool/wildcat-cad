// Wildcat.h : main header file for the Wildcat application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "Application/Win32/resource.h"       // main symbols


/*** Class Predefines ***/
class WCWildcatApp;


/*** Extern Variables ***/
extern WCWildcatApp theApp;



class WCWildcatApp : public CWinApp {
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;
public:
	//Constructors and Destructors
	WCWildcatApp();

	//Other messages
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	DECLARE_MESSAGE_MAP()

	//Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};
