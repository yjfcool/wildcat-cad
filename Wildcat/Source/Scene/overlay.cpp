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
#include "Scene/overlay.h"
#include "Scene/user_interface_layer.h"


/*** Locally Defined Values ***/
//None


/***********************************************~***************************************************/


void WCOverlay::GenerateVBO(void) {
	//Find the center of the overlay
//	GLfloat xCenter = (this->_xMax - this->_xMin) / 2.0f;
//	GLfloat yCenter = (this->_yMax - this->_yMin) / 2.0f;
	
	GLfloat data[8]; 
		data[0] = (GLfloat)this->_xMin;
		data[1] = (GLfloat)this->_yMin;
		data[2] = (GLfloat)this->_xMin;
		data[3] = (GLfloat)this->_yMax;
		data[4] = (GLfloat)this->_xMax;
		data[5] = (GLfloat)this->_yMax;
		data[6] = (GLfloat)this->_xMax;
		data[7] = (GLfloat)this->_yMin;
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);	
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts * 2 * sizeof(GLfloat), data, GL_STATIC_DRAW);
/*** Debug ***
	std::cout << "Overlay VBO: " << this->_numVerts << std::endl;	
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d: %f %f\n", i, data2[i*4], data2[i*4+1]);
	glUnmapBuffer(GL_ARRAY_BUFFER);	
/*** Debug ***/	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


/***********************************************~***************************************************/


WCOverlay::WCOverlay(WCUserInterfaceLayer *layer, bool resize) : ::WCWidget(layer), 
	_isMouseOver(false), _color(OVERLAY_DEFAULT_COLOR), _buffer(0), _numVerts(4), _placement(OVERLAY_DEFAULT_PLACEMENT) {
	//Create a VBO
	glGenBuffers(1, &this->_buffer);
}


WCOverlay::~WCOverlay() {
	//Destroy the VBO
	glDeleteBuffers(1, &this->_buffer);
}


void WCOverlay::SetSize(const WPFloat width, const WPFloat height) {
	this->_width = std::max(width, OVERLAY_WIDTH_MIN);
	this->_height = std::max(height, OVERLAY_HEIGHT_MIN);
	this->_xMax = this->_xMin + this->_width;
	this->_yMax = this->_yMin + this->_height;
	//Check placement
	this->SetPlacement(this->_placement);
}


void WCOverlay::SetPlacement(const WPFloat lowerLeft, const WPFloat lowerRight) {
	//Mark the placement as custom
	this->_placement = WCPlacement::Custom();
	//Set the remaining placement variables
	this->_xMin = lowerLeft;
	this->_yMin = lowerRight;
	this->_xMax = this->_xMin + this->_width;
	this->_yMax = this->_yMin + this->_height;
	//Mark as dirty
	this->MarkDirty();	
}


void WCOverlay::SetPlacement(WCPlacement placement) {
	//Copy the placement
	this->_placement = placement;
	//Place in lower left corner
	if (placement == WCPlacement::LowerLeft()) {
		this->_xMin = this->_layer->Scene()->XMin() + OVERLAY_PLACEMENT_BORDER;
		this->_yMin = this->_layer->Scene()->YMin() + OVERLAY_PLACEMENT_BORDER;
	}
	//Place in lower right corner
	else if (placement == WCPlacement::LowerRight()) {
		this->_xMin = this->_layer->Scene()->XMax() - this->_width - OVERLAY_PLACEMENT_BORDER;
		this->_yMin = this->_layer->Scene()->YMin() + OVERLAY_PLACEMENT_BORDER;
	}
	//Place in upper left corner
	else if (placement == WCPlacement::UpperLeft()) {
		this->_xMin = this->_layer->Scene()->XMin() + OVERLAY_PLACEMENT_BORDER;
		this->_yMin = this->_layer->Scene()->YMax() - this->_height - OVERLAY_PLACEMENT_BORDER;

	}
	//Place in upper right corner
	else if (placement == WCPlacement::UpperRight()) {
		this->_xMin = this->_layer->Scene()->XMax() - this->_width - OVERLAY_PLACEMENT_BORDER;
		this->_yMin = this->_layer->Scene()->YMax() - this->_height - OVERLAY_PLACEMENT_BORDER;
	}
	//Set the max parameters
	this->_xMax = this->_xMin + this->_width;
	this->_yMax = this->_yMin + this->_height;
	//Mark as dirty
	this->MarkDirty();
}


void WCOverlay::OnReshape(void) {
	//Call to the parent function
	this->WCWidget::OnReshape();
	//Check placement
	this->SetPlacement(this->_placement);
}


void WCOverlay::OnMouseDown(const WCMouseButton &button, const WPFloat x, const WPFloat y) {
	//Call the parent OnMouseDown
	this->WCWidget::OnMouseDown(button, x, y);
}


void WCOverlay::OnMouseEnter(WPFloat x, WPFloat y) {
	//Set mouse over to be true
	this->_isMouseOver = true;
	//Since we will change the color, mark as dirty
	this->MarkDirty();
}


void WCOverlay::OnMouseExit(const WPFloat x, const WPFloat y) {
	//Set mouse over to be false
	this->_isMouseOver = false;
	//Since we will change the color, mark as dirty
	this->MarkDirty();
}


