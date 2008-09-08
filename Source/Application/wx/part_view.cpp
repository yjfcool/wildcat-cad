// part_view.cpp

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
