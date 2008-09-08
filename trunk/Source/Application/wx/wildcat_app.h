// wildcat_app.h

#ifndef WX_WILDCAT_APP_HEADER
#define WX_WILDCAT_APP_HEADER

class wxDocManager;
class WCMainFrame;
class wxDocument;
class wxView;

// Define a new application
class WCWildcatApp: public wxApp
{
  public:
	  WCMainFrame *m_frame;

    WCWildcatApp(void);
    bool OnInit(void);
    int OnExit(void);

    wxMDIChildFrame *CreateChildFrame(wxDocument *doc, wxView *view, bool isCanvas);

  protected:
    wxDocManager* m_docManager;
};

DECLARE_APP(WCWildcatApp)

#endif