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
#include "Application/Win32/vis_document.h"
#include "RTVisualization/visualization.h"
#include "RTVisualization/vis_feature.h"
#include "Application/keymap.h"


/*** Locally Defined Macros ***/
#define KEY_UP(vk_code)							((GetAsyncKeyState(vk_code) & 0x8000) ? false : true)
#define KEY_DOWN(vk_code)						((GetAsyncKeyState(vk_code) & 0x8000) ? true : false)


/***********************************************~***************************************************/


BEGIN_MESSAGE_MAP(WCVisDocument, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZING()
END_MESSAGE_MAP()


void WCVisDocument::OnPaint()  {
	//Call to super on display
	this->OnDisplay();
}


void WCVisDocument::OnDraw(CDC* pDC) {
	//Call to super on display
	this->OnDisplay();
}


int WCVisDocument::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	//Call to super OnCreate
	this->OnInitGL();
	InitOpenGL();
	//See what _filePath is
	if (true) {
		this->_visualization = new WCVisualization("", "");
		this->_document = this->_visualization;
		//Set the default surface renderer
//		WCPartFeature::DefaultSurfaceRenderer = this->_part->Scene()->ShaderManager()->ProgramFromName("scn_basiclight");
	}
	//Otherwise, open from file
	else {
	}
	return 0;
}


void WCVisDocument::OnSize(UINT nType, int cx, int cy) {
	//Call to super
	this->OnResize(cx, cy);
}


BOOL WCVisDocument::OnEraseBkgnd(CDC *pDC) {
	return TRUE;
}


void WCVisDocument::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	//Try to let super handle it
	if (this->OnKeyPress(nChar, nRepCnt, nFlags)) return;

	//Otherwise, get event key & modifiers
	bool control = KEY_DOWN(VK_CONTROL);
	bool option = false;
	bool alt = KEY_DOWN(VK_MENU);
	bool command = KEY_DOWN(VK_LWIN);
	bool esc = KEY_DOWN(VK_ESCAPE);
	char key = nChar;
	//Make lower case if not shifted
	if (KEY_UP(VK_SHIFT)) key = tolower(key);

	//Are there any that we want to handle here
	//Nothing for now...

	//Create the key event
	WCKeyEvent event(key, control, option, alt, command, esc);
	WCUserMessage message = this->_visualization->ActiveWorkbench()->KeyMap()->MessageFromKeyEvent( event );
	//If no message return false
	if (message == "") return;
	
	//Pass the message to the part
	this->_visualization->ActiveWorkbench()->OnUserMessage(message);
	//Check to see if is dirty
	if (this->_visualization->IsVisualDirty()) this->OnDisplay();
}


void WCVisDocument::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
	//Check for pan key
	if (KEY_UP(VK_LWIN) && this->_document->ActiveWorkbench()->IsPan())
		this->_document->ActiveWorkbench()->OnPanRelease();
	//Check for rotate key
	if (KEY_UP(VK_SHIFT) && this->_document->ActiveWorkbench()->IsRotate()) 
		this->_document->ActiveWorkbench()->OnRotateRelease();
	//Check for zoom key
	if (KEY_UP(VK_MENU) && this->_document->ActiveWorkbench()->IsZoom())
		this->_document->ActiveWorkbench()->OnZoomRelease();
	//Check for multi-select
	if (KEY_UP(VK_CONTROL) && this->_document->ActiveWorkbench()->IsMultiSelect())
		this->_document->ActiveWorkbench()->OnMultiSelectRelease();
}


void WCVisDocument::OnMouseMove(UINT nFlags, CPoint point) {
	//Pass mouse movement message on to super
	this->OnMouseMovement(point.x, point.y);
}


void WCVisDocument::OnSetFocus(CWnd* pOldWnd) {
	//Setup the idle loop timer
	this->_timer = SetTimer(1, 1, 0);
}


void WCVisDocument::OnKillFocus(CWnd* pNewWnd) {
	//Stop the idle loop timer
	KillTimer(this->_timer);
}


void WCVisDocument::OnTimer(UINT nIDEvent) {
	//Call to super onIdle
	this->OnIdle();
}


BOOL WCVisDocument::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	//Convert to normalized value
	float yDist = (float)zDelta / 60.0f;
	//Pass scrollWheel event on to the document
	this->_document->ActiveWorkbench()->OnScroll(0.0f, yDist);
	//Render the part if it's dirty
	if (this->_document->IsVisualDirty()) this->OnDisplay();
	return TRUE;
}


void WCVisDocument::OnLButtonDown(UINT nFlags, CPoint point) {
	//See if modifier keys are pressed or released
	if (KEY_DOWN(VK_LWIN)) this->_document->ActiveWorkbench()->OnPanPress();
	if (KEY_DOWN(VK_SHIFT)) this->_document->ActiveWorkbench()->OnRotatePress();
	if (KEY_DOWN(VK_MENU)) this->_document->ActiveWorkbench()->OnZoomPress();
	if (KEY_DOWN(VK_CONTROL)) this->_document->ActiveWorkbench()->OnMultiSelectPress();

	//Call to left button pressed
	this->_document->ActiveWorkbench()->OnMouseDown(WCMouseButton::Left());
	//Render the doc if it's dirty
	if (this->_document->IsVisualDirty()) this->OnDisplay();
}


void WCVisDocument::OnLButtonUp(UINT nFlags, CPoint point) {
	//Call to left button released
	this->_document->ActiveWorkbench()->OnMouseUp(WCMouseButton::Left());
	//Render the doc if it's dirty
	if (this->_document->IsVisualDirty()) this->OnDisplay();
}


void WCVisDocument::OnRButtonDown(UINT nFlags, CPoint point) {
	//Call to right button pressed
	this->_document->ActiveWorkbench()->OnMouseDown(WCMouseButton::Right());
	//Render the doc if it's dirty
	if (this->_document->IsVisualDirty()) this->OnDisplay();
}


void WCVisDocument::OnRButtonUp(UINT nFlags, CPoint point) {
	//Call to right button release
	this->_document->ActiveWorkbench()->OnMouseUp(WCMouseButton::Right());
	//Render the doc if it's dirty
	if (this->_document->IsVisualDirty()) this->OnDisplay();
}


void WCVisDocument::OnSizing(UINT nSide, LPRECT lpRect) {
	//Determine width and height
	int width = lpRect->right - lpRect->left;
	int height = lpRect->top - lpRect->bottom;
	//Call to super
	this->OnResize(width, height);
}


/***********************************************~***************************************************/


WCVisDocument::WCVisDocument() : ::WCDocumentView(), _visualization(NULL), _timer(0) {
	//Nothing else for now
}


WCVisDocument::~WCVisDocument() {
	//Delete part if it exists
	if (this->_visualization != NULL) delete this->_visualization;
	//Might need to kill timer
	//KillTimer(this->_timer);
}


/***********************************************~***************************************************/

