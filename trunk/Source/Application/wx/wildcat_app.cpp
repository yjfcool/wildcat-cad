// wildcat_app.h

#include "stdafx.h"
#include "wx/docview.h"
#include <wx/stdpaths.h>

#include "wildcat_app.h"
#include "main_frame.h"
#include "part_document.h"
#include "part_view.h"
#include "Utility/wutil.h"
#include "Utility/log_manager.h"
#include "Utility/log_appenders.h"

IMPLEMENT_APP(WCWildcatApp)

WCWildcatApp::WCWildcatApp(void)
{
    m_docManager = (wxDocManager *) NULL;
	m_frame = NULL;
}

bool WCWildcatApp::OnInit(void)
{
	//Initialize logger manager
	WCLogManager::Initialize();
	//Set root appender to a file appender
	WCFileAppender *appender = new WCFileAppender("wildcat.log", false);
	WCLogManager::RootLogger()->SetAppender(appender);
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCWildcatApp::WCWildcatApp - Starting up...");
	//Initialize xml manager
	xercesc::XMLPlatformUtils::Initialize();

	wxInitAllImageHandlers();

  //// Create a document manager
  m_docManager = new wxDocManager;

  //// Create a template relating part documents to their views
  (void) new wxDocTemplate(m_docManager, _T("Text"), _T("*.txt"), _T(""), _T("txt"), _T("Text Doc"), _T("Text View"),
          CLASSINFO(WCPartDocument), CLASSINFO(WCPartView));

  //// Create the main frame window
  m_frame = new WCMainFrame((wxDocManager *) m_docManager, (wxFrame *) NULL,
                      _T("Wildcat"), wxPoint(0, 0), wxSize(500, 400),
                      wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

  //// Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
  m_frame->SetIcon(wxICON(Wildcat));
#endif
#ifdef __X__
  m_frame->SetIcon(wxIcon(_T("doc.xbm")));
#endif

  //// Make a menubar
  wxMenu *file_menu = new wxMenu;
  wxMenu *edit_menu = (wxMenu *) NULL;

  file_menu->Append(wxID_NEW, _T("&New...\tCtrl-N"));
  file_menu->Append(wxID_OPEN, _T("&Open...\tCtrl-X"));

  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, _T("E&xit\tAlt-X"));
  
  // A nice touch: a history of files visited. Use this menu.
  m_docManager->FileHistoryUseMenu(file_menu);

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(DOCVIEW_ABOUT, _T("&About\tF1"));

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));
  if (edit_menu)
    menu_bar->Append(edit_menu, _T("&Edit"));
  menu_bar->Append(help_menu, _T("&Help"));

#ifdef __WXMAC__
  wxMenuBar::MacSetCommonMenuBar(menu_bar);
#endif //def __WXMAC__
  //// Associate the menu bar with the frame
  m_frame->SetMenuBar(menu_bar);

  m_frame->Centre(wxBOTH);
#ifndef __WXMAC__
  m_frame->Show(true);
#endif //ndef __WXMAC__

  SetTopWindow(m_frame);
  return true;
}

int WCWildcatApp::OnExit(void)
{
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCWildcatApp::~WCWildcatApp - Shutting Down...");
	//Terminate the managers
	xercesc::XMLPlatformUtils::Terminate();
	WCLogManager::Terminate();

	delete m_docManager;
    return 0;
}

/*
 * Centralised code for creating a document frame.
 * Called from view.cpp, when a view is created.
 */
 
wxMDIChildFrame *WCWildcatApp::CreateChildFrame(wxDocument *doc, wxView *view, bool isCanvas)
{
  //// Make a child frame
  wxDocMDIChildFrame *subframe =
      new wxDocMDIChildFrame(doc, view, m_frame, wxID_ANY, _T("Child Frame"),
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

wxString WCWildcatApp::GetExeFolder()const
{
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

	return exedir;
}
