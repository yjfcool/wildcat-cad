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
#include <Scene/scrollbar.h>
#include <Scene/user_interface_layer.h>


//Locall Defined Values
#define SCROLLBAR_SCROLLSENSITIVITY				0.05


//Vertical scrollbar texture parts
#define VSCROLLER_TEX_BACKTOP					0
#define VSCROLLER_TEX_BACKMIDDLE				1
#define VSCROLLER_TEX_BACKUP					2
#define VSCROLLER_TEX_BACKUPSELECTED			3
#define VSCROLLER_TEX_BACKDOWN					4
#define VSCROLLER_TEX_BACKDOWNSELECTED			5
#define VSCROLLER_TEX_KNOBTOP					6
#define VSCROLLER_TEX_KNOBMIDDLE				7
#define VSCROLLER_TEX_KNOBBOTTOM				8


//Horizontal scrollbar texture parts
#define HSCROLLER_TEX_BACKLEFT					0
#define HSCROLLER_TEX_BACKMIDDLE				1
#define HSCROLLER_TEX_BACKRIGHTLEFT				2
#define HSCROLLER_TEX_BACKRIGHTLEFTSELECTED		3
#define HSCROLLER_TEX_BACKRIGHTRIGHT			4
#define HSCROLLER_TEX_BACKRIGHTRIGHTSELECTED	5
#define HSCROLLER_TEX_KNOBLEFT					6
#define HSCROLLER_TEX_KNOBMIDDLE				7
#define HSCROLLER_TEX_KNOBRIGHT					8

    
/***********************************************~***************************************************/


WCScrollbar::WCScrollbar(WCWidget *parent, WPFloat extent, WPFloat position) : ::WCWidget(parent->Layer()),
	_perfLevel(PerformanceLow), _parent(parent), _extent(extent), _position(position) {
	if(WCAdapter::HasGLARBVertexBufferObject()) {
		//Set performance level to medium
		this->_perfLevel = PerformanceMedium;
		//Generate buffers
		glGenBuffers(9, this->_vertBuffers);
		glGenBuffers(9, this->_texBuffers);
	}
}


WCScrollbar::~WCScrollbar() {
	if(this->_perfLevel == PerformanceMedium) {
		//Delete the buffers
		glDeleteBuffers(9, this->_vertBuffers);
		glDeleteBuffers(9, this->_texBuffers);
	}
}


void WCScrollbar::Extent(const WPFloat extent) {
	//Set the extent
	this->_extent = extent;
	//Check if extent = 1.0, reset the position
	if (this->_extent >= 1.0) this->_position = 0.0;
	//Mark as dirty
	this->_isDirty = true;
}


/***********************************************~***************************************************/


