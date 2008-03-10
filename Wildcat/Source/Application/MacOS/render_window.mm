/*** Imported Header Files ***/
#import "Application/MacOS/render_window.h"
#import "Application/MacOS/document.h"


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

	//Try to get main menu
//	NSMenu *menu = [[NSApplication sharedApplication] mainMenu];
//	[[NSApplication sharedApplication] setWindowsMenu:menu];
//	[menu setAutoenablesItems:NO];
//	NSMenuItem *undo = [menu itemWithTitle:@"Undo"];
//	NSMenuItem *redo = [menu itemWithTitle:@"Redo"];
//	[undo setEnabled:YES];
//	[redo setEnabled:YES];
}


- (void)windowWillClose:(NSNotification*)notification
{
	//Call willClose on the document
	[_doc onWindowWillClose];
}


- (void)windowDidResize:(NSNotification*)notification
{
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

