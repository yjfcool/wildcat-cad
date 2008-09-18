// wildcat_app.h

// created by Dan Heeks August 2008, based on docview sample Julian Smart 04/01/98
// some code copied from HeeksCAD

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
#include "Application/wx/wildcat_app.h"
#include "Application/wx/main_frame.h"
#include "Application/wx/part_document.h"
#include "Application/wx/part_view.h"
#include "Utility/wutil.h"
#include "Utility/log_manager.h"
#include "Utility/log_appenders.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


IMPLEMENT_APP(WCWildcatApp)


/***********************************************~***************************************************/


WCWildcatApp::WCWildcatApp(void) : _docManager(NULL), _frame(NULL), _config(NULL) {
	//Nothing else for now
}


WCWildcatApp::~WCWildcatApp(void) {
	//Delete the config if present
	if (this->_config) delete _config;
}


bool WCWildcatApp::OnInit(void) {
	//Create new wxConfig
	this->_config = new wxConfig(_T("Wildcat"));

	int width = 600;
	int height = 400;
	int posx = 200;
	int posy = 200;
	this->_config->Read(_T("MainFrameWidth"), &width);
	this->_config->Read(_T("MainFrameHeight"), &height);
	this->_config->Read(_T("MainFramePosX"), &posx);
	this->_config->Read(_T("MainFramePosY"), &posy);
	if(posx < 0)posx = 0;
	if(posy < 0)posy = 0;

	//Initialize logger manager
	WCLogManager::Initialize();
	//Set root appender to a file appender
	WCFileAppender *appender = new WCFileAppender("wildcat.log", false);
	WCLogManager::RootLogger()->SetAppender(appender);
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCWildcatApp::WCWildcatApp - Starting up...");
	//Initialize xml manager
	xercesc::XMLPlatformUtils::Initialize();
	//Initialize wx image handlers
	wxInitAllImageHandlers();

	//Create a document manager
	this->_docManager = new wxDocManager;

	//Create a template relating part documents to their views
	(void) new wxDocTemplate(this->_docManager, _T("Text"), _T("*.txt"), _T(""), _T("txt"), _T("Text Doc"), _T("Text View"),
							 CLASSINFO(WCPartDocument), CLASSINFO(WCPartView));
	//Create the main frame window
	this->_frame = new WCMainFrame(this->_docManager, (wxFrame*)NULL, _T("Wildcat"), wxPoint(posx, posy), wxSize(width, height),
									wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);
	//Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
	this->_frame->SetIcon(wxICON(Wildcat));
#endif
#ifdef __X__
	this->_frame->SetIcon(wxIcon(_T("doc.xbm")));
#endif
	//Make a menubar
	wxMenu *file_menu = new wxMenu;
	wxMenu *edit_menu = (wxMenu *) NULL;
	file_menu->Append(wxID_NEW, _T("&New...\tCtrl-N"));
	file_menu->Append(wxID_OPEN, _T("&Open...\tCtrl-X"));
	file_menu->AppendSeparator();
	file_menu->Append(wxID_EXIT, _T("E&xit\tAlt-X"));
	//A nice touch: a history of files visited. Use this menu.
	this->_docManager->FileHistoryUseMenu(file_menu);
	//Create the help menu
	wxMenu *help_menu = new wxMenu;
	help_menu->Append(DOCVIEW_ABOUT, _T("&About\tF1"));
	//Create the file menu
	wxMenuBar *menu_bar = new wxMenuBar;
	menu_bar->Append(file_menu, _T("&File"));
	//Create edit menu
	if (edit_menu) menu_bar->Append(edit_menu, _T("&Edit"));
	menu_bar->Append(help_menu, _T("&Help"));

#ifdef __WXMAC__
	wxMenuBar::MacSetCommonMenuBar(menu_bar);
#endif //def __WXMAC__
	// Associate the menu bar with the frame
	this->_frame->SetMenuBar(menu_bar);
#ifndef __WXMAC__
	this->_frame->Show(true);
#endif //ndef __WXMAC__
	SetTopWindow(this->_frame);
	return true;
}


int WCWildcatApp::OnExit(void) {
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCWildcatApp::~WCWildcatApp - Shutting Down...");
	//Delete the document manager if present
	if (this->_docManager) delete this->_docManager;
	//Terminate the managers
	xercesc::XMLPlatformUtils::Terminate();
	WCLogManager::Terminate();
    return 0;
}


/*
 * Centralised code for creating a document frame.
 * Called from view.cpp, when a view is created.
 */
wxMDIChildFrame *WCWildcatApp::CreateChildFrame(wxDocument *doc, wxView *view, bool isCanvas) {
  //// Make a child frame
  wxDocMDIChildFrame *subframe =
      new wxDocMDIChildFrame(doc, view, this->_frame, wxID_ANY, _T("Child Frame"),
                             wxPoint(10, 10), wxSize(300, 300),
                             wxDEFAULT_FRAME_STYLE |
                             wxNO_FULL_REPAINT_ON_RESIZE);

#ifdef __WXMSW__
  subframe->SetIcon(wxString(isCanvas ? _T("chart") : _T("notepad")));
#endif
#ifdef __X__
  subframe->SetIcon(wxIcon(_T("doc.xbm")));
#endif

  //// Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(wxID_NEW, _T("&New..."));
  file_menu->Append(wxID_OPEN, _T("&Open..."));
  file_menu->Append(wxID_CLOSE, _T("&Close"));
  file_menu->Append(wxID_SAVE, _T("&Save"));
  file_menu->Append(wxID_SAVEAS, _T("Save &As..."));

  if (isCanvas)
  {
    file_menu->AppendSeparator();
    file_menu->Append(wxID_PRINT, _T("&Print..."));
    file_menu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
    file_menu->Append(wxID_PREVIEW, _T("Print Pre&view"));
  }

  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, _T("E&xit"));

  wxMenu *edit_menu = (wxMenu *) NULL;

  if (isCanvas)
  {
    edit_menu = new wxMenu;
    edit_menu->Append(wxID_UNDO, _T("&Undo"));
    edit_menu->Append(wxID_REDO, _T("&Redo"));
    edit_menu->AppendSeparator();
    edit_menu->Append(DOCVIEW_CUT, _T("&Cut last segment"));
  }

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(DOCVIEW_ABOUT, _T("&About"));

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));
  if (isCanvas)
    menu_bar->Append(edit_menu, _T("&Edit"));
  menu_bar->Append(help_menu, _T("&Help"));

  //// Associate the menu bar with the frame
  subframe->SetMenuBar(menu_bar);

  return subframe;
}


wxString WCWildcatApp::GetExeFolder()const {
	wxStandardPaths sp;
	wxString exepath = sp.GetExecutablePath();
	int last_fs = exepath.Find('/', true);
	int last_bs = exepath.Find('\\', true);
	wxString exedir;
	if(last_fs > last_bs){
		exedir = exepath.Truncate(last_fs);
	}
	else{
		exedir = exepath.Truncate(last_bs);
	}
	//Return the exe directory
	return exedir;
}


WCDocument* WCWildcatApp::GetWCDocument() {
	wxDocument* doc = this->_docManager->GetCurrentDocument();
	if (doc == NULL) return NULL;
	return ((WCPartDocument*)doc)->Document();
}


/***********************************************~***************************************************/

