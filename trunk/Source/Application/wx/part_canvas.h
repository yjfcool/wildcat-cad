// part_canvas.h

#ifndef WC_WX_PART_CANVAS_HEADER
#define WC_WX_PART_CANVAS_HEADER

#include "wx/docview.h"
#include <wx/glcanvas.h>

class WCPartCanvas: public wxGLCanvas
{
public:
    wxView *view;
    
    WCPartCanvas(wxView *v, wxMDIChildFrame *frame, int *attribList = (int*) NULL);
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
    void OnMouseEvent(wxMouseEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

#endif