/*
HMBlkButtonCell.m

Author: Makoto Kinoshita

Copyright 2004-2006 The Shiira Project. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted 
provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this list of conditions 
  and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice, this list of 
  conditions and the following disclaimer in the documentation and/or other materials provided 
  with the distribution.

THIS SOFTWARE IS PROVIDED BY THE SHIIRA PROJECT ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE SHIIRA PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
*/

#import "Application/MacOS/HMBlkButtonCell.h"

@implementation HMBlkButtonCell

//--------------------------------------------------------------//
#pragma mark -- Initialize --
//--------------------------------------------------------------//

- (void)mouseEntered:(NSEvent*)event
{
    [[self controlView] setNeedsDisplay:YES];
}

- (void)mouseExited:(NSEvent*)event
{
    [[self controlView] setNeedsDisplay:YES];
}

//--------------------------------------------------------------//
#pragma mark -- Drawing --
//--------------------------------------------------------------//

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView*)controlView
{
    // Get mouse location
    NSPoint point;
    point = [NSEvent mouseLocation];
    point = [[controlView window] convertScreenToBase:point];
    point = [controlView convertPoint:point fromView:nil];
    
    // For not mouse over
    if (!NSPointInRect(point, cellFrame)) {
        // Get image
        NSImage*    image;
        image = [self image];
        if (image) {
            // Draw image with fraction
            NSRect  srcRect, destRect;
            srcRect.origin = NSZeroPoint;
            srcRect.size = [image size];
            destRect = srcRect;
            [image drawInRect:destRect fromRect:srcRect operation:NSCompositeSourceOver fraction:0.3f];
        }
    }
    else {
        // Invoke super
        [super drawInteriorWithFrame:cellFrame inView:controlView];
    }
}

@end