void WCVerticalScrollbar::GenerateBuffers(void) {
	if(this->_perfLevel == PerformanceMedium) {
		//Setup arrays for vertex and texCoord data
		GLfloat verts[8];
		GLfloat texCoords[8];

		/*** Top of Back ***/
		verts[0] = (GLfloat)this->_xMin;
		verts[1] = (GLfloat)(this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J());
		verts[2] = verts[0];
		verts[3] = (GLfloat)this->_yMax;
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_BACKTOP].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKTOP].K();
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKTOP].L();
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKTOP].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKTOP].L();
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_BACKTOP]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), verts, GL_STATIC_DRAW);
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_BACKTOP]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		/*** Middle of Back ***/
		verts[0] = (GLfloat)this->_xMin;
		verts[1] = (GLfloat)(this->_yMin + this->_texSizes[VSCROLLER_TEX_BACKUP].J() + this->_texSizes[VSCROLLER_TEX_BACKDOWN].J());
		verts[2] = verts[0];
		verts[3] = (GLfloat)(this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J());
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_BACKMIDDLE].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];
		GLfloat height = (GLfloat)((verts[3] - verts[1]) / SCREEN_PIXEL_WIDTH);
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKMIDDLE].K();
		texCoords[1] = (GLfloat)height;
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKMIDDLE].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)height;
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_BACKMIDDLE]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), verts, GL_STATIC_DRAW);
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_BACKMIDDLE]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		/*** Up Arrow of Back ***/
		verts[0] = (GLfloat)this->_xMin;
		verts[1] = (GLfloat)(this->_yMin + this->_texSizes[VSCROLLER_TEX_BACKDOWN].J());
		verts[2] = verts[0];
		verts[3] = verts[1] + (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKUP].J();
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_BACKUP].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKUP].K();
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKUP].L();
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKUP].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKUP].L();
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_BACKUP]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), verts, GL_STATIC_DRAW);
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_BACKUP]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		/*** Down Arrow of Back ***/
		verts[0] = (GLfloat)this->_xMin;
		verts[1] = (GLfloat)this->_yMin;
		verts[2] = verts[0];
		verts[3] = verts[1] + (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKDOWN].J();
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_BACKDOWN].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKDOWN].K();
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKDOWN].L();
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKDOWN].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKDOWN].L();
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_BACKDOWN]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), verts, GL_STATIC_DRAW);
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_BACKDOWN]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		/*** Slider Calculations ***/
		WPFloat offset = 8 * SCREEN_PIXEL_WIDTH;
		WPFloat scrollHeight = (this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J() + offset) - 
			(this->_yMin + this->_texSizes[VSCROLLER_TEX_BACKDOWN].J() + this->_texSizes[VSCROLLER_TEX_BACKUP].J() - offset);
		WPFloat sliderHeight = this->_extent * scrollHeight;
		WPFloat sliderDiff = scrollHeight - sliderHeight;
		WPFloat topOfSlider = this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J() + offset - (sliderDiff * this->_position);

		/*** Top of Slider ***/
		verts[0] = (GLfloat)(this->_xMin + (2.0 *SCREEN_PIXEL_WIDTH));
		verts[1] = (GLfloat)(topOfSlider - this->_texSizes[VSCROLLER_TEX_KNOBTOP].J());
		verts[2] = verts[0];
		verts[3] = verts[1] + (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBTOP].J();
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_KNOBTOP].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBTOP].K();
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBTOP].L();
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBTOP].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBTOP].L();
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_KNOBTOP]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), verts, GL_STATIC_DRAW);
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_KNOBTOP]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		/*** Middle of Slider ***/
		verts[0] = (GLfloat)(this->_xMin + (2.0 *SCREEN_PIXEL_WIDTH));
		verts[1] = (GLfloat)(topOfSlider - sliderHeight + this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].J());
		verts[2] = verts[0];
		verts[3] = (GLfloat)(topOfSlider - this->_texSizes[VSCROLLER_TEX_KNOBTOP].J());
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].K();
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].L();
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].L();
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_KNOBMIDDLE]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), verts, GL_STATIC_DRAW);
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_KNOBMIDDLE]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		/*** Bottom of Slider ***/
		verts[0] = (GLfloat)(this->_xMin + (2.0 *SCREEN_PIXEL_WIDTH));
		verts[1] = (GLfloat)(topOfSlider - sliderHeight);
		verts[2] = verts[0];
		verts[3] = verts[1] + (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].J();
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].K();
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].L();
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].L();
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_KNOBBOTTOM]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), verts, GL_STATIC_DRAW);
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_KNOBBOTTOM]);
		glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		/*** Clean Up ***/

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		this->_isDirty = false;
	}
	else {
		//Setup arrays for vertex and texCoord data
		GLfloat verts[8];
		GLfloat texCoords[8];

		/*** Slider Calculations ***/
		WPFloat offset = 8 * SCREEN_PIXEL_WIDTH;
		WPFloat scrollHeight = (this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J() + offset) - 
			(this->_yMin + this->_texSizes[VSCROLLER_TEX_BACKDOWN].J() + this->_texSizes[VSCROLLER_TEX_BACKUP].J() - offset);
		WPFloat sliderHeight = this->_extent * scrollHeight;
		WPFloat sliderDiff = scrollHeight - sliderHeight;
		WPFloat topOfSlider = this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J() + offset - (sliderDiff * this->_position);

		//Set the color
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		/*** Top of Back ***/
		verts[0] = (GLfloat)this->_xMin;
		verts[1] = (GLfloat)(this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J());
		verts[2] = verts[0];
		verts[3] = (GLfloat)this->_yMax;
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_BACKTOP].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKTOP].K()/40.0;
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKTOP].L()/40.0;
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKTOP].K()/40.0;
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKTOP].L()/40.0;

		//Enable the texture for use
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, this->_textures[VSCROLLER_TEX_BACKTOP]);
		//Setup vertex array
		glVertexPointer(2, GL_FLOAT, 0, verts);
		//Setup texCoord array
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Middle of Back ***/
		verts[0] = (GLfloat)this->_xMin;
		verts[1] = (GLfloat)(this->_yMin + this->_texSizes[VSCROLLER_TEX_BACKUP].J() + this->_texSizes[VSCROLLER_TEX_BACKDOWN].J());
		verts[2] = verts[0];
		verts[3] = (GLfloat)(this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J());
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_BACKMIDDLE].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];
//		GLfloat height = (GLfloat)((verts[3] - verts[1]) / SCREEN_PIXEL_WIDTH);
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKMIDDLE].K();
		texCoords[1] = 1.0;
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKMIDDLE].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = 10.0;


		//Enable the texture for use
		glBindTexture(GL_TEXTURE_2D, this->_textures[VSCROLLER_TEX_BACKMIDDLE]);
		//Setup vertex array
		glVertexPointer(2, GL_FLOAT, 0, verts);
		//Setup texCoord array
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		//Draw top of back
		glColor4ub(255, 255, 255, 255);
		glDrawArrays(GL_QUADS, 0, 4);

		glBegin(GL_TRIANGLES);
		glTexCoord2f(0, 0);
		glVertex3d(0, 0, 0);
		glTexCoord2f(1.0, 0);
		glVertex3d(4, 0, 0);
		glTexCoord2f(1.0, 1.0);
		glVertex3d(4, 4, 0);
		glEnd();

		/*** Up Arrow of Back ***/
		verts[0] = (GLfloat)this->_xMin;
		verts[1] = (GLfloat)(this->_yMin + this->_texSizes[VSCROLLER_TEX_BACKDOWN].J());
		verts[2] = verts[0];
		verts[3] = verts[1] + (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKUP].J();
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_BACKUP].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKUP].K();
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKUP].L();
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKUP].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKUP].L();

		//Enable the texture for use (depends on state of isUpSelected)
		if (this->_isUpSelected)
			glBindTexture(GL_TEXTURE_2D, this->_textures[VSCROLLER_TEX_BACKUPSELECTED]);
		else
			glBindTexture(GL_TEXTURE_2D, this->_textures[VSCROLLER_TEX_BACKUP]);
		//Setup vertex array
		glVertexPointer(2, GL_FLOAT, 0, verts);
		//Setup texCoord array
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Down Arrow of Back ***/
		verts[0] = (GLfloat)this->_xMin;
		verts[1] = (GLfloat)this->_yMin;
		verts[2] = verts[0];
		verts[3] = verts[1] + (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKDOWN].J();
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_BACKDOWN].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKDOWN].K();
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKDOWN].L();
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKDOWN].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_BACKDOWN].L();

		//Enable the texture for use (depends on state of isDownSelected)
		if (this->_isDownSelected)
			glBindTexture(GL_TEXTURE_2D, this->_textures[VSCROLLER_TEX_BACKDOWNSELECTED]);
		else 
			glBindTexture(GL_TEXTURE_2D, this->_textures[VSCROLLER_TEX_BACKDOWN]);
		//Setup vertex array
		glVertexPointer(2, GL_FLOAT, 0, verts);
		//Setup texCoord array
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Top of Slider ***/
		verts[0] = (GLfloat)(this->_xMin + (2.0 *SCREEN_PIXEL_WIDTH));
		verts[1] = (GLfloat)(topOfSlider - this->_texSizes[VSCROLLER_TEX_KNOBTOP].J());
		verts[2] = verts[0];
		verts[3] = verts[1] + (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBTOP].J();
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_KNOBTOP].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBTOP].K();
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBTOP].L();
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBTOP].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBTOP].L();

		//Enable the texture for use
		glBindTexture(GL_TEXTURE_2D, this->_textures[VSCROLLER_TEX_KNOBTOP]);
		//Setup vertex array
		glVertexPointer(2, GL_FLOAT, 0, verts);
		//Setup texCoord array
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Middle of Slider ***/
		verts[0] = (GLfloat)(this->_xMin + (2.0 *SCREEN_PIXEL_WIDTH));
		verts[1] = (GLfloat)(topOfSlider - sliderHeight + this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].J());
		verts[2] = verts[0];
		verts[3] = (GLfloat)(topOfSlider - this->_texSizes[VSCROLLER_TEX_KNOBTOP].J());
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].K();
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].L();
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].L();

		//Enable the texture for use
		glBindTexture(GL_TEXTURE_2D, this->_textures[VSCROLLER_TEX_KNOBMIDDLE]);
		//Setup vertex array
		glVertexPointer(2, GL_FLOAT, 0, verts);
		//Setup texCoord array
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Bottom of Slider ***/
		verts[0] = (GLfloat)(this->_xMin + (2.0 *SCREEN_PIXEL_WIDTH));
		verts[1] = (GLfloat)(topOfSlider - sliderHeight);
		verts[2] = verts[0];
		verts[3] = verts[1] + (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].J();
		verts[4] = (GLfloat)(this->_xMin + this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].I());
		verts[5] = verts[3];
		verts[6] = verts[4];
		verts[7] = verts[1];	
		texCoords[0] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].K();
		texCoords[1] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].L();
		texCoords[2] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].K();
		texCoords[3] = 0.0f;
		texCoords[4] = 0.0f;
		texCoords[5] = 0.0f;
		texCoords[6] = 0.0f;
		texCoords[7] = (GLfloat)this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].L();

		//Enable the texture for use
		glBindTexture(GL_TEXTURE_2D, this->_textures[VSCROLLER_TEX_KNOBBOTTOM]);
		//Setup vertex array
		glVertexPointer(2, GL_FLOAT, 0, verts);
		//Setup texCoord array
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		//Make sure that vertex and normal arrays are disabled
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
		glDisable(GL_TEXTURE_2D);
	}
}


