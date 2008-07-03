//Imported Header Files
#import "Application/MacOS/opengl_view.h"
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


//Imported Header Files
#import "Application/MacOS/document.h"


/***********************************************~***************************************************/


@implementation WCOpenGLView


- (void)drawRect:(NSRect)rect
{
	//Need to call part document onDisplay
	[_document onDisplay];
}


- (BOOL)acceptsFirstResponder
{
	return YES;
}


- (void)mouseDown:(NSEvent *)theEvent
{
//	NSLog(@"WCOpenGLView::mouseDown");
	//Call to active workbench onMousePress
	[_document onMousePress:theEvent];
}


- (void)mouseUp:(NSEvent *)theEvent
{
//	NSLog(@"WCOpenGLView::mouseUp");
	//Call to active workbench onMouseRelease
	[_document onMouseRelease:theEvent];
}


- (void)rightMouseDown:(NSEvent *)theEvent
{
//	NSLog(@"WCOpenGLView::rightMouseDown");
	//Call to active workbench onMousePress
	[_document onMousePress:theEvent];
}


- (void)rightMouseUp:(NSEvent *)theEvent
{
//	NSLog(@"WCOpenGLView::rightMouseUp");
	//Call to active workbench onMouseRelease
	[_document onMouseRelease:theEvent];
}


- (void)otherMouseDown:(NSEvent *)theEvent
{
//	NSLog(@"WCOpenGLView::otherMouseDown");
	//Call to active workbench onMousePress
	[_document onMousePress:theEvent];
}


- (void)otherMouseUp:(NSEvent *)theEvent
{
//	NSLog(@"WCOpenGLView::otherMouseUp");
	//Call to active workbench onMouseRelease
	[_document onMouseRelease:theEvent];
}


- (void)scrollWheel:(NSEvent *)theEvent
{
//	NSLog(@"WCOpenGLView::scrollWheel");
	//Call to active workbench onScrollWhell method
	[_document onScrollWheel:theEvent];
}


-(void)setDocument:(WCDocument_OSX*)doc
{
	//Set the document
	_document = doc;
}


-(WCDocument_OSX*)document
{
	//Get the document
	return _document;
}


@end


/***********************************************~***************************************************/

