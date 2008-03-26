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
//#include <gdiplus.h>
#include <tiffio.h>

/***********************************************~***************************************************


void WCTextureManager::LoadTexture(WSTexture *texObj) {
   // Initialize GDI+.
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//Create the image from file
	std::string filename = _ResourceDirectory() + "\\" + texObj->_name;
	Gdiplus::Bitmap *bitmap = Gdiplus::Bitmap::FromFile(L"C:\\Documents and Settings\\ghemingway\\My Documents\\Wildcat\\Build\\Win32\\Resources\\conic32.tiff");
	
	//Set the width and height
	texObj->_width = (GLfloat)bitmap->GetWidth();
	texObj->_height = (GLfloat)bitmap->GetHeight();
	//Create rect and temp array for data transfer
	Gdiplus::Rect rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
	GLfloat *data = new GLfloat[bitmap->GetWidth() * bitmap->GetHeight()];
	Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;

	// Get access to the bitmap bits
	bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, bitmap->GetPixelFormat(), bitmapData);
	UINT* pixels = (UINT*)bitmapData->Scan0;
	//Copy all data from bitmap into GL array
	for (UINT iy = 0; iy < bitmapData->Height; iy++) {
		for (UINT ix = 0; ix < bitmapData->Width; ix++) {
			data[iy, ix] = (GLfloat)(*(pixels + ix));
		}
		pixels += bitmapData->Stride / 2;   // NOTE: /2 because we're accessing words!
	}

	//Try reversing the order
	for (int i = 0; i < texObj->_width * texObj->_height; i++) {
		unsigned char *cp = (unsigned char *) &data[i];
		int t = cp[3];
		cp[3] = cp[0];
		cp[0] = t;
		t = cp[2];
		cp[2] = cp[1];
		cp[1] = t;
	}

	//Enable texturing
	glEnable(texObj->_target);
	//Generate a new texture
	glGenTextures (1, &(texObj->_id));
	glBindTexture (texObj->_target, texObj->_id);
	//Set the min/mag filters
	glTexParameteri(texObj->_target, GL_TEXTURE_MIN_FILTER, texObj->_minFilter);
	glTexParameteri(texObj->_target, GL_TEXTURE_MAG_FILTER, texObj->_magFilter);
	glTexImage2D(texObj->_target, 0, GL_RGBA, bitmapData->Width, bitmapData->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//Clean up
	glDisable(texObj->_target);
	//Check for errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTextureManager::LoadTexture - At clean up on image: " << texObj->_name);
	}

	//Clean up
	bitmap->UnlockBits(bitmapData);
	delete data;
	delete bitmapData;
	delete bitmap;
	Gdiplus::GdiplusShutdown(gdiplusToken);
}


/***********************************************~***************************************************/


void WCTextureManager::LoadTexture(WSTexture *texObj) {
	char emsg[1024];
	TIFFRGBAImage img;
	size_t npixels;
	uint32 *data;

	//Create the image from file
	std::string filename = _ResourceDirectory() + "\\" + texObj->_name + ".tiff";
	TIFF *tif = TIFFOpen(filename.c_str(), "r");
	if (tif == NULL){
		fprintf(stderr, "tif == NULL\n");
		exit(1);
	}

	if (TIFFRGBAImageBegin(&img, tif, 0, emsg)){
		npixels = img.width*img.height;
		data = (uint32 *)_TIFFmalloc(npixels*sizeof(uint32));
		if (data != NULL){
			if (TIFFRGBAImageGet(&img, data, img.width, img.height) == 0){
//				TIFFError(name, emsg);
				exit(1);
			}
		}
		TIFFRGBAImageEnd(&img);
//		fprintf(stderr, "Read image %s (%d x %d)\n", name, img.width, img.height);
	}
	else {
//		TIFFError(name, emsg);
		exit(1);
	}
  
	// If cannot directly display ABGR format, we need to reverse the component ordering in each pixel. :-(
	if (true) {
		for (size_t i = 0; i < npixels; i++) {
			unsigned char *cp = (unsigned char *) &data[i];
			int t = cp[3];
			cp[3] = cp[0];
			cp[0] = t;
			t = cp[2];
			cp[2] = cp[1];
			cp[1] = t;
		}
	}

	//Enable texturing
	glEnable(texObj->_target);
	//Generate a new texture
	glGenTextures (1, &(texObj->_id));
	glBindTexture (texObj->_target, texObj->_id);
	//Set the min/mag filters
	glTexParameteri(texObj->_target, GL_TEXTURE_MIN_FILTER, texObj->_minFilter);
	glTexParameteri(texObj->_target, GL_TEXTURE_MAG_FILTER, texObj->_magFilter);
	glTexImage2D(texObj->_target, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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
}


/***********************************************~***************************************************/