/***********************************************~***************************************************/


WCVerticalScrollbar::WCVerticalScrollbar(WCWidget *parent, const WPFloat &extent, const WPFloat &position) : 
	::WCScrollbar(parent, extent, position), _isSliderSelected(false), _isDownSelected(false), _isUpSelected(false), _mark() {

	//Load Top of Back
	WSTexture* tex = this->_parent->Layer()->Scene()->TextureManager()->TextureFromName("blkScrollerBackVT");
	this->_textures[VSCROLLER_TEX_BACKTOP] = tex->_id;
	if(WCAdapter::HasGL15())this->_texSizes[VSCROLLER_TEX_BACKTOP].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width, tex->_height);
	else this->_texSizes[VSCROLLER_TEX_BACKTOP].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width / tex->_texture_width, tex->_height / tex->_texture_height);
	//Load Middle of Back
	tex = this->_parent->Layer()->Scene()->TextureManager()->TextureFromName("blkScrollerBackVM");
	this->_textures[VSCROLLER_TEX_BACKMIDDLE] = tex->_id;
	if(WCAdapter::HasGL15())this->_texSizes[VSCROLLER_TEX_BACKMIDDLE].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width, tex->_height);
	else this->_texSizes[VSCROLLER_TEX_BACKMIDDLE].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width / tex->_texture_width, tex->_height / tex->_texture_height);
	//Load Back Up
	tex = this->_parent->Layer()->Scene()->TextureManager()->TextureFromName("blkScrollerBackVBUp");
	this->_textures[VSCROLLER_TEX_BACKUP] = tex->_id;
	if(WCAdapter::HasGL15())this->_texSizes[VSCROLLER_TEX_BACKUP].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width, tex->_height);
	else this->_texSizes[VSCROLLER_TEX_BACKUP].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width / tex->_texture_width, tex->_height / tex->_texture_height);
	//Load Back Up Selected
	tex = this->_parent->Layer()->Scene()->TextureManager()->TextureFromName("blkScrollerBackVBUpSelected");
	this->_textures[VSCROLLER_TEX_BACKUPSELECTED] = tex->_id;
	if(WCAdapter::HasGL15())this->_texSizes[VSCROLLER_TEX_BACKUPSELECTED].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width, tex->_height);
	else this->_texSizes[VSCROLLER_TEX_BACKUPSELECTED].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width / tex->_texture_width, tex->_height / tex->_texture_height);
	//Load Back Down
	tex = this->_parent->Layer()->Scene()->TextureManager()->TextureFromName("blkScrollerBackVBDown");
	this->_textures[VSCROLLER_TEX_BACKDOWN] = tex->_id;
	if(WCAdapter::HasGL15())this->_texSizes[VSCROLLER_TEX_BACKDOWN].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width, tex->_height);
	else this->_texSizes[VSCROLLER_TEX_BACKDOWN].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width / tex->_texture_width, tex->_height / tex->_texture_height);
	//Load Back Down Selected
	tex = this->_parent->Layer()->Scene()->TextureManager()->TextureFromName("blkScrollerBackVBDownSelected");
	this->_textures[VSCROLLER_TEX_BACKDOWNSELECTED] = tex->_id;
	if(WCAdapter::HasGL15())this->_texSizes[VSCROLLER_TEX_BACKDOWNSELECTED].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width, tex->_height);
	else this->_texSizes[VSCROLLER_TEX_BACKDOWNSELECTED].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width / tex->_texture_width, tex->_height / tex->_texture_height);
	//Load Slider Top
	tex = this->_parent->Layer()->Scene()->TextureManager()->TextureFromName("blkScrollerKnobVT");
	this->_textures[VSCROLLER_TEX_KNOBTOP] = tex->_id;
	if(WCAdapter::HasGL15())this->_texSizes[VSCROLLER_TEX_KNOBTOP].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width, tex->_height);
	else this->_texSizes[VSCROLLER_TEX_KNOBTOP].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width / tex->_texture_width, tex->_height / tex->_texture_height);
	//Load Slider Middle
	tex = this->_parent->Layer()->Scene()->TextureManager()->TextureFromName("blkScrollerKnobVM");
	this->_textures[VSCROLLER_TEX_KNOBMIDDLE] = tex->_id;
	if(WCAdapter::HasGL15())this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width, tex->_height);
	else this->_texSizes[VSCROLLER_TEX_KNOBMIDDLE].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width / tex->_texture_width, tex->_height / tex->_texture_height);
	//Load Slider Bottom
	tex = this->_parent->Layer()->Scene()->TextureManager()->TextureFromName("blkScrollerKnobVB");
	this->_textures[VSCROLLER_TEX_KNOBBOTTOM] = tex->_id;
	if(WCAdapter::HasGL15())this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width, tex->_height);
	else this->_texSizes[VSCROLLER_TEX_KNOBBOTTOM].Set(tex->_width * SCREEN_PIXEL_WIDTH, tex->_height * SCREEN_PIXEL_WIDTH, tex->_width / tex->_texture_width, tex->_height / tex->_texture_height);
}


