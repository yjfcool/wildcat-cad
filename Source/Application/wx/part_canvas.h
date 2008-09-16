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


#ifndef __PART_CANVAS_H__
#define __PART_CANVAS_H__


/*** Included Header Files ***/
#include "Utility/wutil.h"
#include <wx/docview.h>
#include <wx/glcanvas.h>


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefinitions ***/
class WCDocument;


/***********************************************~***************************************************/


class WCPartCanvas : public wxGLCanvas {
private:
    wxView										*_view;												//!< Associated wxView
	//wx Macros
    DECLARE_EVENT_TABLE()
public:
	//Constructors and Destructors
    WCPartCanvas(wxView *view, wxMDIChildFrame *frame, int *attribList=NULL);						//!< Primary constructor
	~WCPartCanvas()								{ }													//!< Default destructor

	//Access Methods
	inline wxView* View(void)					{ return this->_view; }								//!< Get the underlying view
	inline void View(wxView* view)				{ this->_view = view; }								//!< Set the underlying view

	//Event Handlers
	void OnPaint(wxPaintEvent& event);																//!<
    void OnSize(wxSizeEvent& event);																//!<
	void OnEraseBackground(wxEraseEvent& event);													//!<
    void OnMouse(wxMouseEvent& event);																//!<
	void OnKeyDown(wxKeyEvent& event);																//!<
	void OnKeyUp(wxKeyEvent& event);																//!<
	void OnIdle(wxIdleEvent& event);																//!<
	void OnDisplay(void);																			//!<
	WCDocument* GetWCDocument();																	//!<
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__PART_CANVAS_H__

