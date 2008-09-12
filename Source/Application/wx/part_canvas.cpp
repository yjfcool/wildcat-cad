// part_canvas.cpp

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

#include "stdafx.h"
#include "part_canvas.h"
#include "part_document.h"
#include "Kernel/document.h"
#include "Kernel/workbench.h"
#include "Kernel/selection_mode.h"
#include "Application/keymap.h"


BEGIN_EVENT_TABLE(WCPartCanvas, wxGLCanvas)
    EVT_SIZE(WCPartCanvas::OnSize)
	EVT_ERASE_BACKGROUND(WCPartCanvas::OnEraseBackground)
    EVT_PAINT(WCPartCanvas::OnPaint)
    EVT_MOUSE_EVENTS(WCPartCanvas::OnMouse)
	EVT_KEY_DOWN(WCPartCanvas::OnKeyDown)
	EVT_IDLE(WCPartCanvas::OnIdle)
END_EVENT_TABLE()

// Define a constructor for part canvas
WCPartCanvas::WCPartCanvas(wxView *v, wxMDIChildFrame *frame, int *attribList):
wxGLCanvas(frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, L"some text", attribList)
{
	view = v;
	if (m_glContext)
	{
		m_glContext->SetCurrent(*this);
	}
}

WCDocument* WCPartCanvas::GetWCDocument() {
	return view ? (((WCPartDocument *)(view->GetDocument()))->Document()) : NULL;
}

void WCPartCanvas::OnDisplay(void) {
    /* must always be here */
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();

	//Try drawing the document
	if(GetWCDocument())GetWCDocument()->ActiveWorkbench()->Render();

    SwapBuffers();
}

void WCPartCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	OnDisplay();
}

void WCPartCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

	//Set the window width and height parameters
	GetWCDocument()->ActiveWorkbench()->OnReshape(event.GetSize().GetWidth(), event.GetSize().GetHeight());

	//Render the doc if it is dirty
	if (GetWCDocument()->IsVisualDirty()) this->Refresh();
}

void WCPartCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
	// Do nothing, to avoid flashing on MSW
}

void WCPartCanvas::OnMouse( wxMouseEvent& event )
{
	if(event.Entering()){
		SetFocus(); // so middle wheel works
	}

	else if(event.Moving() || event.Dragging())
	{
		//Pass message on to document
		GetWCDocument()->ActiveWorkbench()->OnMouseMove(event.GetX(), event.GetY());
	}

	else if(event.GetWheelRotation() != 0)
	{
		//Convert to normalized value
		float yDist = (float)(event.GetWheelRotation()) / 60.0f;
		//Pass scrollWheel event on to the document
		GetWCDocument()->ActiveWorkbench()->OnScroll(0.0f, yDist);
		//Render the part if it's dirty
		if (GetWCDocument()->IsVisualDirty()) this->Refresh();
	}

	else if(event.LeftDown())
	{
		//See if modifier keys are pressed or released
		if (event.ControlDown()) GetWCDocument()->ActiveWorkbench()->OnPanPress();
		if (event.ShiftDown()) GetWCDocument()->ActiveWorkbench()->OnRotatePress();
		if (event.AltDown()) GetWCDocument()->ActiveWorkbench()->OnZoomPress();
		//if (event.ControlDown()) GetWCDocument()->ActiveWorkbench()->OnMultiSelectPress();

		//Call to left button pressed
		GetWCDocument()->ActiveWorkbench()->OnMouseDown(WCMouseButton::Left());
		//Render the doc if it's dirty
		if (GetWCDocument()->IsVisualDirty()) this->Refresh();
	}
	else if(event.LeftUp())
	{
		//See if modifier keys are pressed or released
		if (!event.ControlDown()) GetWCDocument()->ActiveWorkbench()->OnPanRelease();
		if (!event.ShiftDown()) GetWCDocument()->ActiveWorkbench()->OnRotateRelease();
		if (!event.AltDown()) GetWCDocument()->ActiveWorkbench()->OnZoomRelease();
		//if (!event.ControlDown()) GetWCDocument()->ActiveWorkbench()->OnMultiSelectRelease();

		//Call to left button released
		GetWCDocument()->ActiveWorkbench()->OnMouseUp(WCMouseButton::Left());
		//Render the doc if it's dirty
		if (GetWCDocument()->IsVisualDirty()) this->Refresh();
	}
	else if(event.RightDown())
	{
		//Call to right button pressed
		GetWCDocument()->ActiveWorkbench()->OnMouseDown(WCMouseButton::Right());
		//Render the doc if it's dirty
		if (GetWCDocument()->IsVisualDirty()) this->Refresh();
	}
	else if(event.RightUp())
	{
		//Call to right button release
		GetWCDocument()->ActiveWorkbench()->OnMouseUp(WCMouseButton::Right());
		//Render the doc if it's dirty
		if (GetWCDocument()->IsVisualDirty()) this->Refresh();
	}

	event.Skip();
}