WCVerticalScrollbar::~WCVerticalScrollbar() {
	//Nothing to do - TextureManager will delete the textures
}


void WCVerticalScrollbar::IsVisible(const bool state) {
	//See if this is a change
	if (this->_isVisible != state) {
		this->_isVisible = state;
		//Mark as dirty
		this->_isDirty = true;
	}
}


void WCVerticalScrollbar::OnReshape(void) {
	//Resize according to parent
	this->_xMin = this->_parent->XMin();
	this->_yMin = this->_parent->YMin();
	this->_width = this->_texSizes[VSCROLLER_TEX_BACKTOP].J();
	this->_height = this->_parent->Height();
	this->_xMax = this->_xMin + this->_width;
	this->_yMax = this->_parent->YMax();
}


void WCVerticalScrollbar::OnMouseDown(const WCMouseButton &button, const WPFloat x, const WPFloat y) {
	//Slider calculations
	WPFloat offset = 8 * SCREEN_PIXEL_WIDTH;
	WPFloat scrollHeight = (this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J() + offset) - 
						   (this->_yMin + this->_texSizes[VSCROLLER_TEX_BACKDOWN].J() + this->_texSizes[VSCROLLER_TEX_BACKUP].J() - offset);
	WPFloat sliderHeight = this->_extent * scrollHeight;
	WPFloat sliderDiff = scrollHeight - sliderHeight;
	WPFloat topOfSlider = this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J() + offset - (sliderDiff * this->_position);
	
	//Slider case
	if ((y <= topOfSlider) && (y >= topOfSlider - sliderHeight)) {
		//Mark the down spot
		this->_mark.Set(x, y, this->_position);
		this->_isSliderSelected = true;
	}
	//Middle of back case
	else if ((y < this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J() + offset) &&
			(y > this->_yMin + this->_texSizes[VSCROLLER_TEX_BACKUP].J() + this->_texSizes[VSCROLLER_TEX_BACKDOWN].J() - offset)) {
		//See if above slider
		if (y > topOfSlider) {
			//Scroll the slider up
			this->_position -= 0.1 / sliderDiff;
			this->_position = STDMAX(this->_position, 0.0);
		}
		//Must be below slider
		else {
			//Scroll the slider down
			this->_position += 0.1 / sliderDiff;
			this->_position = STDMIN(this->_position, 1.0);
		}
	}
	//Up arrow of back case
	else if (y >= this->_yMin + this->_texSizes[VSCROLLER_TEX_BACKDOWN].J() - (2*SCREEN_PIXEL_WIDTH)) {
		//Scroll the slider up
		this->_position -= 0.1 / sliderDiff;
		this->_position = STDMAX(this->_position, 0.0);
		//Mark that the up button is selected
		this->_isUpSelected = true;
	}
	//Otherwise in down arrow case
	else {
		//Scroll the slider down
		this->_position += 0.1 / sliderDiff;
		this->_position = STDMIN(this->_position, 1.0);
		//Mark that the down button is selected
		this->_isDownSelected = true;
	}
	//Mark this and the parent as dirty
	this->_isDirty = true;
	this->_parent->MarkDirty();
}


