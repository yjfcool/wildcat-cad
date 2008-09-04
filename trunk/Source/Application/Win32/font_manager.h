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


#ifndef __FONT_MANAGER_H__
#define __FONT_MANAGER_H__


/*** Included Header Files ***/
#include "Utility/wutil.h"
#include "Utility/log_manager.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>


/*** Locally Defined Values ***/
#define FONT_DIRECTORY							"C:\\Windows\\Fonts"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


struct WSFontCharSize {
	GLfloat										width, height, bearing;
	int texture_width, texture_height;  // used for when !WCAdapter::HasGL15() and they have to be pow(2, n)
};


/***********************************************~***************************************************/


class WCFont {
private:
	WSFontCharSize								_chars[128];										//!< Array of character objects
	WPFloat										_height;											//!< Height of the font.
	GLuint										_textures[128];										//!< Holder for GL texture IDs
	//Private Methods
	void GenerateCharacter(FT_Face &face, const char &character);									//!< Generate a given character
	//Hidden Constructors
	WCFont();																						//!< Deny access to default constructor
	WCFont(const WCFont&);																			//!< Deny access to copy constructor
	WCFont& operator=(const WCFont&);																//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCFont(FT_Library &library, const std::string &filename, const GLfloat &height);				//!< Default constructor
	~WCFont();																						//!< Default destructor
	
	//General Methods
	void StringSize(const std::string &str, GLfloat &width, GLfloat &height, GLfloat &origin);		//!< Get the size of a given string
	inline GLuint* Textures(void)				{ return this->_textures; }							//!< Return the textures
	inline WSFontCharSize* Sizes(void)			{ return this->_chars; }							//!< Return the character sizes
};


/***********************************************~***************************************************/


class WCFontManager {
private:
	std::map<std::string, WCFont*>				_fontMap;											//!< Map of all toolbar buttons
	//Private Methods
	void CreateFont(void);																			//!< Create the given font
	//Hidden Constructors
	WCFontManager();																				//!< Deny access to default constructor
	WCFontManager(const WCFontManager& mgr);														//!< Deny access to copy constructor
	WCFontManager& operator=(const WCFontManager& mgr);												//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCFontManager(const std::string &manifest, const std::string &directory, const bool &verbose=false);//!< Initialize the manager with a manifest
	~WCFontManager();																				//!< Default destructor
	
	//Font Access Methods
	WCFont* FontFromName(const std::string &name);													//!< Get a font from a name
};


/***********************************************~***************************************************/


}	   //End Wildcat Namespace
#endif //__TOOLBAR_MANAGER_H__

