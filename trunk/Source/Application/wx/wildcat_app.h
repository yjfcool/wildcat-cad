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


#ifndef __WILDCAT_APP__
#define __WILDCAT_APP__


/*** Included Header Files ***/
#include <Utility/wutil.h>
#include <wx/config.h>


/*** Some wx Stuff ***/
class wxDocManager;
class wxDocument;
class wxView;


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefinitions ***/

class WCMainFrame;
class WCDocument;


/***********************************************~***************************************************/


class WCWildcatApp : public wxApp {
protected:
	wxDocManager								*_docManager;										//!<
	WCMainFrame									*_frame;											//!<
	wxConfig									*_config;											//!<
public:
	//Constructors and Destructors
	WCWildcatApp(void);																				//!< Default constructor
	~WCWildcatApp(void);																			//!< Default destructor

	//Member Access Methods
	inline wxConfig* Config(void)				{ return this->_config; }							//!< Return application configuration
	inline WCMainFrame* Frame(void)				{ return this->_frame; }							//!< Return application frame
	inline wxDocManager* DocManager(void)		{ return this->_docManager; }						//!< Return application document manager

	bool OnInit(void);
	int OnExit(void);

	wxMDIChildFrame *CreateChildFrame(wxDocument *doc, wxView *view, bool isCanvas);
	wxString GetExeFolder()const;
	WCDocument* GetWCDocument();
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace


//Global Access Function
DECLARE_APP(WCWildcatApp)


#endif //__WILDCAT_APP__