void WCVerticalScrollbar::OnMouseUp(const WCMouseButton &button, const WPFloat x, const WPFloat y) {
	//Clear all of the selection values
	if (this->_isSliderSelected) {
		this->_isSliderSelected = false;
	}
	if (this->_isUpSelected) {
		this->_isUpSelected = false;
		this->_isDirty = true;
	}
	if (this->_isDownSelected) {
		this->_isDownSelected = false;
		this->_isDirty = true;
	}
}


void WCVerticalScrollbar::OnMouseMove(const WPFloat x, const WPFloat y) {
	//Check if scrolling
	if (this->_isSliderSelected) {
		//Slider calculations
		WPFloat offset = 8 * SCREEN_PIXEL_WIDTH;
		WPFloat scrollHeight = (this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J() + offset) - 
							   (this->_yMin + this->_texSizes[VSCROLLER_TEX_BACKDOWN].J() + this->_texSizes[VSCROLLER_TEX_BACKUP].J() - offset);
		WPFloat sliderHeight = this->_extent * scrollHeight;
		WPFloat sliderDiff = scrollHeight - sliderHeight;
		//Adjust the position
		WPFloat diff = (y - this->_mark.J());
		this->_position = this->_mark.K() - (diff) / sliderDiff;
		//Bounds check position
		this->_position = STDMAX(this->_position, 0.0);
		this->_position = STDMIN(this->_position, 1.0);
		//Mark as dirty
		this->_isDirty = true;
		this->_parent->MarkDirty();
	}
}


