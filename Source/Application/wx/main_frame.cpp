// main_frame.cpp

#include "stdafx.h"
#include "main_frame.h"
#include "part_canvas.h"
#include "wildcat_app.h"

/*
 * This is the top-level window of the application.
 */
 
IMPLEMENT_CLASS(WCMainFrame, wxDocMDIParentFrame)
BEGIN_EVENT_TABLE(WCMainFrame, wxDocMDIParentFrame)
    EVT_MENU(DOCVIEW_ABOUT, WCMainFrame::OnAbout)
END_EVENT_TABLE()

WCMainFrame::WCMainFrame(wxDocManager *manager, wxFrame *frame, const wxString& title,
    const wxPoint& pos, const wxSize& size, long type):
  wxDocMDIParentFrame(manager, frame, wxID_ANY, title, pos, size, type, _T("WCMainFrame"))
{
  editMenu = (wxMenu *) NULL;
	
	// AUI manager allows dockable/split frames
	m_aui_manager = new wxAuiManager(this);

	// main tool bar
  	m_toolBar = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_NODIVIDER | wxTB_FLAT);
	m_toolBar->SetToolBitmapSize(wxSize(32, 32));
    m_toolBar->AddTool(wxID_NEW, _T("New"), wxBitmap(wxGetApp().GetExeFolder() + _T("/Resources/new32.tiff"), wxBITMAP_TYPE_TIF), _T("New part"));
    m_toolBar->Realize();

  	m_aui_manager->AddPane(m_toolBar, wxAuiPaneInfo().Name(_T("ToolBar")).Caption(_T("General Tools")).ToolbarPane().Top());
	m_aui_manager->Update();
}

void WCMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
	wxMessageBox(_T("Wildcat CAD\nAuthor: Graham Hemingway\nhttp://code.google.com/p/wildcat-cad/\n\nContributors:\nDan Heeks\n\nBuilt with wxWidgets\nwww.wxwidgets.org"), _T("About Wildcat"));
}

// Creates a canvas. Called from view.cpp when a new drawing
// view is created.
WCPartCanvas *WCMainFrame::CreateCanvas(wxView *view, wxMDIChildFrame *parent)
{
	int graphics_attrib_list[] = {
		WX_GL_RGBA,
		1,
		WX_GL_DOUBLEBUFFER,
		1,
		WX_GL_DEPTH_SIZE,
		1,
		WX_GL_MIN_RED,
		1,
		WX_GL_MIN_GREEN,
		1,
		WX_GL_MIN_BLUE,
		1,
		WX_GL_MIN_ALPHA,
		0,
		0
	};

	// Non-retained canvas
	WCPartCanvas *canvas = new WCPartCanvas(view, parent, graphics_attrib_list);
	canvas->SetCursor(wxCursor(wxCURSOR_PENCIL));

	return canvas;
}
