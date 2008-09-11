// main_frame.h

#include "wx/mdi.h"
#include "wx/docview.h"
#include "wx/docmdi.h"
#include <wx/aui/aui.h>
#include <map>

#define MAX_TOOLS 10000

enum{
	FIRST_TOOLBAR_BUTTON_ID = 1,
	NEXT_ID_AFTER_TOOLBAR_BUTTONS = FIRST_TOOLBAR_BUTTON_ID + MAX_TOOLS
};

// Define a new frame
class WCPartCanvas;
class WCMainFrame: public wxDocMDIParentFrame
{
	DECLARE_CLASS(WCMainFrame)
public:
	wxMenu *editMenu;
	wxAuiManager* m_aui_manager;
	wxToolBarBase *m_toolBar;
	std::map<int, wxToolBarToolBase*> m_id_button_map;

	WCMainFrame(wxDocManager *manager, wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size,
		long type);

	void OnAbout(wxCommandEvent& event);
	WCPartCanvas *CreateCanvas(wxView *view, wxMDIChildFrame *parent);
	wxToolBarToolBase* AddToolBarTool(wxToolBar* toolbar, int tool_id, const wxString& title, wxBitmap& bitmap, const wxString& caption);
	void OnToolBarButton( wxCommandEvent& event );
	void OnUpdateToolBarButton( wxUpdateUIEvent& event );

	DECLARE_EVENT_TABLE()
};

#define DOCVIEW_CUT     1
#define DOCVIEW_ABOUT   wxID_ABOUT

extern bool singleWindowMode;
