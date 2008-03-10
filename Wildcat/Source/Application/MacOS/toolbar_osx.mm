/*** Imported Header Files ***/
#import "Application/MacOS/toolbar_osx.h"
#import "Application/MacOS/HMBlkPanel.h"


/*** Included Header Files ***/
#include "Application/MacOS/toolbar_bridge.h"
#include "Application/toolbar.h"


/***********************************************~***************************************************/


@implementation WCToolbarButton_OSX


+ (Class)cellClass
{
	return [WCToolbarButtonCell_OSX class];
}


- (void)viewDidMoveToWindow
{
	if ([self window]) {
		if (![self showsBorderOnlyWhileMouseInside]) {
			[self setShowsBorderOnlyWhileMouseInside:YES];
		}
	}
	else {
		[self setShowsBorderOnlyWhileMouseInside:NO];
	}
}


- (id)initWithBridge:(WCToolbarButton_Bridge*)bridge frame:(NSRect)frame
{
	//Do the cocoa thing
	self = [super initWithFrame:frame];
	//Make sure is valid
	if (self != nil) _bridge = bridge;
	//Return self
	return self;
}


- (WCToolbarButton_Bridge*)bridge
{
	//Return bridge
	return _bridge;
}


@end


/***********************************************~***************************************************/


@implementation WCToolbarButtonCell_OSX


- (void)mouseEntered:(NSEvent*)event
{
	//Set needs display
//	[[self controlView] setNeedsDisplay:YES];
}


- (void)mouseExited:(NSEvent*)event
{
	//Set needs display
//	[[self controlView] setNeedsDisplay:YES];
}



- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView*)controlView
{
	//Get the bridge
	WCToolbarButton_OSX *button = (WCToolbarButton_OSX*)controlView;
	WCToolbarButton_Bridge *bridge = [button bridge];

	//Get images
	NSImage *stdImage = bridge->StandardIcon();
//	NSImage *activeImage = bridge->StandardIcon();
	NSRect  srcRect, destRect;
	srcRect.origin = NSZeroPoint;
	srcRect.size = [stdImage size];
	destRect = srcRect;

	//See if not enabled
	//If there is a valid image
	if (!bridge->IsEnabled())
		[stdImage drawInRect:destRect fromRect:srcRect operation:NSCompositeSourceOver fraction:0.1f];
	//Otherwise...
	else {
		switch(bridge->Type()) {
			case TOOLBARELEMENT_TYPE_MOMENTARY:
				[stdImage drawInRect:destRect fromRect:srcRect operation:NSCompositeSourceOver fraction:1.0f];
				break;
			case TOOLBARELEMENT_TYPE_TOGGLE:
				//Draw bold 
				if (bridge->IsActive()) {
					[stdImage drawInRect:destRect fromRect:srcRect operation:NSCompositeSourceOver fraction:1.0f];
				}
				//Draw the inactive version
				else {
/*
					NSLog(@"X %f", destRect.origin.x);
					NSLog(@"Y %f", destRect.origin.y);
					NSLog(@"Width %f", destRect.size.width);
					NSLog(@"Height %f", destRect.size.height);
*/
					[stdImage drawInRect:destRect fromRect:srcRect operation:NSCompositeSourceOver fraction:0.25f];
				}
				break;
			case TOOLBARELEMENT_TYPE_MODE:
				[stdImage drawInRect:destRect fromRect:srcRect operation:NSCompositeSourceOver fraction:1.0f];
				break;
		}
	}
	//Draw rect around image
	NSRect selfRect;
	selfRect.origin = NSZeroPoint;
	selfRect.size = [stdImage size];
	[[NSColor colorWithCalibratedWhite:0.4 alpha:1.0] set];
	NSFrameRect(selfRect);
}


- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView*)controlView
{
	NSLog(@"Drawing Interior");
	// Get image
	NSImage* image;
	image = [self image];
	//If there is a valid image
	if (image) {
		// Draw image with fraction
		NSRect  srcRect, destRect;
		srcRect.origin = NSZeroPoint;
		srcRect.size = [image size];
		destRect = srcRect;
		[image drawInRect:destRect fromRect:srcRect operation:NSCompositeSourceOver fraction:1.0f];
	}
	//Draw rect around image
//	NSRect selfRect = [self drawingRectFor];
//	selfRect.origin = NSZeroPoint;
//	selfRect.size = [image size];
	[[NSColor colorWithCalibratedWhite:0.4 alpha:1.0] set];
	NSFrameRect(cellFrame);}


