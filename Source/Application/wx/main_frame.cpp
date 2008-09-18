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
#include "Application/wx/main_frame.h"
#include "Application/wx/part_canvas.h"
#include "Application/wx/wildcat_app.h"
#include "Kernel/workbench.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/ 


//IMPLEMENT_CLASS(WCMainFrame, wxDocMDIParentFrame)
BEGIN_EVENT_TABLE(WCMainFrame, wxDocMDIParentFrame)
EVT_SIZE(WCMainFrame::OnSize)
EVT_SIZING(WCMainFrame::OnSize)
EVT_MOVE(WCMainFrame::OnMove)
EVT_MENU(DOCVIEW_ABOUT, WCMainFrame::OnAbout)
EVT_MENU_RANGE(FIRST_TOOLBAR_BUTTON_ID, FIRST_TOOLBAR_BUTTON_ID + MAX_TOOLS, WCMainFrame::OnToolBarButton)
EVT_UPDATE_UI_RANGE(FIRST_TOOLBAR_BUTTON_ID, FIRST_TOOLBAR_BUTTON_ID + MAX_TOOLS, WCMainFrame::OnUpdateToolBarButton)
END_EVENT_TABLE()


/***********************************************~***************************************************/


WCMainFrame::WCMainFrame(wxDocManager *manager, wxFrame *frame, const wxString& title,
    const wxPoint& pos, const wxSize& size, long type):
	::wxDocMDIParentFrame(manager, frame, wxID_ANY, title, pos, size, type, _T("WCMainFrame")),
	_editMenu(NULL), _auiManager(NULL), _idButtonMap(), _toolbarManager(NULL) {
	// AUI manager allows dockable/split frames
	this->_auiManager = new wxAuiManager(this);
	//Create toolbar manager
	std::string resourcesDirectory = _ResourceDirectory();
	this->_toolbarManager = new WCToolbarManager((WCDocument*)this, std::string("toolbar_manifest.xml"), resourcesDirectory, false);
	//Read layout
	wxString str;
	wxGetApp().Config()->Read(_T("AuiPerspective"), &str, _T("layout2|name=mdiclient;caption=;state=256;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=0;besth=0;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=Standard;caption=Standard;state=2108156;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=390;besth=39;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=537;floaty=592;floatw=407;floath=65|name=View;caption=View;state=2108156;dir=1;layer=10;row=0;pos=401;prop=100000;bestw=234;besth=39;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=790;floaty=500;floatw=251;floath=65|name=Part Design;caption=Part Design;state=2108156;dir=1;layer=10;row=0;pos=696;prop=100000;bestw=195;besth=39;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=565;floaty=385;floatw=212;floath=65|name=Sketcher;caption=Sketcher;state=2108156;dir=1;layer=10;row=3;pos=674;prop=100000;bestw=234;besth=39;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=987;floaty=474;floatw=251;floath=65|name=Profiles;caption=Profiles;state=2108156;dir=1;layer=10;row=3;pos=0;prop=100000;bestw=663;besth=39;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=799;floaty=606;floatw=680;floath=65|name=Operations;caption=Operations;state=2108156;dir=1;layer=10;row=0;pos=646;prop=100000;bestw=39;besth=39;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=517;floaty=503;floatw=56;floath=65|dock_size(5,0,0)=10|dock_size(1,10,0)=41|dock_size(1,10,3)=41|"));
	this->_auiManager->LoadPerspective(str);
}


WCMainFrame::~WCMainFrame() {
	//Save the application layout
	wxString str = this->_auiManager->SavePerspective();
	wxGetApp().Config()->Write(_T("AuiPerspective"), str);
	//Delete the toolbar manager (windows only)
	delete this->_toolbarManager;
}


void WCMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event) ) {
	wxMessageBox(_T("Wildcat CAD\nAuthor: Graham Hemingway\nhttp://code.google.com/p/wildcat-cad/\n\nContributors:\nDan Heeks\n\nBuilt with wxWidgets\nwww.wxwidgets.org"), _T("About Wildcat"));
}


void WCMainFrame::OnSize( wxSizeEvent& evt ) {
	//Set the main window size
	wxSize size = evt.GetSize();
	int width = size.GetWidth();
	int height = size.GetHeight();
	//Record app window size
	wxGetApp().Config()->Write(_T("MainFrameWidth"), width);
	wxGetApp().Config()->Write(_T("MainFrameHeight"), height);
	//Pass on the event
	evt.Skip();
}


void WCMainFrame::OnMove( wxMoveEvent& evt ) {
	//Move the window
	wxPoint pos = GetPosition();
	int posx = pos.x;
	int posy = pos.y;
	//Record the position
	wxGetApp().Config()->Write(_T("MainFramePosX"), posx);
	wxGetApp().Config()->Write(_T("MainFramePosY"), posy);
	//Pass on the event
	evt.Skip();
}


//Creates a canvas. Called from view.cpp when a new drawing
// view is created.
WCPartCanvas *WCMainFrame::CreateCanvas(wxView *view, wxMDIChildFrame *parent) {
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
	//Create the non-retained canvas
	WCPartCanvas *canvas = new WCPartCanvas(view, parent, graphics_attrib_list);
	canvas->SetCursor(wxCursor(wxCURSOR_PENCIL));
	//Return the new canvas
	return canvas;
}


wxToolBar* WCMainFrame::AddToolBar(const wxString& name) {
  	wxToolBar* newToolbar = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_NODIVIDER | wxTB_FLAT);
	newToolbar->SetToolBitmapSize(wxSize(32, 32));
	return newToolbar;
}


wxToolBarToolBase* WCMainFrame::AddToolBarTool(wxToolBar* toolbar, int tool_id, const wxString& title, wxBitmap& bitmap, const wxString& caption) {
	//Get the image
	wxImage image = bitmap.ConvertToImage();
	//Flip it to compensate for OSX craziness
	image = image.Mirror(false);
	//Add the tool to the toolbar
	wxToolBarToolBase* tool = toolbar->AddTool(tool_id, title, image, caption);
	//Insert into button map
	this->_idButtonMap.insert( std::make_pair(tool_id, tool) );
	//Return the new tool
	return tool;
}


void WCMainFrame::OnToolBarButton( wxCommandEvent& event ) {
	std::map<int, wxToolBarToolBase*>::iterator FindIt = this->_idButtonMap.find(event.GetId());
	if(FindIt != this->_idButtonMap.end()) {
		wxToolBarToolBase*tool = FindIt->second;
		if(tool->GetLabel() == _T("new")) {
			this->m_docManager->OnFileNew(event);
		}
		else if(wxGetApp().GetWCDocument()) {
			wxGetApp().GetWCDocument()->ActiveWorkbench()->OnUserMessage(std::string(tool->GetLabel().mb_str()));
		}
	}
}


void WCMainFrame::OnUpdateToolBarButton( wxUpdateUIEvent& event ) {
	//Do nothing for now
}


/***********************************************~***************************************************/

