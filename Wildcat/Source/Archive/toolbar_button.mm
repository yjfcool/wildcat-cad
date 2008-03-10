/*** Imported Header Files ***/
#import "Application/MacOS/toolbar_button.h"


/*** Included Header Files ***/
#include "Application/toolbar_manager.h"


/***********************************************~***************************************************/


@implementation WCToolbarButton


+ (Class)cellClass
{
	return [WCToolbarButtonCell class];
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


@end


/***********************************************~***************************************************/


@implementation WCToolbarButtonCell


- (void)mouseEntered:(NSEvent*)event
{
	//Set needs display
	[[self controlView] setNeedsDisplay:YES];
}


- (void)mouseExited:(NSEvent*)event
{
	//Set needs display
	[[self controlView] setNeedsDisplay:YES];
}



- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView*)controlView
{
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
	NSRect selfRect;
	selfRect.origin = NSZeroPoint;
	selfRect.size = [image size];
	[[NSColor colorWithCalibratedWhite:0.4 alpha:1.0] set];
	NSFrameRect(selfRect);
}


- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView*)controlView
{
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
//	//Draw rect around image
//	NSRect selfRect;
//	selfRect.origin = NSZeroPoint;
//	selfRect.size = [image size];
//	[[NSColor colorWithCalibratedWhite:0.1 alpha:0.1] set];
//	NSFrameRect(selfRect);
}


@end


/***********************************************~***************************************************/

