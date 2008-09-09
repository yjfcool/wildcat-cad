// main_frame.h

#include "wx/mdi.h"
#include "wx/docview.h"
#include "wx/docmdi.h"
#include <wx/aui/aui.h>

// Define a new frame
class WCPartCanvas;
class WCMainFrame: public wxDocMDIParentFrame
{
	DECLARE_CLASS(WCMainFrame)
public:
	wxMenu *editMenu;
	wxAuiManager* m_aui_manager;
	wxToolBarBase *m_toolBar;

	WCMainFrame(wxDocManager *manager, wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size,
		long type);

	void OnAbout(wxCommandEvent& event);
	WCPartCanvas *CreateCanvas(wxView *view, wxMDIChildFrame *parent);

	DECLARE_EVENT_TABLE()
};

#define DOCVIEW_CUT     1
#define DOCVIEW_ABOUT   wxID_ABOUT

extern bool singleWindowMode;