@end


/***********************************************~***************************************************/


@implementation WCToolbar_OSX


- (void)setButtonPlacement
{
/*
	//Find the width
	float width = [[self window] frame].size.width - (2 * TOOLBAR_ICON_PADDING);
	float height = [[self window] frame].size.height;
	int cols = floor(width / (TOOLBAR_ICON_SMALLSIZE + (2 * TOOLBAR_ICON_PADDING)));
	cols = std::max(cols, 1);
	int col, row;
	//Loop through all buttons and set placement
	float x, y;
	float offset = TOOLBAR_ICON_SMALLSIZE + (2 * TOOLBAR_ICON_PADDING);
	NSRect placementRect;
	NSButton *button;
	for (NSUInteger index=0; index < [_buttons count]; index++) {
		//Figure out row & col
		row = (index / cols) + 1;
		col = index % cols;
		//Get the button
		button = [_buttons objectAtIndex:index];
		//Determine placement
		x = (2 * TOOLBAR_ICON_PADDING) + (col * offset);
		y = (height - TOOLBAR_HEADER_HEIGHT) - (row * offset) + 8;
		//Create the rectangle
		placementRect = NSMakeRect(x, y, TOOLBAR_ICON_SMALLSIZE, TOOLBAR_ICON_SMALLSIZE);
		//Update button placement
		[button setFrame:placementRect];
	}
*/
}


- (NSRect)setToolbarSize
{
	//Get the upper left corner
	NSRect frame = [[self window] frame];
/*
	float upperLeftX = frame.origin.x;
	float upperLeftY = frame.origin.y + frame.size.height;

	//Determine height and width
	float offset = TOOLBAR_ICON_SMALLSIZE + (2 * TOOLBAR_ICON_PADDING);
	float cols = floor((frame.size.width - 2 * TOOLBAR_ICON_PADDING) / offset );
	float rows = ceil(_toolbar->Elements().size() / cols);
	float height = TOOLBAR_HEADER_HEIGHT + (rows * offset) + (2 * TOOLBAR_ICON_PADDING);
	//Determine new Y
	float newY = upperLeftY - height;

	//Create the rect
	NSRect newRect = NSMakeRect(upperLeftX, newY, frame.size.width, height);
*/
	//Return the rect
	return frame;
}


/***********************************************~***************************************************/


- (id)initWithBridge:(WCToolbar_Bridge*)bridge title:(NSString*)title position:(NSRect)position
{
	//Setup inital panelWindow
	HMBlkPanel *panelWindow = [[HMBlkPanel alloc] initWithContentRect:position 
												  styleMask:NSBorderlessWindowMask 
												  backing:NSBackingStoreRetained 
												  defer:NO];
    // Do the regular Cocoa thing
    self = [super initWithWindow:panelWindow];
	//Setup bridge
	_bridge = bridge;
	//Setup remaining tb attributes
	[[self window] setDelegate:self];
	//Set toolbar title
	if (title != nil) [[self window] setTitle:title];
	//Allocate NSArray for buttons
	_buttons = [[NSMutableArray array] retain];
	//Return self
    return self;
}



- (void)dealloc
{
    // Do the regular Cocoa thing.
    [super dealloc];
	//Do a bunch of other things here
	[_buttons release];
}


- (void)windowDidResize:(NSNotification*)notification
{
/*
	//Make sure not too skinny
	NSRect tbSize = [[self window] frame];
	NSRect tbOldSize = tbSize;
	float offset = TOOLBAR_ICON_SMALLSIZE + (2 * TOOLBAR_ICON_PADDING);
	//Get the upper left coordinates
	float upperLeftX = [[self window] frame].origin.x;
	float upperLeftY = [[self window] frame].origin.y + [[self window] frame].size.height;
//	NSLog(@"UpperLeftX: %f", upperLeftX);
//	NSLog(@"upperLeftY: %f", upperLeftY);

	//Determine new height and y
	float cols = floor((tbSize.size.width - 2 * TOOLBAR_ICON_PADDING) / offset);
	int count = 10;//[_buttons count];
	float rows = ceil(count / cols);
//	NSLog(@"Cols: %f", cols);
//	NSLog(@"Rows: %f", rows);

	float height = TOOLBAR_HEADER_HEIGHT + (rows * offset) + TOOLBAR_FOOTER_HEIGHT;
	//Determine new Y
	float newY = upperLeftY - height;
	//Create the rect
	tbSize = NSMakeRect(upperLeftX, newY, tbSize.size.width, height);
	//If the rect has changed...
	if (!NSEqualRects(tbSize, tbOldSize))
		//Resize the toolbar
		[[self window] setFrame:tbSize display:YES animate:NO];

	//Reset button placement
	[self setButtonPlacement];
*/
}


