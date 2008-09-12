// wildcat_app.h

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

#ifndef WX_WILDCAT_APP_HEADER
#define WX_WILDCAT_APP_HEADER

class wxDocManager;
class WCMainFrame;
class wxDocument;
class wxView;
#include "Workbenches/Kernel/document.h"
#include <wx/config.h>

// Define a new application
class WCWildcatApp: public wxApp
{
public:
	WCMainFrame *m_frame;
	wxConfig* m_config;

	WCWildcatApp(void);
	~WCWildcatApp(void);
	bool OnInit(void);
	int OnExit(void);

	wxMDIChildFrame *CreateChildFrame(wxDocument *doc, wxView *view, bool isCanvas);
	wxString GetExeFolder()const;
	WCDocument* GetWCDocument();

protected:
	wxDocManager* m_docManager;
};

DECLARE_APP(WCWildcatApp)

#endif