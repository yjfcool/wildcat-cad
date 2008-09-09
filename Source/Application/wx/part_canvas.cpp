// part_canvas.cpp

#include "stdafx.h"
#include "part_canvas.h"
#include "part_document.h"
#include "Kernel/document.h"
#include "Kernel/workbench.h"
#include "Kernel/selection_mode.h"
#include "Application/keymap.h"


/*** Locally Defined Macros ***/
#define KEY_UP(vk_code)							(::wxGetKeyState(vk_code))
#define KEY_DOWN(vk_code)						(!(::wxGetKeyState(vk_code)))


/***********************************************~***************************************************/


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

void WCPartCanvas::OnDisplay(void) {
    /* must always be here */
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();

	//Try drawing the document
    WCPartDocument *doc = (WCPartDocument *)(view->GetDocument());
	doc->Document()->ActiveWorkbench()->Render();

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
	((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnReshape(event.GetSize().GetWidth(), event.GetSize().GetHeight());

	//Render the doc if it is dirty
	if (((WCPartDocument*)(view->GetDocument()))->Document()->IsVisualDirty()) this->Refresh();
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
		((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnMouseMove(event.GetX(), event.GetY());
	}

	else if(event.GetWheelRotation() != 0)
	{
		//Convert to normalized value
		float yDist = (float)(event.GetWheelRotation()) / 60.0f;
		//Pass scrollWheel event on to the document
		((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnScroll(0.0f, yDist);
		//Render the part if it's dirty
		if (((WCPartDocument*)(view->GetDocument()))->Document()->IsVisualDirty()) this->Refresh();
	}

	else if(event.LeftDown())
	{
		//See if modifier keys are pressed or released
		if (KEY_DOWN(WXK_WINDOWS_LEFT)) ((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnPanPress();
		if (KEY_DOWN(WXK_SHIFT)) ((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnRotatePress();
		if (KEY_DOWN(WXK_WINDOWS_MENU)) ((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnZoomPress();
		if (KEY_DOWN(WXK_CONTROL)) ((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnMultiSelectPress();

		//Call to left button pressed
		((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnMouseDown(WCMouseButton::Left());
		//Render the doc if it's dirty
		if (((WCPartDocument*)(view->GetDocument()))->Document()->IsVisualDirty()) this->Refresh();
	}
	else if(event.LeftUp())
	{
		//Call to left button released
		((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnMouseUp(WCMouseButton::Left());
		//Render the doc if it's dirty
		if (((WCPartDocument*)(view->GetDocument()))->Document()->IsVisualDirty()) this->Refresh();
	}
	else if(event.RightDown())
	{
		//Call to right button pressed
		((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnMouseDown(WCMouseButton::Right());
		//Render the doc if it's dirty
		if (((WCPartDocument*)(view->GetDocument()))->Document()->IsVisualDirty()) this->Refresh();
	}
	else if(event.RightUp())
	{
		//Call to right button release
		((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnMouseUp(WCMouseButton::Right());
		//Render the doc if it's dirty
		if (((WCPartDocument*)(view->GetDocument()))->Document()->IsVisualDirty()) this->Refresh();
	}

	event.Skip();
}

void WCPartCanvas::OnKeyDown(wxKeyEvent& event)
{
	WCWorkbench* wb = ((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench();

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
	bool control = KEY_DOWN(WXK_CONTROL);
	bool option = false;
	bool alt = KEY_DOWN(WXK_WINDOWS_MENU);
	bool command = KEY_DOWN(WXK_WINDOWS_LEFT);
	bool esc = KEY_DOWN(WXK_ESCAPE);
	char key = event.GetKeyCode();
	//Make lower case if not shifted
	if (KEY_UP(WXK_SHIFT)) key = tolower(key);

	//Create the key event
	WCKeyEvent e(key, control, option, alt, command, esc);
	WCUserMessage message = wb->KeyMap()->MessageFromKeyEvent( e );
	//If no message return false
	if (message == ""){event.Skip(); return;}
	
	//Pass the message to the part
	wb->OnUserMessage(message);
	//Check to see if is dirty
	if (((WCPartDocument*)(view->GetDocument()))->Document()->IsVisualDirty()) this->Refresh();

	event.Skip();
}

void WCPartCanvas::OnKeyUp(wxKeyEvent& event)
{
	WCWorkbench* wb = ((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench();

	//Check for pan key
	if (KEY_UP(WXK_WINDOWS_LEFT) && wb->IsPan())
		wb->OnPanRelease();
	//Check for rotate key
	if (KEY_UP(WXK_SHIFT) && wb->IsRotate()) 
		wb->OnRotateRelease();
	//Check for zoom key
	if (KEY_UP(WXK_WINDOWS_MENU) && wb->IsZoom())
		wb->OnZoomRelease();
	//Check for multi-select
	if (KEY_UP(WXK_CONTROL) && wb->IsMultiSelect())
		wb->OnMultiSelectRelease();

	event.Skip();
}

void WCPartCanvas::OnIdle(wxIdleEvent& event) {
	//Call idle method
	((WCPartDocument*)(view->GetDocument()))->Document()->ActiveWorkbench()->OnIdle();

	//Render the doc if it is dirty
	if (((WCPartDocument*)(view->GetDocument()))->Document()->IsVisualDirty()) 
	this->Refresh();

	event.Skip();
}
