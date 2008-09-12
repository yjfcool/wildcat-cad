// part_view.cpp

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


#include "stdafx.h"
#include "part_view.h"
#include "part_document.h"
#include "wildcat_app.h"
#include "main_frame.h"
#include "part_canvas.h"

IMPLEMENT_DYNAMIC_CLASS(WCPartView, wxView)

BEGIN_EVENT_TABLE(WCPartView, wxView)
END_EVENT_TABLE()

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool WCPartView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
{
    frame = wxGetApp().CreateChildFrame(doc, this, true);
    frame->SetTitle(_T("WCPartView"));

    canvas = wxGetApp().m_frame->CreateCanvas(this, frame);
#ifdef __X__
    // X seems to require a forced resize
    int x, y;
    frame->GetSize(&x, &y);
    frame->SetSize(wxDefaultCoord, wxDefaultCoord, x, y);
#endif

#ifdef __WXMSW__
	frame->SetIcon(wxICON(Wildcat));
#endif

    frame->Show(true);
	canvas->Show(true);
    Activate(true);

    return true;
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void WCPartView::OnDraw(wxDC *dc)
{
}

void WCPartView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
  if (canvas)
    canvas->Refresh();
}

// Clean up windows used for displaying the view.
bool WCPartView::OnClose(bool deleteWindow)
{
  if (!GetDocument()->Close())
    return false;

  // Clear the canvas in  case we're in single-window mode,
  // and the canvas stays.
  canvas->ClearBackground();
  canvas->view = (wxView *) NULL;
  canvas = (WCPartCanvas *) NULL;

  wxString s(wxTheApp->GetAppName());
  if (frame)
    frame->SetTitle(s);

  SetFrame((wxFrame*)NULL);

  Activate(false);

  if (deleteWindow)
  {
    delete frame;
    return true;
  }
  return true;
}