- (void)onButtonPress:(id)sender
{
	//Call on press to bridge toolbar
	_bridge->OnButtonPress( sender );
}


- (void)onCloseButtonPress
{
	//Call to the bridge about the close
	_bridge->OnCloseButtonPress();
}


- (void)addButton:(WCToolbarButton_Bridge*)bridge
{
	//Find the width
	float width = [[self window] frame].size.width - (2 * TOOLBAR_ICON_PADDING);
	float height = [[self window] frame].size.height;
	int cols = floor(width / (TOOLBAR_ICON_SMALLSIZE + (2 * TOOLBAR_ICON_PADDING)));
	cols = std::max(cols, 1);
	int col, row;
	float x, y;
	float offset = TOOLBAR_ICON_SMALLSIZE + (2 * TOOLBAR_ICON_PADDING);
	//Figure out row & col
	int index = [_buttons count];
	row = index / cols + 1;
	col = index % cols;
	//Determine placement
	x = (2 * TOOLBAR_ICON_PADDING) + (col * offset);
	y = (height - TOOLBAR_HEADER_HEIGHT) - (row * offset) + 8;
	//Create the rectangle
	NSRect buttonRect = NSMakeRect(x, y, TOOLBAR_ICON_SMALLSIZE, TOOLBAR_ICON_SMALLSIZE);

	//Create the button
	WCToolbarButton_OSX *button = [[WCToolbarButton_OSX alloc] initWithBridge:bridge frame:buttonRect];
	[button setButtonType:NSToggleButton];
	[button setBordered:NO];
	[button setImagePosition:NSImageOnly];
	//Set the tooltip
	[button setToolTip:bridge->Tooltip()];
	//Set final button stuff
	[button setTarget:self];
	[button setAction:@selector(onButtonPress:)];
	[[[self window] contentView] addSubview:button];

	//Add to the list of buttons
	[_buttons addObject:button];
	//Let the bridge know about this instace
	bridge->AddInstance(button);
}
/*
	//Determine toolbar position and size
	NSRect contentRect;
	float offset = TOOLBAR_ICON_SMALLSIZE + (2 * TOOLBAR_ICON_PADDING);
	//No pre-specified position
	if ((_toolbar->Position().I() == -1.0) && (_toolbar->Position().J() == -1.0)) {
		//Use two columns
		float cols = 2;
		float width = (cols * offset) + (2 * TOOLBAR_ICON_PADDING);
		float rows = ceil(_toolbar->Elements().size() / cols);
		float height = TOOLBAR_HEADER_HEIGHT + (rows * offset) + TOOLBAR_FOOTER_HEIGHT;
		//Determine x, y location
		NSRect parentRect = [[[document renderWindow] window] frame];
		float x = parentRect.origin.x + parentRect.size.width;
		float y = parentRect.origin.y + parentRect.size.height - height;
		//Get content rect
		contentRect = NSMakeRect(x, y, width, height);
	}
	//Have valid position
	else {
		//I->x, J->y, K->width, L->height
		contentRect = NSMakeRect(_toolbar->Position().I(), _toolbar->Position().J(), _toolbar->Position().K(), _toolbar->Position().L());
	}

	//Set the position
	[[self window] setFrame:contentRect display:YES];
	//Set min/max size
	NSSize minSize = NSMakeSize((2 * TOOLBAR_ICON_PADDING) + offset, TOOLBAR_HEADER_HEIGHT + offset + (2 * TOOLBAR_ICON_PADDING));
	NSSize maxSize = NSMakeSize((2 * TOOLBAR_ICON_PADDING) + ([_buttons count] * offset), 
								 TOOLBAR_HEADER_HEIGHT + ([_buttons count] * offset) + TOOLBAR_FOOTER_HEIGHT);
	[[self window] setMinSize:minSize];
	[[self window] setMaxSize:maxSize];
*/


- (void)setVisible:(bool)state
{
	//Make with visible
	if (state) [[self window] orderFront:self];
	//Otherwise hide window
	else [[self window] orderOut:self];
}


@end


/***********************************************~***************************************************/

