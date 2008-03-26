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


/*** Included Header Files ***/
#include "Application/Win32/part_document.h"
#include "PartDesign/part.h"
#include "PartDesign/part_feature.h"
#include "Application/keymap.h"


/***********************************************~***************************************************/


BEGIN_MESSAGE_MAP(WCPartDocument, CWnd)
//	ON_WM_PAINT()
	ON_WM_CREATE()
//	ON_WM_SIZE()
//	ON_WM_ERASEBKGND()
//	ON_WM_KEYDOWN()
//	ON_WM_MOUSEMOVE()
//	ON_WM_SETFOCUS()
//	ON_WM_KILLFOCUS()
//	ON_WM_KEYUP()
//	ON_WM_TIMER()
//	ON_WM_MOUSEWHEEL()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONUP()
//	ON_WM_RBUTTONDOWN()
//	ON_WM_RBUTTONUP()
//	ON_WM_SIZING()
END_MESSAGE_MAP()


/*
void WCPartDocument::OnPaint()  {
	// device context for painting
	CPaintDC dc(this);
	this->OnDisplay();
	SwapBuffers(this->m_hgldc);
}
*/

int WCPartDocument::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	//Call to super OnCreate
	this->OnInitGL();
	InitOpenGL();
	//See what _filePath is
	if (true) {
		this->_part = new WCPart("", "");
		this->_document = this->_part;
		//Set the default surface renderer
		WCPartFeature::DefaultSurfaceRenderer = this->_part->Scene()->ShaderManager()->ProgramFromName("scn_basiclight");
	}
	//Otherwise, open from file
	else {
	}
	return 0;
}

/*
void WCPartDocument::OnSize(UINT nType, int cx, int cy) {
	this->OnResize(cx, cy);
}


BOOL WCPartDocument::OnEraseBkgnd(CDC *pDC) {
	return TRUE;
}


void WCPartDocument::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	switch(nChar) {
		case VK_ESCAPE:
			exit(0);
			break;
		default:
			this->OnKeyPress();
			break;
	}
}


void WCPartDocument::OnMouseMove(UINT nFlags, CPoint point) {
	this->OnMouseMove();
}


/***********************************************~***************************************************/


WCPartDocument::WCPartDocument() : ::WCDocumentView(), _part(NULL) {
//	SetTimer();
}


WCPartDocument::~WCPartDocument() {
}


BOOL WCPartDocument::PreCreateWindow(CREATESTRUCT& cs) {
	return ::WCDocumentView::PreCreateWindow(cs);
}


/***********************************************~***************************************************/