void WCOverlay::OnMouseMove(const WPFloat x, const WPFloat y) {
}


void WCOverlay::Render(void) {
	//Draw the background if appropriate
	if (this->_isVisible && this->_isMouseOver) {
		//Check to see if is dirty
		if (this->_isDirty) {
			this->GenerateVBO();
			this->_isDirty = false;
		}
		//Prep for rendering
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
		glVertexPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), 0);
		
		/*** Draw the geometry ***/
		this->_color.Enable();
		glDrawArrays(GL_TRIANGLE_FAN, 0, this->_numVerts);
		
		/*** Draw done - now clean up ***/
		
		//Clean up
		glBindBuffer(GL_ARRAY_BUFFER, 0);	
		glDisableClientState(GL_VERTEX_ARRAY);		
	}
}



/***********************************************~***************************************************/


#define OVERLAY_CORNER_SEGMENTS					8
#define OVERLAY_CORNER_SIZE						8 * SCREEN_PIXEL_WIDTH


void WCOverlay::RenderRectangle(const WPFloat xMin, const WPFloat yMin, const WPFloat xMax,	const WPFloat yMax, 
	const WCColor &fillColor, const WCColor &edgeColor, const WCColor &shadeColor) {
	//Set up data
	GLfloat yMid = (yMin*0.33) + (yMax*0.66);
	GLfloat data[8] = { xMin, yMin, xMin, yMax, xMax, yMax, xMax, yMin };
	GLfloat midData[8] = { xMin, yMid, xMin, yMax, xMax, yMax, xMax, yMid };

	//Draw the filled quad
	fillColor.Enable();
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, data);
	glDrawArrays(GL_QUADS, 0, 4);
	
	//Draw the shade quad
	shadeColor.Enable();
	glVertexPointer(2, GL_FLOAT, 0, midData);
	glDrawArrays(GL_QUADS, 0, 4);	
	
	//Draw the edge
	edgeColor.Enable();
	glLineWidth(1.0);
	glVertexPointer(2, GL_FLOAT, 0, data);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	//Cleanup and exit
	glDisableClientState(GL_VERTEX_ARRAY);
}


void WCOverlay::RenderRoundedRectangle(const WPFloat xMin, const WPFloat yMin, const WPFloat xMax,	const WPFloat yMax, 
	const WCColor &fillColor, const WCColor &edgeColor, const WCColor &shadeColor) {
	//#+1 verts per segment, 4 segments, starting vertex, repeat first segment vertex for closure
	GLuint numVertices = (4 * (OVERLAY_CORNER_SEGMENTS + 1)) + 2;
	GLfloat l =OVERLAY_CORNER_SIZE;
	GLfloat angle = 2.0 * M_PI;
	GLfloat inc = angle / (4 * OVERLAY_CORNER_SEGMENTS);
	GLfloat data[numVertices * 2];
	GLuint index = 0;
	
	//Set the center point
	data[index++] = (xMin + xMax) / 2.0;
	data[index++] = (yMin + yMax) / 2.0;
	
	//Lower right corner
	for (GLuint i=0; i<=OVERLAY_CORNER_SEGMENTS; i++) {
		data[index++] = (xMax - OVERLAY_CORNER_SIZE) + (l * cos(angle));
		data[index++] = (yMin + OVERLAY_CORNER_SIZE) + (l * sin(angle));
		angle -= inc;
	}
	//Lower left corner
	angle += inc;
	for (GLuint i=0; i<=OVERLAY_CORNER_SEGMENTS; i++) {
		data[index++] = (xMin + OVERLAY_CORNER_SIZE) + (l * cos(angle));
		data[index++] = (yMin + OVERLAY_CORNER_SIZE) + (l * sin(angle));
		angle -= inc;
	}
	//Upper left corner
	angle += inc;
	for (GLuint i=0; i<=OVERLAY_CORNER_SEGMENTS; i++) {
		data[index++] = (xMin + OVERLAY_CORNER_SIZE) + (l * cos(angle));
		data[index++] = (yMax - OVERLAY_CORNER_SIZE) + (l * sin(angle));
		angle -= inc;
	}
	//Upper right corner
	angle += inc;
	for (GLuint i=0; i<=OVERLAY_CORNER_SEGMENTS; i++) {
		data[index++] = (xMax - OVERLAY_CORNER_SIZE) + (l * cos(angle));
		data[index++] = (yMax - OVERLAY_CORNER_SIZE) + (l * sin(angle));
		angle -= inc;

	}
	//Close last point
	data[index++] = xMax;
	data[index++] = yMin + OVERLAY_CORNER_SIZE;
	//Set up the render state
	fillColor.Enable();
//	glDisable(GL_LINE_SMOOTH);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, data);
	//Draw the filled quad
	glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);
	//Draw the edge
	edgeColor.Enable();
	glLineWidth(1.0);
	glVertexPointer(2, GL_FLOAT, 0, data+2);
	glDrawArrays(GL_LINE_LOOP, 0, numVertices-1);
	//Cleanup and exit
	glDisableClientState(GL_VERTEX_ARRAY);
}


/***********************************************~***************************************************/

