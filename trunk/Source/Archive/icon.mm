/*** Included Header Files ***/
#include "icon.h"

/*** Platform Specific Included Headers ***/
#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#endif


/***********************************************~***************************************************/


WCIcon::WCIcon(std::string name) : _name(name), _texture(0), _width(0), _height(0) {
	//Convert std::string to NSString
	const char *cString = name.c_str();
	NSString* imageName = [NSString stringWithCString:cString];

	//Get the main bundle of the application
	NSBundle* bundle = [NSBundle mainBundle];
	//Load icon
	NSImage* image = [[NSImage alloc] initWithContentsOfFile:[bundle pathForImageResource:imageName]];
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
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	//Generate a new texture
	glGenTextures (1, &this->_texture);
	glBindTexture (GL_TEXTURE_RECTANGLE_ARB, this->_texture);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, imageSize.width, imageSize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, [bitmap bitmapData]);
	//Clean up
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	//Check for errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "ScrollbarMakeTextureFromImage - At clean up.");
	}
	//Set the width and height
	this->_width = [bitmap pixelsWide];
	this->_height = [bitmap pixelsHigh];
	//Release the bitmap
	[bitmap release];
}


WCIcon::~WCIcon() {
	//Delete the texture
	glDeleteTextures(1, &this->_texture);
}


/***********************************************~***************************************************/

