// part_view.h

#ifndef WC_WX_PART_VIEW_HEADER
#define WC_WX_PART_VIEW_HEADER

#include "wx/docview.h"

class WCPartCanvas;

class WCPartView: public wxView
{
public:
    wxMDIChildFrame *frame;
    WCPartCanvas *canvas;
  
    WCPartView() { canvas = (WCPartCanvas *) NULL; frame = (wxMDIChildFrame *) NULL; }
    ~WCPartView() {}

    bool OnCreate(wxDocument *doc, long flags);
    void OnDraw(wxDC *dc);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    bool OnClose(bool deleteWindow = true);

private:
    DECLARE_DYNAMIC_CLASS(WCPartView)
    DECLARE_EVENT_TABLE()
};



#endif