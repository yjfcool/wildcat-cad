/*** Imported Header Files ***/
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


#import "Application/MacOS/document.h"
#import "Application/MacOS/render_window.h"


/*** Included Header Files ***/
#include "Kernel/document.h"
#include "Kernel/workbench.h"
#include "Kernel/selection_mode.h"


/***********************************************~***************************************************/


@implementation WCDocument_OSX


- (id)init
{
	//Do the cocoa thing
    self = [super init];
	//If super was created successfully
    if (self) {
		//Initialize remaining variables
		_renderWindow = nil;
		_document = NULL;
		//Do not create an OSX undo manager
		[self setHasUndoManager:NO];
    }
	//Return reference to self
    return self;
}


- (void)close
{
	//Do Cocoa thing and pass to super
	[super close];
	//Close and release render window
	[_renderWindow release];
}


- (void)makeWindowControllers {
	//Create a render window.
	_renderWindow = [[WCRenderWindow alloc] initWithDocument:self];
	//Add as window controller
	[self addWindowController:_renderWindow];
}


- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{
	NSLog(@"WCDocument_OSX::dataOfType - Should not be here.");
    return nil;
}


//- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
//{
//	NSLog(@"WCDocument_OSX::dataOfType - Should not be here.");
//    return YES;
//}


- (void)onInitOpenGL
{
	//Make renderView OpenGL context active
	NSOpenGLContext *context = [[_renderWindow renderView] openGLContext];
	[context makeCurrentContext];
	//That's all...
}


- (void)onBecomeKey
{
	//Establish timer to handle idle loop
	NSInvocation *invoker = [NSInvocation invocationWithMethodSignature:[self methodSignatureForSelector:@selector(onIdle)]];
	[invoker setSelector:@selector(onIdle)];
	[invoker setTarget:self];
	_timer = [NSTimer scheduledTimerWithTimeInterval:0.0001 invocation:invoker repeats:YES];
	//Retain timer
	[_timer retain];
}


- (void)onResignKey
{
	//Invalidate the current timer and release
//	[_timer invalidate];
//	[_timer release];

	//Pass to active workbench
	//...
}


- (void)onDisplay
{
	NSOpenGLContext *context = [[_renderWindow renderView] openGLContext];
	//Make the context active
	[context makeCurrentContext];
	//Draw the part
	_document->ActiveWorkbench()->Render();
	//Flush the context
	[context flushBuffer];
	
	//Set the status text in the render window
	std::string statusText = "Status: " + _document->Status();
	const char *text = statusText.c_str();
	//Convert std::string to NSString
	NSString *newStatus = [[NSString alloc] initWithCString:text];
	NSString *oldStatus = [[_renderWindow statusText] objectValue];
	//Compare the strings...
	if (NSOrderedSame != [newStatus compare:oldStatus]) {
		//Set renderWindow status text
		[[_renderWindow statusText] setObjectValue:newStatus];
	}
	//Release the string
	[newStatus release];
}
 
 
- (void)onResize:(NSSize)size
{
	//Get the new renderView width and height
	int width = (int)size.width;
	int height = (int)size.height;

	//Set the window width and height parameters
	_document->ActiveWorkbench()->OnReshape(width, height);
	//Render the doc if it's dirty
	if (_document->IsVisualDirty())	[self onDisplay];
}


- (void)onIdle
{
	//Call Idle method
	_document->ActiveWorkbench()->OnIdle();
	//Render the doc if it's dirty
	if (_document->IsVisualDirty()) [self onDisplay];
}


