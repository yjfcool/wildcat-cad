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
#include "Application/wx/part_view.h"
#include "Application/wx/part_document.h"
#include "Application/wx/wildcat_app.h"
#include "Application/wx/main_frame.h"
#include "Application/wx/part_canvas.h"


/***********************************************~***************************************************/ 


IMPLEMENT_DYNAMIC_CLASS(WCPartView, wxView)
BEGIN_EVENT_TABLE(WCPartView, wxView)
END_EVENT_TABLE()


/***********************************************~***************************************************/ 


// What to do when a view is created. Creates actual
// windows for displaying the view.
bool WCPartView::OnCreate(wxDocument *doc, long WXUNUSED(flags) ) {
	this->_frame = wxGetApp().CreateChildFrame(doc, this, true);
    this->_frame->SetTitle(_T("WCPartView"));
    this->_canvas = wxGetApp().Frame()->CreateCanvas(this, this->_frame);
#ifdef __X__
    // X seems to require a forced resize
    int x, y;
    this->_frame->GetSize(&x, &y);
    this->_frame->SetSize(wxDefaultCoord, wxDefaultCoord, x, y);
#endif
#ifdef __WXMSW__
	this->_frame->SetIcon(wxICON(Wildcat));
#endif

    this->_frame->Show(true);
	this->_canvas->Show(true);
    Activate(true);
	//Always return true
    return true;
}


// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void WCPartView::OnDraw(wxDC *dc) {
}


void WCPartView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint)) {
	//If the canvas is present, refresh it
	if (this->_canvas) this->_canvas->Refresh();
}


// Clean up windows used for displaying the view.
bool WCPartView::OnClose(bool deleteWindow) {
	//If doc won't close, then don't close
	if (!this->GetDocument()->Close()) return false;
	// Clear the canvas in  case we're in single-window mode,
	// and the canvas stays.
	this->_canvas->ClearBackground();
	this->_canvas->View(NULL);
	this->_canvas = NULL;
	wxString s(wxTheApp->GetAppName());
	if (this->_frame) this->_frame->SetTitle(s);
	SetFrame((wxFrame*)NULL);
	Activate(false);
	//If we are deleting the window
	if (deleteWindow) {
		//Delete the frame
		delete this->_frame;
	}
	//We outta here
	return true;
}


/***********************************************~***************************************************/

