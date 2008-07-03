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


/*** Included Header Files ***/
#include "Application/Win32/stdafx.h"
#include "Application/Win32/wildcat_app.h"
#include "Application/Win32/main_frame.h"
#include "Application/Win32/document_frame.h"
#include "Utility/wutil.h"
#include "Utility/log_manager.h"
#include "Utility/log_appenders.h"
#include <winsock2.h>


/***********************************************~***************************************************/


void InitializeApplication(void) {
	//Initialize logger manager
	WCLogManager::Initialize();
	//Set root appender to a file appender
	WCFileAppender *appender = new WCFileAppender("wildcat.log", false);
	WCLogManager::RootLogger()->SetAppender(appender);
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCWildcatApp::WCWildcatApp - Starting up...");
	//Initialize xml manager
	xercesc::XMLPlatformUtils::Initialize();

	//Initialize TCP/IP stack
	WSADATA wsaData = {0};
	WORD wVer = MAKEWORD(2,2);
	WSAStartup( wVer, &wsaData );
}


void ShutdownApplication(void) {
	//Shutdown winsock
	WSACleanup();
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCWildcatApp::~WCWildcatApp - Shutting Down...");
	//Terminate the managers
	xercesc::XMLPlatformUtils::Terminate();
	WCLogManager::Terminate();
}


/***********************************************~***************************************************/


BEGIN_MESSAGE_MAP(WCWildcatApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &WCWildcatApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &WCWildcatApp::OnFileNew)
END_MESSAGE_MAP()


WCWildcatApp::WCWildcatApp() {
	//Any other initialization code here...
}


WCWildcatApp::~WCWildcatApp() {
	//Shutdown the Wildcat infrastructure
	ShutdownApplication();
}

WCWildcatApp theApp;


BOOL WCWildcatApp::InitInstance() {
	//Initialize the Wildcat infrastructure
	InitializeApplication();

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMDIFrameWnd* pFrame = new WCMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create main MDI frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	// try to load shared MDI menus and accelerator table

	//TODO: add additional member variables and load calls for
	//	additional menu types your application may need
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_WildcatTYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_WildcatTYPE));

	// The main window has been initialized, so show and update it
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	return TRUE;
}


int WCWildcatApp::ExitInstance()  {
	//TODO: handle additional resources you may have added
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}


void WCWildcatApp::OnFileNew()  {
	WCMainFrame* pFrame = STATIC_DOWNCAST(WCMainFrame, m_pMainWnd);
	// create a new MDI child window
	pFrame->CreateNewChild( RUNTIME_CLASS(WCDocumentFrame), IDR_WildcatTYPE, m_hMDIMenu, m_hMDIAccel );
}


void WCWildcatApp::OnAppAbout() {
//	CAboutDlg aboutDlg;
//	aboutDlg.DoModal();
}


/***********************************************~***************************************************/

