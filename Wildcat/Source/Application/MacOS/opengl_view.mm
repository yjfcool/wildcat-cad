//Imported Header Files
#import "Application/MacOS/opengl_view.h"
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

