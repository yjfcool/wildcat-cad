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
#include "Scene/text.h"
#include "Scene/scene.h"
#include "Application/Win32/font_manager.h"


/***********************************************~***************************************************/


// Win32 platform does not use one texture per string, so no need to gen texture.  Instead,
// uses FreeType to gen texture per char and puts these into an atlas.  Drawing just renders
// each char texture.  See DrawX2 functions below.
void WCText::GenerateTexture(void) {
	//Make sure scene in present
	if (this->_scene == NULL) return;
	//Get the font reference
	this->_font = this->_scene->FontManager()->FontFromName("palatino_16");
	//Check to make sure we got a font
	if (this->_font == NULL) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCText::GenerateTexture - Not able to get font reference");
		return;
	}

	//The the size of the entire text
	this->_font->StringSize(this->_text, this->_texWidth, this->_texHeight);
	//Mark as clean
	this->_isDirty = false;
}


void WCText::DrawAtPoint(const GLfloat &x, const GLfloat &y) {
	//Generate texture if needed
	if (this->_isDirty) this->GenerateTexture();
	//Make sure we have a font
	if (this->_font == NULL) return;
	GLuint *textures = this->_font->Textures();
	WSFontCharSize *sizes = this->_font->Sizes(); 

	//Enable GL parameters
	glEnable(GL_TEXTURE_RECTANGLE_ARB);	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//Turn on the color
	this->_color.Enable();

	//Setup some data
	GLfloat xPos = x, width, height;
	GLfloat vertData[8];
	GLfloat texData[8];
	//Get the c-string
	const char* cStr = this->_text.c_str();
	unsigned int length = this->_text.length();
	//Loop through all chars
	for (unsigned int i=0; i < length; i++) {
		//Get char width and height
		width = sizes[ cStr[i] ].width;
		height = sizes[ cStr[i] ].height;
		//Determine tex data
		texData[0] = 0.0;
		texData[1] = height;
		texData[2] = 0.0;
		texData[3] = 0.0;
		texData[4] = width;
		texData[5] = 0.0;
		texData[6] = width;
		texData[7] = height;
		//Change to screen sizes
		width *= SCREEN_PIXEL_WIDTH;
		height *= SCREEN_PIXEL_WIDTH;
		//Determine vert data
		vertData[0] = xPos;
		vertData[1] = y;
		vertData[2] = xPos;
		vertData[3] = y + height;
		vertData[4] = xPos + width;
		vertData[5] = vertData[3];
		vertData[6] = vertData[4];
		vertData[7] = y;
		//Set texture and pointers
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textures[ cStr[i] ]);
		glVertexPointer(2, GL_FLOAT, 0, vertData);
		glTexCoordPointer(2, GL_FLOAT, 0, texData);
		//Draw textured quad
		glDrawArrays(GL_QUADS, 0, 4);
		//Increment xPos
		xPos = vertData[4];
	}

	//Make sure that vertex and normal arrays are disabled
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
	//Clean up
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
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