void WCPartCanvas::OnKeyDown(wxKeyEvent& event)
{
	WCWorkbench* wb = GetWCDocument()->ActiveWorkbench();

	//See what type of key is pressed
	switch(event.GetKeyCode()) {
		//Capture view keys
		case '1': wb->NamedView("Front"); event.Skip(); return;
		case '2': wb->NamedView("Back"); event.Skip(); return;
		case '3': wb->NamedView("Left"); event.Skip(); return;
		case '4': wb->NamedView("Right"); event.Skip(); return;
		case '5': wb->NamedView("Top"); event.Skip(); return;
		case '6': wb->NamedView("Bottom"); event.Skip(); return;
		case '7': wb->NamedView("Isometric"); event.Skip(); return;
		case '8': wb->ZoomToFit(); event.Skip(); return;
		case 'v': wb->SaveAs("test.xml"); event.Skip(); return;
		// escape key
		case WXK_ESCAPE: wb->DrawingMode( new WCSelectionMode( wb ) ); event.Skip();return;
	}

	//Otherwise, get event key & modifiers
	bool control = event.ControlDown();
	bool option = false;
	bool alt = event.AltDown();
	bool command = event.CmdDown();
	bool esc = ::wxGetKeyState(WXK_ESCAPE);
	char key = event.GetKeyCode();
	//Make lower case if not shifted
	if (!event.ShiftDown()) key = tolower(key);

	//Create the key event
	WCKeyEvent e(key, control, option, alt, command, esc);
	WCUserMessage message = wb->KeyMap()->MessageFromKeyEvent( e );
	//If no message return false
	if (message == ""){event.Skip(); return;}
	
	//Pass the message to the part
	wb->OnUserMessage(message);
	//Check to see if is dirty
	if (GetWCDocument()->IsVisualDirty()) this->Refresh();

	event.Skip();
}

void WCPartCanvas::OnKeyUp(wxKeyEvent& event)
{
	WCWorkbench* wb = GetWCDocument()->ActiveWorkbench();

	switch(event.GetKeyCode()){
		//Check for pan key
		case WXK_WINDOWS_LEFT:
			if (wb->IsPan())wb->OnPanRelease();
			break;
		//Check for rotate key
		case WXK_SHIFT:
			if (wb->IsRotate())wb->OnRotateRelease();
			break;
		//Check for zoom key
		case WXK_MENU:
			if (wb->IsZoom())wb->OnZoomRelease();
			break;
		//Check for multi-select
		case WXK_CONTROL:
			if (wb->IsMultiSelect())wb->OnMultiSelectRelease();
			break;
	}

	event.Skip();
}

void WCPartCanvas::OnIdle(wxIdleEvent& event) {
	if(GetWCDocument()) {
		//Call idle method
		GetWCDocument()->ActiveWorkbench()->OnIdle();

		//Render the doc if it is dirty
		if (GetWCDocument()->IsVisualDirty()) 
			this->Refresh();
	}
}
