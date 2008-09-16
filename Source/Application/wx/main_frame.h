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


#ifndef __MAIN_FRAME_H__
#define __MAIN_FRAME_H__


/*** Included Header Files ***/
#include "Utility/wutil.h"
#include <wx/mdi.h>
#include <wx/docview.h>
#include <wx/docmdi.h>
#include <wx/aui/aui.h>
#include "Application/toolbar_manager.h"


/*** Locally Define Values ***/
#define MAX_TOOLS 10000
#define DOCVIEW_CUT     1
#define DOCVIEW_ABOUT   wxID_ABOUT
enum{
	FIRST_TOOLBAR_BUTTON_ID = wxID_HIGHEST + 1,
	NEXT_ID_AFTER_TOOLBAR_BUTTONS = FIRST_TOOLBAR_BUTTON_ID + MAX_TOOLS
};


/*** Externally Defined Variables ***/
extern bool singleWindowMode;


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefinitions ***/
class WCPartCanvas;


/***********************************************~***************************************************/


class WCMainFrame : public wxDocMDIParentFrame {
public:
	wxMenu										*_editMenu;											//!< Edit menu object
	wxAuiManager								*_auiManager;										//!< wxAuiManager
	std::map<int, wxToolBarToolBase*>			_idButtonMap;										//!< Map of all buttons
	WCToolbarManager							*_toolbarManager;									//!< Toolbar manager
public:
	//Constructors and Destructors
	WCMainFrame(wxDocManager *manager, wxFrame *frame, const wxString& title, const wxPoint& pos,	//!< Primary constructor
												const wxSize& size, long type);
	~WCMainFrame();																					//!< Default destructor

	//Primary Access Methods
	inline wxAuiManager* AuiManager(void)		{ return this->_auiManager; }						//!< Get the AuiManager
	inline WCToolbarManager* ToolbarManager(void){ return this->_toolbarManager; }					//!< Get the toolbar manager

	//Event Handlers
	void OnAbout(wxCommandEvent& event);															//!< On about event request
	void OnSize(wxSizeEvent& evt);																	//!< On resize of main window
	void OnMove(wxMoveEvent& evt);																	//!< On main window move
	void OnToolBarButton( wxCommandEvent& event );													//!< On toolbar button press
	void OnUpdateToolBarButton( wxUpdateUIEvent& event );											//!< On update of toolbar button

	//Primary Class Methods
	WCPartCanvas *CreateCanvas(wxView *view, wxMDIChildFrame *parent);								//!<
	wxToolBar* AddToolBar(const wxString& name);													//!<
	wxToolBarToolBase* AddToolBarTool(wxToolBar* toolbar, int tool_id, const wxString& title,		//!<
												wxBitmap& bitmap, const wxString& caption);

	//wx Macro
	DECLARE_EVENT_TABLE()
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__MAIN_FRAME_H__

