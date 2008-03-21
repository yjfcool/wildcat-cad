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


/*** Included Header Files ***/
#include "Utility/texture_manager.h"
#include "Utility/log_manager.h"


/*** Platform Specific Included Headers ***/
#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#endif


/***********************************************~***************************************************/


void WCTextureManager::LoadTexture(WSTexture *texObj) {
	//Convert std::string to NSString
	const char *cString = texObj->_name.c_str();
	NSString* imageName = [NSString stringWithCString:cString];

	//Get the main bundle of the application
	NSBundle* bundle = [NSBundle mainBundle];
	//Load icon
	NSImage* image = [[NSImage alloc] initWithContentsOfFile:[bundle pathForImageResource:imageName]];
	//Check to make sure image was loaded
	if (image == nil) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTextureManager::LoadTexture - Unable to load image: " << texObj->_name);
		//Exit it not able to load
		return;
	}
	//Get the size of the image
	NSSize imageSize = [image size];
	//Lock focus onto the texture
	[image lockFocus];
	//Get bitmap from image
	NSBitmapImageRep *bitmap = [[NSBitmapImageRep alloc] initWithFocusedViewRect:NSMakeRect(0.0f, 0.0f, imageSize.width, imageSize.height)];
	//Unlock from image
	[image unlockFocus];
	[image release];

	//Enable texturing
	glEnable(texObj->_target);
	//Generate a new texture
	glGenTextures (1, &(texObj->_id));
	glBindTexture (texObj->_target, texObj->_id);
	//Set the min/mag filters
	glTexParameteri(texObj->_target, GL_TEXTURE_MIN_FILTER, texObj->_minFilter);
	glTexParameteri(texObj->_target, GL_TEXTURE_MAG_FILTER, texObj->_magFilter);
	glTexImage2D(texObj->_target, 0, GL_RGBA, imageSize.width, imageSize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, [bitmap bitmapData]);
	//Clean up
	glDisable(texObj->_target);
	//Check for errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTextureManager::LoadTexture - At clean up on image: " << texObj->_name);
	}
	//Set the width and height
	texObj->_width = [bitmap pixelsWide];
	texObj->_height = [bitmap pixelsHigh];
	//Release the bitmap
	[bitmap release];
}


/***********************************************~***************************************************/

