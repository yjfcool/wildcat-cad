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


/*** Imported Header Files ***/
#import <Kernel/OSX/render_window.h>
#import <Kernel/OSX/document.h>


/***********************************************~***************************************************/


@implementation WCRenderWindow


- (id)initWithDocument:(WCDocument_OSX*)document
{
    // Do the regular Cocoa thing, specifying a particular nib.
    self = [super initWithWindowNibName:@"RenderWindow"];
	//Set the document
	_doc = document;
	//Setup remaining attributes
	[[self window] setDelegate:self];
	//Return self
    return self;
}


- (void)dealloc
{
    // Do the regular Cocoa thing.
    [super dealloc];
}


- (void)windowDidLoad
{
	//Make sure there is a renderView
	ASSERT(_renderView);
	//Do the Cocoa thing
	[super windowDidLoad];
	//Set the init status text
	[_statusText setObjectValue:@"Status: Ready"];
	[[self window] setAcceptsMouseMovedEvents:YES];
	//Setup the renderView
	[_renderView setDocument:_doc];
	//Setup tool tip rect
	[_renderView addToolTipRect:[_renderView frame] owner:_doc userData:nil];
	//Try calling to idle
	[[NSNotificationCenter defaultCenter] postNotificationName:@"OnIdleEvent" object:self];
}


- (void)windowWillClose:(NSNotification*)notification
{
	//Call willClose on the document
	[_doc onWindowWillClose];
}


- (void)windowDidResize:(NSNotification*)notification
{
	//Update the renderView
	[_renderView update];
	//Call resize on the document
	[_doc onResize:[_renderView frame].size];
}


- (void)windowDidBecomeKey:(NSNotification*)notification
{
	//Let document know window is now key
	[_doc onBecomeKey];
}


- (void)windowDidResignKey:(NSNotification*)notification
{
	//Let document know window is no longer key
	[_doc onResignKey];
}


- (NSTextField*)statusText
{
	//Return the status text object
	return _statusText;
}


- (void)mouseMoved:(NSEvent*)theEvent
{
	//Get current mouse location
	NSPoint point = [[self window] mouseLocationOutsideOfEventStream];
	//Check to see if mouse is within renderView
	BOOL hit = [_renderView mouse:point inRect:[_renderView frame]];
	//If so, call part document onMouseMove
	if (hit == TRUE) {
		//Convert the point to the renderView coordinate system
		point = [_renderView convertPointFromBase:point];
		int x = (int)point.x;
		int y = (int)point.y;
		//Invert the y coordinate (Wildcat thinks that upper left is 0,0 while OSX thinks lower-left is 0,0)
		int height = (int)([_renderView frame].size.height);
		y = height - y;
		///Call to partDocument
		[_doc onMouseMoveXPosition:x withYPosition:y];
	}
}


-(void)mouseDragged:(NSEvent*)theEvent{
	//Get current mouse location
	NSPoint point = [[self window] mouseLocationOutsideOfEventStream];
	//Check to see if mouse is within renderView
	BOOL hit = [_renderView mouse:point inRect:[_renderView frame]];
	//If so, call part document onMouseMove
	if (hit == TRUE) {
		//Convert the point to the renderView coordinate system
		point = [_renderView convertPointFromBase:point];
		int x = (int)point.x;
		int y = (int)point.y;
		//Invert the y coordinate (Wildcat thinks that upper left is 0,0 while OSX thinks lower-left is 0,0)
		int height = (int)([_renderView frame].size.height);
		y = height - y;
		///Call to partDocument
		[_doc onMouseMoveXPosition:x withYPosition:y];
	}
}


-(void)keyDown:(NSEvent*)theEvent
{
	//Pass event on to document
	[_doc onKeyPress:theEvent];
}


- (WCOpenGLView*)renderView
{
	//Return the render view
	return _renderView;
}


- (WCDocument_OSX*)document
{
	//Return the document
	return _doc;
}


@end


/***********************************************~***************************************************/