void WCVerticalScrollbar::OnScroll(const WPFloat &x, const WPFloat &y) {
	//Use use y events
	if (fabs(y) < 0.01) return;
	//Slider calculations
	WPFloat offset = 8 * SCREEN_PIXEL_WIDTH;
	WPFloat scrollHeight = (this->_yMax - this->_texSizes[VSCROLLER_TEX_BACKTOP].J() + offset) - 
						   (this->_yMin + this->_texSizes[VSCROLLER_TEX_BACKDOWN].J() + this->_texSizes[VSCROLLER_TEX_BACKUP].J() - offset);
	WPFloat sliderHeight = this->_extent * scrollHeight;
	WPFloat sliderDiff = scrollHeight - sliderHeight;
	//Calculate new position (desensitize y by 8)
	this->_position -= (y * SCROLLBAR_SCROLLSENSITIVITY) / sliderDiff;
	//Bounds check position
	this->_position = STDMAX(this->_position, 0.0);
	this->_position = STDMIN(this->_position, 1.0);
	//Mark as dirty
	this->_isDirty = true;
	this->_parent->MarkDirty();
}


void WCVerticalScrollbar::Render(void) {
	//Only render if visible
	if (!this->_isVisible) return;
	if(this->_perfLevel == PerformanceMedium) {
		//Generate buffers if necessary
		if (this->_isDirty) this->GenerateBuffers();

		//Set the color
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		/*** Top of Back ***/
		//Enable the texture for use
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_textures[VSCROLLER_TEX_BACKTOP]);
		//Setup vertex array
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_BACKTOP]);
		glVertexPointer(2, GL_FLOAT, 0, 0);
		//Setup texCoord array
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_BACKTOP]);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Middle of Back ***/
		//Enable the texture for use
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_textures[VSCROLLER_TEX_BACKMIDDLE]);
		//Setup vertex array
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_BACKMIDDLE]);
		glVertexPointer(2, GL_FLOAT, 0, 0);
		//Setup texCoord array
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_BACKMIDDLE]);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Up Arrow of Back ***/
		//Enable the texture for use (depends on state of isUpSelected)
		if (this->_isUpSelected)
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_textures[VSCROLLER_TEX_BACKUPSELECTED]);
		else
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_textures[VSCROLLER_TEX_BACKUP]);
		//Setup vertex array
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_BACKUP]);
		glVertexPointer(2, GL_FLOAT, 0, 0);
		//Setup texCoord array
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_BACKUP]);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Down Arrow of Back ***/
		//Enable the texture for use (depends on state of isDownSelected)
		if (this->_isDownSelected)
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_textures[VSCROLLER_TEX_BACKDOWNSELECTED]);
		else 
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_textures[VSCROLLER_TEX_BACKDOWN]);
		//Setup vertex array
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_BACKDOWN]);
		glVertexPointer(2, GL_FLOAT, 0, 0);
		//Setup texCoord array
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_BACKDOWN]);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Top of Slider ***/
		//Enable the texture for use
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_textures[VSCROLLER_TEX_KNOBTOP]);
		//Setup vertex array
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_KNOBTOP]);
		glVertexPointer(2, GL_FLOAT, 0, 0);
		//Setup texCoord array
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_KNOBTOP]);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Middle of Slider ***/
		//Enable the texture for use
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_textures[VSCROLLER_TEX_KNOBMIDDLE]);
		//Setup vertex array
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_KNOBMIDDLE]);
		glVertexPointer(2, GL_FLOAT, 0, 0);
		//Setup texCoord array
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_KNOBMIDDLE]);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Bottom of Slider ***/
		//Enable the texture for use
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_textures[VSCROLLER_TEX_KNOBBOTTOM]);
		//Setup vertex array
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffers[VSCROLLER_TEX_KNOBBOTTOM]);
		glVertexPointer(2, GL_FLOAT, 0, 0);
		//Setup texCoord array
		glBindBuffer(GL_ARRAY_BUFFER, this->_texBuffers[VSCROLLER_TEX_KNOBBOTTOM]);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
		//Draw top of back
		glDrawArrays(GL_QUADS, 0, 4);

		/*** Clean Up ***/
		//Bind back to nothing
		glBindBuffer(GL_ARRAY_BUFFER, 0);	
		//Make sure that vertex and normal arrays are disabled
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
	}
	else {
		this->GenerateBuffers();
	}
	
	//Check for errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVerticalScrollbar::Render Error - GL error at Exit.");
}


/***********************************************~***************************************************/

