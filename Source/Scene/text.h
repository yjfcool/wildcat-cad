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


#ifndef __TEXT_H__
#define __TEXT_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCFont;


/***********************************************~***************************************************/


//Pre-Defined Font Types
#define TEXTSTYLE_ROMAN							1
#define TEXTSTYLE_BOLD							2
#define TEXTSTYLE_ITALIC						3

class WCTextStyle {
private:
	unsigned int								_style;												//!< Text style
	//Deny Access
	WCTextStyle();																					//!< Deny access to default constructor
	WCTextStyle(unsigned int style) : _style(style) { }												//!< Primary hidden constructor
	friend class WCText;
public:
	~WCTextStyle()								{ }													//!< Default destructor
	static WCTextStyle Roman(void)				{ return WCTextStyle(TEXTSTYLE_ROMAN); }			//!< Roman constructor
	static WCTextStyle Bold(void)				{ return WCTextStyle(TEXTSTYLE_BOLD); }				//!< Bold constructor
	static WCTextStyle Italic(void)				{ return WCTextStyle(TEXTSTYLE_ITALIC); }			//!< Italic constructor
};


/***********************************************~***************************************************/


//Pre-Defined Font Types
#define TEXTFONT_TIMES							1
#define TEXTFONT_PALATINO						2
#define TEXTFONT_HELVETICA						3
#define TEXTFONT_TAHOMA							4

class WCTextFont {
private:
	unsigned int								_type;												//!< Font type
	//Deny Access
	WCTextFont();																					//!< Deny access to default constructor
	WCTextFont(unsigned int type) : _type(type)	{ }													//!< Primary hidden constructor
	friend class WCText;
public:
	~WCTextFont()								{ }													//!< Default destructor
	static WCTextFont Helvetica(void)			{ return WCTextFont(TEXTFONT_HELVETICA); }			//!< Helvetica constructor
	static WCTextFont Palatino(void)			{ return WCTextFont(TEXTFONT_PALATINO); }			//!< Palatino constructor
	static WCTextFont Tahoma(void)				{ return WCTextFont(TEXTFONT_TAHOMA); }				//!< Helvetica constructor
	static WCTextFont Times(void)				{ return WCTextFont(TEXTFONT_TIMES); }				//!< Times constructor

};


/***********************************************~***************************************************/


class WCText {
private:
	WCScene										*_scene;											//!< Associated scene
	std::string									_text;												//!< String of the text
	WCColor										_color;												//!< Color of the text
	std::string									_fontName;											//!< Name of the font
	float										_fontSize;											//!< Font size
	bool										_isUnderlined;										//!< Is the text underlined
	bool										_isDirty;											//!< Dirty flag
	WCFont										*_font;												//!< Point to font object (non-OSX only)
	GLuint										_texture;											//!< Texture for text
	GLfloat										_texWidth, _texHeight;								//!< Texture size info (generated)
	//Private Methods
	void GenerateTexture(void);																		//!< Generate the text texture
	//Hidden Constructors
	WCText();																						//!< Deny access to default constructor
	WCText(const WCText&);																			//!< Deny access to copy constructor
	WCText& operator=(const WCText&);																//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCText(WCScene *scene, const std::string &text, const WCColor &color,							//!< Primary constructor
												const WCTextFont &textFont, const WCTextStyle &textStyle, const float &size);
	~WCText();																						//!< Default destructor
	
	//General Access Methods
	inline GLfloat Width(void)					{ return (GLfloat)(this->_texWidth * SCREEN_PIXEL_WIDTH); }	//!< Get the width of the text
	inline GLfloat Height(void)					{ return (GLfloat)(this->_texHeight * SCREEN_PIXEL_WIDTH); }//!< Get the height of the text
	inline std::string Text(void)				{ return this->_text; }								//!< Get the text string
	void Text(const std::string &text);																//!< Set the text string
	inline WCColor Color(void)					{ return this->_color; }							//!< Get the text color
	void Color(const WCColor &color);																//!< Set the text color
	inline float FontSize(void)					{ return this->_fontSize; }							//!< Get the text font size
	void FontSize(const float &fontSize);															//!< Set the text font size
	inline bool IsUnderlined(void) const		{ return this->_isUnderlined; }						//!< Get the text underlined state
	void IsUnderlined(const bool &state);															//!< Set the text underlined state
	
	//Drawing Methods
	void DrawAtPoint(const GLfloat &x, const GLfloat &y);											//!< Draw text at a certain 2D point
	void DrawAtPoint(const WCVector4 &pt, const WCVector4 &uUnit=WCVector4(1.0, 0.0, 0.0, 0.0),		//!< Draw text at an aligned 3D point
										  const WCVector4 &vUnit=WCVector4(0.0, 1.0, 0.0, 0.0));
};


/***********************************************~***************************************************/


#endif //__TEXT_H__

