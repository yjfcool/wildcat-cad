// part_canvas.cpp

#include "stdafx.h"
#include "part_canvas.h"
#include "part_document.h"
#include "Kernel/document.h"
#include "Kernel/workbench.h"

BEGIN_EVENT_TABLE(WCPartCanvas, wxGLCanvas)
    EVT_SIZE(WCPartCanvas::OnSize)
	EVT_ERASE_BACKGROUND(WCPartCanvas::OnEraseBackground)
    EVT_PAINT(WCPartCanvas::OnPaint)
    EVT_MOUSE_EVENTS(WCPartCanvas::OnMouseEvent)
END_EVENT_TABLE()

// Define a constructor for part canvas
WCPartCanvas::WCPartCanvas(wxView *v, wxMDIChildFrame *frame, int *attribList):
 wxGLCanvas(frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, L"some text", attribList)
{
	view = v;
	if (m_glContext)
	{
		m_glContext->SetCurrent(*this);
	}
 }

void WCPartCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    /* must always be here */
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();

	//Try drawing the document
    WCPartDocument *doc = (WCPartDocument *)(view->GetDocument());
	doc->Document()->ActiveWorkbench()->Render();

    SwapBuffers();
}

void WCPartCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

	//Set the window width and height parameters
	((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnReshape(event.GetSize().GetWidth(), event.GetSize().GetHeight());

	Refresh();
}

void WCPartCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
	// Do nothing, to avoid flashing on MSW
}

void WCPartCanvas::OnMouseEvent(wxMouseEvent& event)
{
	event.Skip();
}
