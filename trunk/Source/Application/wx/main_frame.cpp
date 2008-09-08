// main_frame.cpp

#include "stdafx.h"
#include "main_frame.h"
#include "part_canvas.h"

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
}

void WCMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox(_T("DocView Demo\nAuthor: Julian Smart\nUsage: docview.exe"), _T("About DocView"));
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