- (void)onMousePress:(NSEvent*)theEvent
{
	//See if modifier keys are pressed
	if ([theEvent modifierFlags] & NSControlKeyMask)
		_document->ActiveWorkbench()->OnPanPress();
	else if ([theEvent modifierFlags] & NSShiftKeyMask)
		_document->ActiveWorkbench()->OnRotatePress();
	else if ([theEvent modifierFlags] & NSAlternateKeyMask)
		_document->ActiveWorkbench()->OnZoomPress();
	else if ([theEvent modifierFlags] & NSCommandKeyMask)
		_document->ActiveWorkbench()->OnMultiSelectPress();

	//See if left button pressed
	if ([theEvent buttonNumber] == 0) {
//		NSLog(@"Button Zero");
		_document->ActiveWorkbench()->OnMouseDown(WCMouseButton::Left());
	}
	//Middle button
	else if ([theEvent buttonNumber] > 1) {
//		NSLog(@"Button One");
		_document->ActiveWorkbench()->OnMouseDown(WCMouseButton::Middle());
	}
	//Right button
	if ([theEvent buttonNumber] == 1) {
//		NSLog(@"Button Two");
		_document->ActiveWorkbench()->OnMouseDown(WCMouseButton::Right());
	}
	//Render the doc if it's dirty
	if (_document->IsVisualDirty()) [self onDisplay];
}


- (void)onMouseRelease:(NSEvent*)theEvent
{
	//Clear modifiers if necessary
	if (_document->ActiveWorkbench()->IsPan()) _document->ActiveWorkbench()->OnPanRelease();
	if (_document->ActiveWorkbench()->IsRotate()) _document->ActiveWorkbench()->OnRotateRelease();
	if (_document->ActiveWorkbench()->IsZoom()) _document->ActiveWorkbench()->OnZoomRelease();
	if (_document->ActiveWorkbench()->IsMultiSelect()) _document->ActiveWorkbench()->OnMultiSelectRelease();
	
	//See if left button pressed
	if ([theEvent buttonNumber] == 0) {
//		NSLog(@"Button Zero");
		_document->ActiveWorkbench()->OnMouseUp(WCMouseButton::Left());
	}
	//Middle button
	else if ([theEvent buttonNumber] == 1) {
//		NSLog(@"Button One");
		_document->ActiveWorkbench()->OnMouseUp(WCMouseButton::Middle());
	}
	//Right button
	if ([theEvent buttonNumber] == 2) {
//		NSLog(@"Button Two");
		_document->ActiveWorkbench()->OnMouseUp(WCMouseButton::Right());
	}
	//Render the doc if it's dirty
	if (_document->IsVisualDirty()) [self onDisplay];
}


- (void)onScrollWheel:(NSEvent *)theEvent
{
	//Pass scrollWheel event on to the document
	_document->ActiveWorkbench()->OnScroll((WPFloat)[theEvent deltaX], (WPFloat)[theEvent deltaY]);
	//Render the part if it's dirty
	if (_document->IsVisualDirty()) [self onDisplay];
}


- (void)onMouseMoveXPosition:(int)x withYPosition:(int)y
{
	//React to a mouse movement
	_document->ActiveWorkbench()->OnMouseMove(x, y);
	//Render the part if it's dirty
	if (_document->IsVisualDirty()) [self onDisplay];
}


- (bool)onKeyPress:(NSEvent*)theEvent
{
	//Get the pressed keys (without any modifiers)
	NSString *keys = [theEvent charactersIgnoringModifiers];
	switch([keys characterAtIndex:0]) {
		//Capture arrow keys
		case NSUpArrowFunctionKey:		_document->Scene()->OnArrowKeyPress(WCArrowKey::Up()); return true; break;
		case NSLeftArrowFunctionKey:	_document->Scene()->OnArrowKeyPress(WCArrowKey::Left()); return true; break;
		case NSRightArrowFunctionKey:	_document->Scene()->OnArrowKeyPress(WCArrowKey::Right()); return true; break;
		case NSDownArrowFunctionKey:	_document->Scene()->OnArrowKeyPress(WCArrowKey::Down()); return true; break;

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


- (void)onWindowWillClose
{
}


- (WCRenderWindow*)renderWindow
{
	//Return the render window
	return _renderWindow;
}


- (void)document:(WCDocument*)document
{
	//Set the document
	_document = document;
}


- (WCDocument*)document
{
	//Return the associated wildcat document
	return _document;

}


@end


/***********************************************~***************************************************/

