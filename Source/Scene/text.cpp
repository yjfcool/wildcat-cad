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


/***********************************************~***************************************************/


WCText::WCText(const std::string &text, const WCColor &color, const WCTextFont &textFont, const WCTextStyle &textStyle, 
	const float &fontSize) : _text(text), _color(color), _fontName(""), _fontSize(fontSize),
	_isUnderlined(false), _isBacked(false), _isDirty(true), _texture(0), _texWidth(0.0), _texHeight(0.0) {

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


void WCText::DrawAtPoint(const GLfloat &x, const GLfloat &y) {
	//Generate texture if needed
	if (this->_isDirty) this->GenerateTexture();
	//If there is a texture, draw with it
	if (this->_texture != 0) {
		//Determine size of texture
		GLfloat width = (GLfloat)(this->_texWidth * SCREEN_PIXEL_WIDTH);
		GLfloat height = (GLfloat)(this->_texHeight * SCREEN_PIXEL_WIDTH);
		GLfloat vertData[8] = {x, y, x, y+height, x+width, y+height, x+width, y };
		GLfloat texData[8] = {0.0, this->_texHeight, 0.0, 0.0, this->_texWidth, 0.0, this->_texWidth, this->_texHeight };

		//Enable the texture for use
		glEnable(GL_TEXTURE_RECTANGLE_ARB);	
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_texture);
		//Setup vertex array
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertData);
		//Setup texture array
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texData);

		//Draw textured quad
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glDrawArrays(GL_QUADS, 0, 4);

		//Make sure that vertex and normal arrays are disabled
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
		//Clean up
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
	}
}


void WCText::DrawAtPoint(const WCVector4 &pt, const WCVector4 &uUnit, const WCVector4 &vUnit) {
	//Generate texture if needed
	if (this->_isDirty) this->GenerateTexture();
	//If there is a texture, draw with it
	if (this->_texture != 0) {
		//Determine size of texture
		GLfloat width = (GLfloat)(this->_texWidth * SCREEN_PIXEL_WIDTH);
		GLfloat height = (GLfloat)(this->_texHeight * SCREEN_PIXEL_WIDTH);
		//Find rect corners
		WCVector4 ul = pt + (vUnit * height);
		WCVector4 ur = pt + (vUnit * height) + (uUnit * width);
		WCVector4 lr = pt + (uUnit * width);
		//Load data
		GLfloat vertData[12] = {(GLfloat)pt.I(), (GLfloat)pt.J(), (GLfloat)pt.K(), 
								(GLfloat)ul.I(), (GLfloat)ul.J(), (GLfloat)ul.K(),
								(GLfloat)ur.I(), (GLfloat)ur.J(), (GLfloat)ur.K(),
								(GLfloat)lr.I(), (GLfloat)lr.J(), (GLfloat)lr.K() };
		GLfloat texData[8] = {0.0, this->_texHeight, 
							  0.0, 0.0, 
							  this->_texWidth, 0.0, 
							  this->_texWidth, this->_texHeight };
		//Set up GL drawing 
		glDisable (GL_DEPTH_TEST);
		//Enable the texture for use
		glEnable(GL_TEXTURE_RECTANGLE_ARB);	
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_texture);
		//Setup vertex array
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertData);
		//Setup texture array
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texData);

		//Draw textured quad
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glDrawArrays(GL_QUADS, 0, 4);
		//Do we need to draw the backside also
		if (this->_isBacked) {
			//Load data
			GLfloat backVertData[12] = {(GLfloat)lr.I(), (GLfloat)lr.J(), (GLfloat)lr.K(),
										(GLfloat)ur.I(), (GLfloat)ur.J(), (GLfloat)ur.K(),
										(GLfloat)ul.I(), (GLfloat)ul.J(), (GLfloat)ul.K(),
										(GLfloat)pt.I(), (GLfloat)pt.J(), (GLfloat)pt.K() };
			GLfloat backTexData[8] = {0.0, this->_texHeight, 
									  0.0, 0.0, 
									  this->_texWidth, 0.0, 
									  this->_texWidth, this->_texHeight };			
			//Update pointers
			glVertexPointer(3, GL_FLOAT, 0, backVertData);		
			glTexCoordPointer(2, GL_FLOAT, 0, backTexData);
			//Draw again
			glDrawArrays(GL_QUADS, 0, 4);
		}
		
		//Make sure that vertex and normal arrays are disabled
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
		//Clean up
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
	}
}


/***********************************************~***************************************************/

