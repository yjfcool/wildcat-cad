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
#include "Application/Win32/document_view.h"
#include "Kernel/document.h"
#include "Kernel/workbench.h"
#include "Kernel/selection_mode.h"


/*** Locally Defined Macros ***/
#define KEY_UP(vk_code)							((GetAsyncKeyState(vk_code) & 0x8000) ? false : true)
#define KEY_DOWN(vk_code)						((GetAsyncKeyState(vk_code) & 0x8000) ? true : false)


/***********************************************~***************************************************/


WCDocumentView::WCDocumentView() : _document(NULL) {
//	SetTimer();
}


WCDocumentView::~WCDocumentView() {
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(this->m_hglRC);
}


BOOL WCDocumentView::PreCreateWindow(CREATESTRUCT& cs) {
	//Make sure the base window can pre-create
	if (!CWnd::PreCreateWindow(cs)) return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	//Set the window size
	cs.cx = 400;
	cs.cy = 400;
	//Set the window properties
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);
	//All is good
	return TRUE;
}


BOOL WCDocumentView::SetFormat(HDC hdc) {
	PIXELFORMATDESCRIPTOR *ppfd;
	int pixelformat;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),		// Size of this pfd
		1,									// Version number
		PFD_DRAW_TO_WINDOW |				// Support window
		PFD_SUPPORT_OPENGL |				// Support OpenGL
		PFD_GENERIC_FORMAT |				// 
		PFD_DOUBLEBUFFER |					// Support double buffer
		PFD_TYPE_RGBA,						// Support RGBA
		32,									// 32-bit color depth
		0, 0, 0, 0, 0, 0,					// Color bits ignored
		8,									// No alpha buffer
		0,									// Shift bit ignored
		8,									// No accum buffer
		0, 0, 0, 0,							// Accum bits ignored
		64,									// 64-bit depth buffer
		8,									// Stencil buffer
		8,									// Auxiliary buffer
		PFD_MAIN_PLANE,						// Main layer
		0,									// Reserved
		0, 0, 0								// Layer masks ignored
	};
	//Copy the pixel format
	ppfd = &pfd;

	//Try to choose a compatible pixel format
	if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0) {
		::MessageBox(NULL, _T("ChoosePixelFomat failed!"), _T("Error"), MB_OK);
		return FALSE;
	}
	//Now try to set that format
	if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) {
		::MessageBox(NULL, _T("SetPixelFomat failed!"), _T("Error"), MB_OK);
		return FALSE;
	}
	//All is good
	return TRUE;
}


void WCDocumentView::OnInitGL(void) {
	//glShadeModel(GL_SMOOTH);
	//glClearColor(0.0, 0.0, 0.0, 0.5);
	//glClearDepth(1.0);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	this->m_hgldc = ::GetDC(this->m_hWnd);
	//Check the pixel format
	if (!SetFormat(this->m_hgldc)) {
		::MessageBox(::GetFocus(), _T("SetPixelFormat Failed!"), _T("Error"), MB_OK);
		return;
	}
	this->m_hglRC = wglCreateContext(this->m_hgldc);
	BOOL retVal = wglMakeCurrent(this->m_hgldc, this->m_hglRC);
	if (retVal == FALSE) {
		::MessageBox(::GetFocus(), _T("MakeCurrent Failed!"), _T("Error"), MB_OK);
		return;
	}
}


void WCDocumentView::OnDisplay(void) {
	//Device context for painting
	CPaintDC dc(this);

	//Make sure we have context
	BOOL retVal = wglMakeCurrent(this->m_hgldc, this->m_hglRC);
	if (retVal == FALSE) {
		::MessageBox(::GetFocus(), _T("WCDocumentView::OnDisplay - MakeCurrent Failed!"), _T("Error"), MB_OK);
		return;
	}
	//Update the status bar if needed
	//...
	//Try drawing the document
	this->_document->ActiveWorkbench()->Render();
	//Swap the buffers
	SwapBuffers(this->m_hgldc);
}


void WCDocumentView::OnResize(const int &width, const int &height) {
	//Set the window width and height parameters
	this->_document->ActiveWorkbench()->OnReshape(width, height);
	//Render the doc if it is dirty
	if (this->_document->IsVisualDirty()) this->OnDisplay();
}


void WCDocumentView::OnIdle(void) {
	//Call idle method
	this->_document->ActiveWorkbench()->OnIdle();
	//Render the doc if it is dirty
//	if (this->_document->IsVisualDirty()) 
	this->OnDisplay();
}


void WCDocumentView::OnMouseMovement(const int &x, const int &y) {
	//Pass message on to document
	this->_document->ActiveWorkbench()->OnMouseMove(x, y);
	//Render the doc if it is dirty
	if (this->_document->IsVisualDirty()) this->OnDisplay();
}


bool WCDocumentView::OnKeyPress(UINT nChar, UINT nRepCnt, UINT nFlags) {
	char key = nChar;
	//Make lower case if not shifted
	if (KEY_UP(VK_SHIFT)) key = tolower(key);
	//See what type of key is pressed
	switch(key) {
		//Capture arrow keys
//		case NSUpArrowFunctionKey:		_document->Scene()->OnArrowKeyPress(WCArrowKey::Up()); return true; break;
//		case NSLeftArrowFunctionKey:	_document->Scene()->OnArrowKeyPress(WCArrowKey::Left()); return true; break;
//		case NSRightArrowFunctionKey:	_document->Scene()->OnArrowKeyPress(WCArrowKey::Right()); return true; break;
//		case NSDownArrowFunctionKey:	_document->Scene()->OnArrowKeyPress(WCArrowKey::Down()); return true; break;

		//Capture view keys
		case '1': _document->ActiveWorkbench()->NamedView("Front"); return true; break;
		case '2': _document->ActiveWorkbench()->NamedView("Back"); return true; break;
		case '3': _document->ActiveWorkbench()->NamedView("Left"); return true; break;
		case '4': _document->ActiveWorkbench()->NamedView("Right"); return true; break;
		case '5': _document->ActiveWorkbench()->NamedView("Top"); return true; break;
		case '6': _document->ActiveWorkbench()->NamedView("Bottom"); return true; break;
		case '7': _document->ActiveWorkbench()->NamedView("Isometric"); return true; break;
		case '8': _document->ActiveWorkbench()->ZoomToFit(); return true; break;
		case 'v': _document->ActiveWorkbench()->SaveAs("test.xml"); return true; break;
		//Capture escape
		case 96:
			//Revert to default drawing mode
//			_document->ActiveWorkbench()->DrawingMode( WCDrawingMode::Selection( _document->ActiveWorkbench() ));
			_document->ActiveWorkbench()->DrawingMode( new WCSelectionMode( _document->ActiveWorkbench() ) );
			 return true;
			break;
	}
	//Otherwise, did not capture
	return false;
}


void WCDocumentView::OnWindowWillClose(void) {
	std::cout << "Window will close\n";
}


/***********************************************~***************************************************/

