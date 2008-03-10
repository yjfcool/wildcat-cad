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


/*** Included Header Files***/
#include "Scene/text.h"


/*** Platform Specific Included Headers ***/
#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#endif


/***********************************************~***************************************************/


void WCText::GenerateTexture(void) {
	//Set up the text color
	NSColor *textColor = [NSColor colorWithCalibratedRed:this->_color.R() green:this->_color.G() blue:this->_color.B() alpha:this->_color.A()];
	//Set up the font
	const char* cstring = this->_fontName.c_str();
	NSString *aString = [[NSString alloc] initWithCString:cstring];
	NSFont *font = [NSFont fontWithName:aString size:this->_fontSize];
	[aString release];
	//Also set the color
	NSDictionary *attribs;
	//Switch depending on underlined state
	if (this->_isUnderlined) {
		attribs = [NSDictionary dictionaryWithObjectsAndKeys:font, NSFontAttributeName, 
															 textColor, NSForegroundColorAttributeName, 
															 [NSNumber numberWithInt:NSSingleUnderlineStyle], NSUnderlineStyleAttributeName, 
															 nil];
	}
	//No underline
	else {
		attribs = [NSDictionary dictionaryWithObjectsAndKeys:font, NSFontAttributeName, 
															 textColor, NSForegroundColorAttributeName, 
															 nil];	
	}
	cstring = this->_text.c_str();
	aString = [[NSString alloc] initWithCString:cstring];
	//Set up attributed string
	NSAttributedString *attribString = [[NSAttributedString alloc] initWithString:aString attributes:attribs];
	[aString release];

	NSImage *image;
	NSBitmapImageRep *bitmap;
	//Set the previous size
	NSSize previousSize;
	previousSize.width = this->_texWidth;
	previousSize.height = this->_texHeight;
	NSSize frameSize = [attribString size];

	//Create image for drawing string
	image = [[NSImage alloc] initWithSize:frameSize];
	[image lockFocus];
	//Set antialiasing for the context
	[[NSGraphicsContext currentContext] setShouldAntialias:NO];
	//Draw string at offset position
	[attribString drawAtPoint:NSMakePoint(0.0, 0.0)];
	//Get bitmap from image
	bitmap = [[NSBitmapImageRep alloc] initWithFocusedViewRect:NSMakeRect(0.0f, 0.0f, frameSize.width, frameSize.height)];
	//Unlock from bitmap
	[image unlockFocus];
	//Determine texture size
	this->_texWidth = [bitmap pixelsWide];
	this->_texHeight = [bitmap pixelsHigh];

	//Enable texturing
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	//If no texture exists, gen a new one
	if (0 == this->_texture) glGenTextures (1, &this->_texture);
	glBindTexture (GL_TEXTURE_RECTANGLE_ARB, this->_texture);
	//If same size as old texture, just reuse
	if ((previousSize.width == this->_texWidth) && (previousSize.height == this->_texHeight)) {
		glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_texWidth, this->_texHeight, GL_RGBA, GL_UNSIGNED_BYTE, [bitmap bitmapData]);
	} 
	//Otherwise, set up all new texture
	else {
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, this->_texWidth, this->_texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, [bitmap bitmapData]);
	}
	//Disable texture state
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	
	//Clean up a bit
	[bitmap release];
	[image release];
	[attribString release]; 
	//Check for errors
	if (glGetError() != GL_NO_ERROR) {
		glDeleteTextures(1, &this->_texture);
		this->_texture = 0;
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeView::Render Error - GL error at Exit.");
	}
	//No longer dirty
	this->_isDirty = false;
}


/***********************************************~***************************************************/

