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
#include "Utility/adapter.h"


/*** Platform Specific Included Headers ***/
#include <tiffio.h>


/***********************************************~***************************************************/


void _TIFFWarning(const char* module, const char* fmt, va_list ap) {
	//Do nothing here for now
//	CLOGGER_ERROR(WCLogManager::RootLogger(), "_TIFFWarning caught");
}


void WCTextureManager::LoadTexture(WSTexture *texObj) {
	//Set the warning handler
	TIFFSetWarningHandler(_TIFFWarning); 
	//Create the image from file
	std::string filename = _ResourceDirectory() + "\\" + texObj->_name + ".tiff";
	TIFF *tif;
	try {
		tif = TIFFOpen(filename.c_str(), "r");
	} catch (...) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTextureManager::LoadTexture - Caught an exception.");
	}
	if (tif == NULL){
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTextureManager::LoadTexture - Not able to load: " << texObj->_name);
		return;
	}

	//Read the tif image data into a buffer
	char emsg[1024];
	TIFFRGBAImage img;
	TIFFRGBAImageBegin(&img, tif, 1, emsg);
	
	if(WCAdapter::HasGL15()) {
		texObj->_texture_width = img.width;
		texObj->_texture_height = img.height;
	}
	else {
		// use pow(2, n) for texture width and height
		for(texObj->_texture_width = 1; texObj->_texture_width < img.width; texObj->_texture_width *= 2){}
		for(texObj->_texture_height = 1; texObj->_texture_height < img.height; texObj->_texture_height *= 2){}
	}

	//Get all of the raster data
	size_t npixels = texObj->_texture_width * texObj->_texture_height;
	uint32 *data = new uint32[npixels];
	TIFFReadRGBAImageOriented(tif, texObj->_texture_width, texObj->_texture_height, data, ORIENTATION_TOPLEFT, 0);
	TIFFRGBAImageEnd(&img);

	//Enable texturing
	glEnable(texObj->_target);
	//Generate a new texture
	glGenTextures (1, &(texObj->_id));
	glBindTexture (texObj->_target, texObj->_id);
	//Set the min/mag filters
	glTexParameteri(texObj->_target, GL_TEXTURE_MIN_FILTER, texObj->_minFilter);
	glTexParameteri(texObj->_target, GL_TEXTURE_MAG_FILTER, texObj->_magFilter);
	glTexParameteri(texObj->_target, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(texObj->_target, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(texObj->_target, 0, 4, texObj->_texture_width, texObj->_texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//Clean up
	glDisable(texObj->_target);
	//Check for errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTextureManager::LoadTexture - At clean up on image: " << texObj->_name);
	}
	//Get the object width and height
    texObj->_width = (GLfloat)img.width;
    texObj->_height = (GLfloat)img.height;
	//Clean up
	delete data;
}


/***********************************************~***************************************************/

