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
//Include WCFont Support for Win32
#ifndef __APPLE__
#include "Application/Win32/font_manager.h"
#endif


/***********************************************~***************************************************/


WCText::WCText(WCScene *scene, const std::string &text, const WCColor &color, const WCTextFont &textFont,
	const WCTextStyle &textStyle, const float &fontSize) : _scene(scene), _text(text), _color(color), _fontName(""),
	_fontSize(fontSize), _isUnderlined(false), _isDirty(true), _isBacked(false), _texture(0), _texWidth(0.0),
	_texHeight(0.0) {

	//Determine font name
	std::string fontName;
	switch(textFont._type) {
		case TEXTFONT_TIMES: fontName = "Times"; break;
		case TEXTFONT_PALATINO: fontName = "Palatino"; break;
		case TEXTFONT_HELVETICA: fontName = "Helvetica"; break;
		case TEXTFONT_TAHOMA: fontName = "Tahoma"; break;
		default: fontName = "Times"; break;
	}
	
	//Determine font style
	std::string fontStyle;
	switch(textStyle._style) {
		case TEXTSTYLE_ROMAN: fontStyle = "Roman"; break;
		case TEXTSTYLE_BOLD: fontStyle = "Bold"; break;
		case TEXTSTYLE_ITALIC: fontStyle = "Italic"; break;
		default: fontStyle = "Roman"; break;
	}
	//Combine font and style
	this->_fontName = fontName + "-" + fontStyle;
	//Generate the textures
	this->GenerateTexture();
}


WCText::~WCText() {
	//Is there a texture to delete
	if (this->_texture != 0) {
		glDeleteTextures(1, &this->_texture);
		//Ensure it is zeroed for failure cases
		this->_texture = 0; 
	}
}


void WCText::Text(const std::string &text) {
	//See if a change has been made
	if (this->_text != text) {
		//Update and mark as dirty
		this->_text = text;
		this->_isDirty = true;
	}
}


void WCText::Color(const WCColor &color) {
	//See if a change has been made
	if (this->_color != color) {
		//Update and mark as dirty
		this->_color = color;
		this->_isDirty = true;
	}
}


void WCText::FontSize(const float &fontSize) {
	//See if a change has been made
	if (this->_fontSize != fontSize) {
	//Update and mark as dirty
		this->_fontSize = fontSize;
		this->_isDirty = true;
	}
}


void WCText::IsUnderlined(const bool &state) {
	//See if a change has been made
	if (this->_isUnderlined != state) {
	//Update and mark as dirty
		this->_isUnderlined = state;
		this->_isDirty = true;
	}
}


/***********************************************~***************************************************/

