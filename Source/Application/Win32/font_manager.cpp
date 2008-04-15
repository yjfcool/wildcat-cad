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
#include "Application/Win32/font_manager.h"


/*** Locally Defined Values ***/
//None


/***********************************************~***************************************************/


// This is a highly modified version from tutorial 43 NeHe.GameDev.Net
void WCFont::GenerateCharacter(FT_Face &face, const char &character) {
	//Load the Glyph for our character.
	if (FT_Load_Glyph( face, FT_Get_Char_Index(face, character), FT_LOAD_DEFAULT )) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCFont::GenerateCharacter - FT_Load_Glyph failed");
		return;
	}
	//Get pointer to face slot
	FT_GlyphSlot slot = face->glyph;
	//Render the glyph to a bitmap
	FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

	//Set the width, height, and vertical bearing of the character
	this->_chars[character].width = (GLfloat)slot->bitmap.width;
	this->_chars[character].height = (GLfloat)slot->bitmap.rows;
	this->_chars[character].bearing = (GLfloat)slot->metrics.horiBearingY / 64.0f;
	//Allocate memory for the texture data (four component)
	GLubyte *data = new GLubyte[4 * slot->bitmap.width * slot->bitmap.rows];
	//Set color to white and alpha to bitmap value
	unsigned int bmIndex = 0, dataIndex = 0;
	unsigned char byte;
	for(int j=0; j < slot->bitmap.rows; j++) {
		for(int i=0; i < slot->bitmap.width; i++){
			//Get the byte value
			byte = slot->bitmap.buffer[bmIndex++];
			data[dataIndex++] = byte;
			data[dataIndex++] = byte;
			data[dataIndex++] = byte;
			data[dataIndex++] = byte;
		}
	}

	//Now we just setup some texture paramaters.
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_textures[character]);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//Load the data into the texture
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, slot->bitmap.width, slot->bitmap.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//Delete the texture data
    delete data;
}


// This code is taken from tutorial 43 of NeHe.GameDev.Net
WCFont::WCFont(FT_Library &library, const std::string &filename, const GLfloat &height) : _height(height) {
	//Create new FreeType face
	FT_Face face;
	if (FT_New_Face(library, filename.c_str(), 0, &face)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCFont::WCFont - Problem with font file:" << filename);
		return;
	}
	//For some twisted reason, Freetype measures font size
	//in terms of 1/64ths of pixels.  Thus, to make a font
	//h pixels high, we need to request a size of h*64.
	//(h << 6 is just a prettier way of writting h*64)
	FT_F26Dot6 size = (FT_F26Dot6)(this->_height * 64);
	FT_Set_Char_Size(face, size, size, 72, 72);

	//Generate all 128 GL textures
	glGenTextures(128, this->_textures);
	//This is where we actually create each of the character objects
	for(unsigned char i=0;i<128;i++) {
		this->GenerateCharacter(face, i);
	}

	//Get rid of FreeType face
	FT_Done_Face(face);
	//Check for errors
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCFont::WCFont - Unspecified GL Errors.");
}


WCFont::~WCFont() {
	//Delete GL textures
	glDeleteTextures(128, this->_textures);
}


void WCFont::StringSize(const std::string &str, GLfloat &width, GLfloat &height, GLfloat &origin) {
	//Get the c-string
	const char* cStr = str.c_str();
	unsigned int length = str.length();
	//Reset width and bearing
	GLfloat maxAbove=0, maxBelow=0;
	GLfloat above, below;
	width = 0;
	//Loop through all chars
	for (unsigned int i=0; i < length; i++) {
		//Determine above and below
		above = this->_chars[ cStr[i] ].bearing;
		below = this->_chars[ cStr[i] ].height - above;
		//Find the max above and below
		maxAbove = STDMAX(above, maxAbove);
		maxBelow = STDMAX(below, maxBelow);
		//Add the incremental width of this char
		width += this->_chars[ cStr[i] ].width;
//		CLOGGER_INFO(WCLogManager::RootLogger(), cStr[i] << ": " << this->_chars[ cStr[i] ].height << " - " << above);
	}
	//Determine height and bearing
	height = maxAbove + maxBelow;
	origin = maxBelow;
//	CLOGGER_INFO(WCLogManager::RootLogger(), str << ": " << height << " - " << origin);
}


/***********************************************~***************************************************/


WCFontManager::WCFontManager(const std::string &manifest, const std::string &directory, const bool &verbose) {
	//Create and initilize a freetype font library.
	FT_Library library;
	if (FT_Init_FreeType( &library )) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCFontManager::WCFontManager - FT_Init_FreeType failed");
		return;
	}

	//Initialize all the fonts
	WCFont *palatino = new WCFont(library, _FontDirectory() + "\\pala.TTF", 16.0f);
	this->_fontMap.insert( std::make_pair("palatino_16", palatino) );

	//Ditto for the library.
	FT_Done_FreeType(library);
}


WCFontManager::~WCFontManager() {
	//Delete all fonts
	std::map<std::string, WCFont*>::iterator iter;
	while (!this->_fontMap.empty()) {
		//Get the first item in the map
		iter = this->_fontMap.begin();
		//And delete it
		delete (*iter).second;
	}
}

	
WCFont* WCFontManager::FontFromName(const std::string &name) {
	//Check to see if the name is in the font map
	std::map<std::string, WCFont*>::iterator iter = this->_fontMap.find(name);
	//See if you found anything
	if( iter == this->_fontMap.end() ) {
	//Not able to find
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCFontManager::FontFromName - Not able to find font: " << name);
		return NULL;
	}
	//Otherwise, get the object
	return (*iter).second;
}


/***********************************************~***************************************************/

