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
#include "Scene/tree_view.h"
#include "Scene/user_interface_layer.h"
#include "Scene/scene.h"
#include "Scene/scrollbar.h"
#include "Scene/event_controller.h"
#include "Scene/text.h"


//Local Defines
#define TREEVIEW_STEM_WIDTH						1.5
#define TREEVIEW_STEM_COLOR						0.0, 0.0, 0.0, 1.0
#define TREEVIEW_TEXT_COLOR						1.0, 1.0, 1.0, 1.0
#define TREEVIEW_TEXT_SELECTED_COLOR			1.0, 1.0, 0.0, 1.0
#define TREEVIEW_OPENCLOSE_COLOR				0.3f, 0.8f, 0.3f, 0.9f
#define TREEVIEW_ICON_SIZE						26


/***********************************************~***************************************************/


WCTreeElement::WCTreeElement(WCTreeView *tree, std::string name, WCEventController *controller, WSTexture *icon) : ::WCObject(),
	_tree(tree), _name(name), _label(NULL), _controller(controller), _isSelected(false), _isMouseOver(false), _isOpen(true), _icon(icon) {
	//Make sure tree is not null
	if (this->_tree == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeElement::WCTreeElement - NULL TreeView passed."); 
		//throw error
		return; 
	}
	//Create label for element
	this->_label = new WCText(this->_name, WCColor(TREEVIEW_TEXT_COLOR), WCTextFont::Times(), WCTextStyle::Roman(), 16.0);
	//Register the element
	this->_tree->RegisterElement(this);
}


WCTreeElement::~WCTreeElement() {
	//Need to release all children
	std::list<WCTreeElement*>::iterator iter;
	for (iter = this->_childList.begin(); iter != this->_childList.end(); iter++) {
		(*iter)->Release(*this);
	}
	//Unregister the element
	if 	(this->_tree != NULL) this->_tree->UnregisterElement(this);
}


void WCTreeElement::IsSelected(const bool state) {
	//Check to see if dirty
	if (state != this->_isSelected) {
		//Mark the tree as dirty
		this->_tree->MarkDirty();
		//Set the state
		this->_isSelected = state;
	}
}


WPUInt WCTreeElement::CountVisibleChildren(void) {
	//if closed return 0
	if (!this->_isOpen) return 0;
	WPUInt count = this->_childList.size();
	//Go through each child asking for number of visible children
	std::list<WCTreeElement*>::iterator iter;
	for (iter = this->_childList.begin(); iter != this->_childList.end(); iter++) {
		count += (*iter)->CountVisibleChildren();
	}	
	return count;
}


WPFloat WCTreeElement::FindMaxWidth(const WPUInt depth) {
	//Determine width of this item
	WPFloat iconSize = TREEVIEW_ICON_SIZE * SCREEN_PIXEL_WIDTH;
	//Determine entire size of line (add 4 pixels as a buffer for highlighting box)
	WPFloat width = (iconSize * (depth + 1.25)) + this->_label->Width() + (4 * SCREEN_PIXEL_WIDTH);
	//Return if not open
	if (!this->_isOpen) return width;
	//Go through each child
	WPFloat tmpWidth;
	std::list<WCTreeElement*>::iterator iter;
	for (iter = this->_childList.begin(); iter != this->_childList.end(); iter++) {
		//Ask for childs max width
		tmpWidth = (*iter)->FindMaxWidth(depth+1);
		//If tmpWidth is wider than width, update width
		if (tmpWidth > width) width = tmpWidth;
	}
	//Return the max width
	return width;
}


bool WCTreeElement::AddFirstChild(WCTreeElement *element) {
	//Make sure element is not null
	if (element == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeElement::AddFirstChild - NULL Element passed."); return false;  }
	//Add to the end of the list
	this->_childList.push_front(element);
	//Retain the child
	element->Retain(*this);
	//Notify upwards
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
	return true;
}


bool WCTreeElement::AddLastChild(WCTreeElement *element) {
	//Make sure element is not null
	if (element == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeElement::AddLastChild - NULL Element passed."); return false;  }
	//Add to the end of the list
	this->_childList.push_back(element);
	//Retain the child
	element->Retain(*this);
	//Notify upwards
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
	return true;
}


bool WCTreeElement::AddBefore(WCTreeElement *element, WCTreeElement *sibling) {
	//Make sure element is not null
	if (element == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeElement::AddBefore - NULL Element passed."); return false;  }
	//Find the place of sibling
	std::list<WCTreeElement*>::iterator iter;
	for (iter = this->_childList.begin(); iter != this->_childList.end(); iter++) {
		//If a match is found
		if (*iter == sibling) {
			this->_childList.insert(iter, element);
			//Retain the child
			element->Retain(*this);
			//Notify upwards
			this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
			return true;
		}
	}
	//Sibling not found case
	return false;
}


bool WCTreeElement::AddAfter(WCTreeElement *element, WCTreeElement *sibling) {
	//Make sure element is not null
	if (element == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeElement::AddAfter - NULL Element passed."); return false;  }
	//Check last element case
	if (sibling == this->_childList.back()) { 
		this->_childList.push_back(element);
		//Retain the child
		element->Retain(*this);
		return true; 
	}
	//Find the sibling
	std::list<WCTreeElement*>::iterator iter;
	for (iter = this->_childList.begin(); iter != this->_childList.end(); iter++) {
		//If a match is found
		if (*iter == sibling) {
			this->_childList.insert(++iter, element);
			//Retain the child
			element->Retain(*this);
			return true;
			//Notify upwards
			this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
		}
	}
	//Sibling not found case
	return false;
}


bool WCTreeElement::Remove(WCTreeElement *element) {
	//Make sure element is not null
	if (element == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeElement::Remove - NULL Element passed."); return false;  }
	//Find the element
	std::list<WCTreeElement*>::iterator iter;
	for (iter = this->_childList.begin(); iter != this->_childList.end(); iter++) {
		//If element is found
		if (*iter == element) {
			//Remove element
			this->_childList.erase(iter);
			//Release the element
			element->Release(*this);
			//Notify upwards
			this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
			return true;
		}
	}
	//Element not found case
	return false;	
}


WCTreeElement* WCTreeElement::OnMouseDown(const WCMouseButton &button, WCVector4 &pos, WPUInt depth) {
	GLfloat iconSize = (GLfloat)(TREEVIEW_ICON_SIZE * SCREEN_PIXEL_WIDTH);
	GLfloat indent = iconSize * depth;
	GLfloat yLoc = (GLfloat)pos.L();	

	//Determine if within the box vertically
	if ((pos.J() <= yLoc) && (pos.J() >= yLoc - iconSize)) {
		GLfloat xLoc = (GLfloat)pos.K() + indent;
		//See if within open/close box
		if ((this->_childList.size() > 0) && 
			(pos.J() >= yLoc-0.666*iconSize) && (pos.J() <= yLoc-0.333*iconSize) && 
			(pos.I() >= xLoc-0.666*iconSize) && (pos.I() <= xLoc-0.333*iconSize)) {
			this->_isOpen = !this->_isOpen;
			this->_tree->MarkDirty();
			return this;
		}
		//See if within name box
		if ((pos.I() >= xLoc) && (pos.I() <= xLoc + (1.25 * iconSize) + this->_label->Width() + (4 * SCREEN_PIXEL_WIDTH))) {
			//See if selected
			if (this->_isSelected) this->_controller->OnDeselection(false);
			//Or if not selected
			else this->_controller->OnSelection(false, std::list<WCVisualObject*>());
			//Mark the tree as dirty
			this->_tree->MarkDirty();
			return this;
		}
		//Otherwise do nothing
		return this;
	}
	//Otherwise, modify y value for next object
	pos.L( pos.L() - iconSize );
	//If open, then check children
	if (this->_isOpen) {
		//Draw each child
		std::list<WCTreeElement*>::iterator iter;
		for (iter = this->_childList.begin(); iter != this->_childList.end(); iter++) {
			//Draw the child and its children
			WCTreeElement *retVal = (*iter)->OnMouseDown(button, pos, depth+1);
			//If not null, return the retVal
			if (retVal != NULL) return retVal;
		}
	}
	//Otherwise, no click
	return NULL;
}


WCTreeElement* WCTreeElement::OnMouseMove(WCVector4 &pos, WPUInt depth) {
	GLfloat iconSize = (GLfloat)(TREEVIEW_ICON_SIZE * SCREEN_PIXEL_WIDTH);
	GLfloat xLoc = (GLfloat)(pos.K() + depth * iconSize);
	GLfloat yLoc = (GLfloat)pos.L();
	//Determine if within the box
	if ((pos.J() <= yLoc) && (pos.J() >= yLoc - iconSize) &&		//Below top of box and above bottom of box
		(pos.I() >= xLoc) && (pos.I() <= xLoc + (1.25 * iconSize) + this->_label->Width() + (4 * SCREEN_PIXEL_WIDTH))) {
		return this;
	}
	
	//Otherwise, modify y value for next object
	pos.L( pos.L() - iconSize );	
	//Render children if open
	if (this->_isOpen) {
		//Draw each child
		std::list<WCTreeElement*>::iterator iter;
		for (iter = this->_childList.begin(); iter != this->_childList.end(); iter++) {
			//Draw the child and its children
			WCTreeElement *retVal = (*iter)->OnMouseMove(pos, depth+1);
			//If not null, return the retVal
			if (retVal != NULL) return retVal;
		}
	}
	//Otherwise return NULL
	return NULL;
}


void WCTreeElement::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
//	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeElement::ReceiveNotice - Not yet implemented.");
}


void WCTreeElement::Render(WPFloat &x, WPFloat &y, WPUInt depth) {
	//Setup location and spacing
	GLfloat iconSize = (GLfloat)(TREEVIEW_ICON_SIZE * SCREEN_PIXEL_WIDTH);
	GLfloat xLoc = (GLfloat)(x + iconSize * depth);
	GLfloat yLoc = (GLfloat)y;

	//Set drawing state
	glEnableClientState(GL_VERTEX_ARRAY);
	//If open, draw drop-stem
	if ((this->_isOpen) && (this->_childList.size() > 0)){
		GLfloat drop =  (GLfloat)(this->CountVisibleChildren() - this->_childList.back()->CountVisibleChildren());
		//Draw only if a drop-stem is needed
		if (drop > 0) {
			//Stem color is black
			glColor4f(TREEVIEW_STEM_COLOR);
			glLineWidth(TREEVIEW_STEM_WIDTH);
			GLfloat dropVerts[4] = { xLoc+0.5f*iconSize, yLoc-0.5f*iconSize, 
									 xLoc+0.5f*iconSize, yLoc-0.5f*iconSize-(iconSize*drop) };
			glVertexPointer(2, GL_FLOAT, 0, dropVerts);
			glDrawArrays(GL_LINES, 0, 2);
		}
	}

	//Draw stem if not root
	if (depth != 0) {
		//Stem color is black
		glColor4f(TREEVIEW_STEM_COLOR);
		glLineWidth(TREEVIEW_STEM_WIDTH);
		GLfloat stemVerts[4] = { xLoc-0.5f*iconSize, yLoc-0.5f*iconSize, 
								 xLoc+0.05f*iconSize, yLoc-0.5f*iconSize };
		glVertexPointer(2, GL_FLOAT, 0, stemVerts);
		glDrawArrays(GL_LINES, 0, 2);
	}
	
	//Draw open/close icon if there are children
	if (this->_childList.size() > 0) {
		//Set the color
		glColor4f(TREEVIEW_OPENCLOSE_COLOR);
		//Setup vertices of the triangle (depends on open or closed)
		GLfloat openCloseVerts[6];
		//Open - V
		if (this->_isOpen) {
			openCloseVerts[0] = xLoc - (0.666f *iconSize);
			openCloseVerts[1] = yLoc - (0.4f *iconSize);
			openCloseVerts[2] = xLoc - (0.333f * iconSize);
			openCloseVerts[3] = openCloseVerts[1];
			openCloseVerts[4] = xLoc - (0.5f * iconSize);
			openCloseVerts[5] = yLoc - (0.733f * iconSize);
		}
		//Closed - |>
		else {
			openCloseVerts[0] = xLoc - (0.6f *iconSize);
			openCloseVerts[1] = yLoc - (0.333f *iconSize);
			openCloseVerts[2] = xLoc - (0.266f * iconSize);
			openCloseVerts[3] = yLoc - (0.5f * iconSize);
			openCloseVerts[4] = openCloseVerts[0];
			openCloseVerts[5] = yLoc - (0.666f * iconSize);
		}
		//Setup the vertex pointer
		glVertexPointer(2, GL_FLOAT, 0, openCloseVerts);
		//Add draw
		glDrawArrays(GL_TRIANGLES, 0, 3);		
	}

	//Enable texture coordinates
	if (this->_icon != NULL) {
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		//Icon vertices
		GLfloat iconVerts[8] = { xLoc, yLoc - iconSize,
								 xLoc, yLoc, 
								 xLoc + iconSize, yLoc,
								 xLoc + iconSize, yLoc - iconSize };
		GLfloat texCoords[8] = { 3.0, 3.0,
								 3.0, 29.0,
								 29.0, 29.0,
								 29.0, 3.0 };
		//If there is an icon, enable its texture
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_icon->_id);
		glVertexPointer(2, GL_FLOAT, 0, iconVerts);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		glDrawArrays(GL_QUADS, 0, 4);
		
		//Clean up drawing state
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
	}
	
	//Draw highlight box if mouse is over
	if (this->_isMouseOver) {
		GLfloat textWidth = this->_label->Width();
		GLfloat xMin = (GLfloat)(xLoc + 1.25 * iconSize);
		glDisable(GL_BLEND);
		WCOverlay::RenderRectangle(xMin-SCREEN_PIXEL_WIDTH, yLoc-iconSize+SCREEN_PIXEL_WIDTH, xMin+textWidth+SCREEN_PIXEL_WIDTH, yLoc-SCREEN_PIXEL_WIDTH, 
			WCColor(0.5, 0.5, 0.5, 0.3), 
			WCColor(1.0, 1.0, 1.0, 0.2),
			WCColor(0.75, 0.75, 0.75, 0.25));
		glEnable(GL_BLEND);
	}

	//Set the text up approprately
	if (this->_isSelected) {
		//Set the text color
		this->_label->Color( WCColor(TREEVIEW_TEXT_SELECTED_COLOR) );
		//Set the text underlined state
		this->_label->IsUnderlined(true);
	}
	else {
		//Set the text color
		this->_label->Color( WCColor(TREEVIEW_TEXT_COLOR) );
		//Set the text underlined state
		this->_label->IsUnderlined(false);
	}
	//Draw the label
	this->_label->DrawAtPoint((GLfloat)(xLoc+(1.25*iconSize)),
							  (GLfloat)(yLoc-(0.9*iconSize)+SCREEN_PIXEL_WIDTH));

	//Modify y value for next object
	y = y - iconSize;
	//Render children if open
	if (this->_isOpen) {
		//Draw each child
		std::list<WCTreeElement*>::iterator iter;
		for (iter = this->_childList.begin(); iter != this->_childList.end(); iter++) {
			//Draw the child and its children
			(*iter)->Render(x, y, depth+1);
		}
	}
}


void WCTreeElement::Print(int indent) {
	//Print the name of the element
	for (int i=0; i< indent; i++) std::cout << "\t";
	std::cout << this->_name << "(*: " << this << ")\n";
	//Print out all children indented
	std::list<WCTreeElement*>::iterator iter;
	for (iter = this->_childList.begin(); iter != this->_childList.end(); iter++)
		(*iter)->Print(indent+1);	
}


/***********************************************~***************************************************/


void WCTreeView::GenerateTexture(void) {
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_tex);
	GLfloat borderColor[] = {0.0, 0.0, 0.0, 0.0};
	glTexParameterfv(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, this->_texWidth, this->_texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeView::GenerateTexture - GL error at Create.");

	//Bind to framebuffer and bind texture
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_framebuffer);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, this->_tex, 0);
	GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeView::WCTreeView - Framebuffer is not complete.");
	//Set the viewport
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, this->_texWidth, this->_texHeight);
	//Set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, this->_virtualWidth, 0.0, this->_virtualHeight, -1.0, 1.0);
	//Clear the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	//Clear the framebuffer
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//Render full virtual tree
	WPFloat xPos = 0.0;
	WPFloat yPos = this->_virtualHeight;
	this->_root->Render(xPos, yPos, 0);
	//Restore clear color
	glClearColor(0.0, 0.0, 0.0, 1.0);

	//Restore the modelview matrix
	glPopMatrix();
	//Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	//Restore the viewport
	glPopAttrib();

	//Unbind the framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//Check for errors
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeView::WCTreeView - At clean up.");
}


void WCTreeView::GenerateBuffers(void) {
	//Create the vertex data
	GLfloat data[8];
	//Check scroll state
	if (this->_scrollbar->IsVisible()) {
		//Lower left
		data[0] = (GLfloat)(this->_xMin + this->_scrollbar->Width());
		data[1] = (GLfloat)this->_yMin;
		//Upper left
		data[2] = (GLfloat)(this->_xMin + this->_scrollbar->Width());
		data[3] = (GLfloat)this->_yMax;
		//Upper right
		data[4] = (GLfloat)(this->_xMax + this->_scrollbar->Width());
		data[5] = (GLfloat)this->_yMax;
		//Lower right
		data[6] = (GLfloat)(this->_xMax + this->_scrollbar->Width());
		data[7] = (GLfloat)this->_yMin;
	}
	else {
		//Lower left
		data[0] = (GLfloat)this->_xMin;
		data[1] = (GLfloat)this->_yMin;
		//Upper left
		data[2] = (GLfloat)this->_xMin;
		data[3] = (GLfloat)this->_yMax;
		//Upper right
		data[4] = (GLfloat)this->_xMax;
		data[5] = (GLfloat)this->_yMax;
		//Lower right
		data[6] = (GLfloat)this->_xMax;
		data[7] = (GLfloat)this->_yMin;
	}

	//Copy the data into the VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), data, GL_STATIC_DRAW);

/*** DEBUG ***
	std::cout << "Actual Width: " << this->_width << std::endl;
	std::cout << "Actual Height: " << this->_height << std::endl;
	std::cout << "Virtual Width: " << this->_virtualWidth << std::endl;
	std::cout << "Virtual Height: " << this->_virtualHeight << std::endl;
	std::cout << "Texture Width: " << this->_texWidth << std::endl;
	std::cout << "Texture Height: " << this->_texHeight << std::endl;
	std::cout << "Scrollbar Extent: " << this->_scrollbar->Extent() << std::endl;
	std::cout << "Scrollbar Position: " << this->_scrollbar->Position() << std::endl;
/*** DEBUG ***/

	//Determine upper left tex-coord
	GLfloat actualPixelHeight = (GLfloat)(this->_height / SCREEN_PIXEL_WIDTH);
	GLfloat actualPixelWidth = (GLfloat)(this->_width / SCREEN_PIXEL_WIDTH);
	GLfloat upperY = (GLfloat)(this->_texHeight - ((this->_texHeight - actualPixelHeight) * this->_scrollbar->Position()));
	//Create the color data
	GLfloat texCoords[8];
	//Upper left
	texCoords[2] = 0.0;
	texCoords[3] = upperY;
	//Upper right
	texCoords[4] = actualPixelWidth;
	texCoords[5] = texCoords[3];
	//Lower right
	texCoords[6] = actualPixelWidth;
	texCoords[7] = texCoords[3] - actualPixelHeight;
	//Lower left
	texCoords[0] = 0.0;
	texCoords[1] = texCoords[7];

/*** DEBUG ***
	std::cout << "TC[0] " << texCoords[0] << std::endl;
	std::cout << "TC[1] " << texCoords[1] << std::endl;
	std::cout << "TC[2] " << texCoords[2] << std::endl;
	std::cout << "TC[3] " << texCoords[3] << std::endl;
	std::cout << "TC[4] " << texCoords[4] << std::endl;
	std::cout << "TC[5] " << texCoords[5] << std::endl;
	std::cout << "TC[6] " << texCoords[6] << std::endl;
	std::cout << "TC[7] " << texCoords[7] << std::endl;
/*** DEBUG ***/
	//Copy the data into the VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->_texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
	//Clean up
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


WCVector4 WCTreeView::FromVirtualWindow(const WCVector4 &pos) {
	return pos;
}


WCVector4 WCTreeView::ToVirtualWindow(const WCVector4 &pos) {
	//Perform initial tree offset adjustment
	WPFloat newX = pos.I() - this->_xMin;
	WPFloat newY = pos.J() - this->_yMin;
	WPFloat height = this->_height;
	
	//Perform scrollbar adjustments
	if (this->_scrollbar->IsVisible()) {
		//Adjust x for scrollbar width
		newX -= this->_scrollbar->Width();
		//Perform extent and position adjustment
		newY += (this->_virtualHeight - this->_height) * (1.0 - this->_scrollbar->Position());
		//Set the height to virtualHeight
		height = this->_virtualHeight;
	}
	//Set the new values
	WCVector4 newPos(newX, newY, 0.0, height);
	//Return the adjust position
	return newPos;
}


/***********************************************~***************************************************/


WCTreeView::WCTreeView(WCUserInterfaceLayer *layer) : ::WCOverlay(layer, false), _elementMap(), _root(NULL), _selected(), _mouseOver(NULL),
	_tex(0), _texWidth(0), _texHeight(0), _framebuffer(0), _vertexBuffer(0), _texCoordBuffer(0),
	_scrollbar(NULL), _scale(1.0), _virtualWidth(0.0), _virtualHeight(0.0) {

	//Make sure layer is not null
	if (this->_layer == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeView::WCTreeView - NULL layer passed.");
		//throw error
		return;
	}
	//Register the widget with the layer
	layer->RegisterWidget(this);
	//Set up generation texture
	glGenTextures(1, &this->_tex);
	//Generate the framebuffer object
	glGenFramebuffersEXT(1, &(this->_framebuffer));
	
	//Generate the buffers
	glGenBuffers(1, &this->_vertexBuffer);
	glGenBuffers(1, &this->_texCoordBuffer);
	this->_isDirty = true;
	//Create the scrollbar
	this->_scrollbar = new WCVerticalScrollbar(this, 1.0, 0.0);
	this->_scrollbar->IsVisible(false);
	this->_scrollbar->Retain(*this);
}


WCTreeView::~WCTreeView() {
	//Release and delete the scrollbar
	if (this->_scrollbar != NULL) {
		this->_scrollbar->Release(*this);
		delete this->_scrollbar;
	}
	//Release the root
	if (this->_root != NULL) this->_root->Release(*this);
	//Make sure to unregister the widget
	if (this->_layer != NULL) this->_layer->UnregisterWidget(this);
	//Delete the texture
	glDeleteTextures(1, &(this->_tex));
	//Delete the buffers
	glDeleteBuffers(1, &this->_vertexBuffer);
	glDeleteBuffers(1, &this->_texCoordBuffer);
}


void WCTreeView::IsVisible(const bool state) {
	//If switching do stuff
	if (state != this->_isVisible) {
		//Set the state
		this->_isVisible = state;
		//Clear the mouse over (if appropriate)
		if (this->_mouseOver != NULL) {
			this->_mouseOver->IsMouseOver(false);
			this->_mouseOver = NULL;
		}
		//Mark as dirty
		this->MarkDirty();
	}
}


void WCTreeView::MarkDirty(void) {
	//Calculate the width and height of the virtual tree
	if (this->_root != NULL) {
		//Upate virtual width and height
		this->_virtualWidth = this->_root->FindMaxWidth(0);
		this->_virtualHeight = (this->_root->CountVisibleChildren() + 1) * (TREEVIEW_ICON_SIZE * SCREEN_PIXEL_WIDTH);
		//Set the xMax and width
		this->_width = this->_virtualWidth + this->_scrollbar->Width();
		this->_xMax = this->_xMin + this->_width;
		//Set the yMin and height
		this->_height = STDMIN(this->_layer->Scene()->Height() - 1.5, this->_virtualHeight);
		this->_yMin = this->_yMax - this->_height;
		//Update texture width and height
		this->_texWidth = (GLuint)(this->_virtualWidth / SCREEN_PIXEL_WIDTH);
		this->_texHeight = (GLuint)(this->_virtualHeight / SCREEN_PIXEL_WIDTH);
		//Update scrollbar values
		WPFloat extent = this->_height / this->_virtualHeight;
		this->_scrollbar->Extent( STDMIN(extent, 1.0) );
		//If there is a virtual extent, then enable scrollbar
		if (extent < 1.0) this->_scrollbar->IsVisible(true);
		else this->_scrollbar->IsVisible(false);
	}
	else {
		//Set the xMax and width
		this->_width = 1.0;
		this->_xMax = this->_xMin + this->_width;
		//Set the virtual width and height, and texture width and height
		this->_virtualWidth = 0.0;
		this->_virtualHeight = 0.0;
		this->_texWidth = 0;
		this->_texHeight = 0;
		//Update the scroll bar
		this->_scrollbar->IsVisible(false);
		this->_scrollbar->Extent(1.0);
	}
	//Update the scrollbar
	this->_scrollbar->OnReshape();
	//Call parent mark dirty
	this->WCWidget::MarkDirty();
}


bool WCTreeView::RegisterElement(WCTreeElement *element) {
	//Make sure element is not null
	if (element == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeView::RegisterElement - NULL element passed.");
		return false;
	}
	//Set the root if needed
	if (this->_root == NULL) {
		this->_root = element;
	}
	//Add the element into the map - don't need to care if already registered
	this->_elementMap.insert( std::make_pair( element, element) );
	//Retain the element
	element->Retain(*this);
	//Mark as dirty
	this->MarkDirty();
	return true;
}


bool WCTreeView::UnregisterElement(WCTreeElement *element) {
	//Return true if null element passed
	if (element == NULL) return false;
	//Go through all registered elements and remove
	std::map<WCTreeElement*,WCTreeElement*>::iterator iter;
	for (iter = this->_elementMap.begin(); iter != this->_elementMap.end(); iter++) {
		(*iter).second->Remove(element);
	}
	//Add the element into the map
	int count = this->_elementMap.erase( element );
	//If there are none, no worries
	if (count == 0)	return false;
	//Release the element
	element->Release(*this);
	//If there are no elements left, set root to NULL
	if (element == this->_root) this->_root = NULL;
	//Mark as dirty
	this->MarkDirty();
	return true;
}


WCTreeElement* WCTreeView::ElementFromName(const std::string &name) {
	//Go through all registered elements and remove
	std::map<WCTreeElement*,WCTreeElement*>::iterator iter;
	for (iter = this->_elementMap.begin(); iter != this->_elementMap.end(); iter++) {
		if ((*iter).second->Name() == name) return (*iter).second;
	}
	//Otherwise, return null
	return NULL;
}


void WCTreeView::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark as dirty
	this->MarkDirty();
}


void WCTreeView::OnReshape(void) {
	//Set the upper-left corner and the height
	this->_xMin = this->_layer->Scene()->XMin() + OVERLAY_PLACEMENT_BORDER;
	this->_yMax = this->_layer->Scene()->YMax() - OVERLAY_PLACEMENT_BORDER;
	//xMax, yMin, width, and height will be set by MarkDirty (will also update scrollbar)
	this->MarkDirty();
}


void WCTreeView::OnMouseDown(const WCMouseButton &button, const WPFloat x, const WPFloat y) {
	//Make sure is visible and only react to left-mouse
	if ((!this->_isVisible) || (button != WCMouseButton::Left())) return;
	//If appropriate, see if belongs to scrollbar
	if (this->_scrollbar->HitTest(x, y)) {
		//Pass along mouse event
		this->_scrollbar->OnMouseDown(button, x, y);
	}
	//Otherwise...
	else {
		//Setup position
		WCVector4 pos = this->ToVirtualWindow(WCVector4(x, y, 0.0, this->_height));
		//See if any elements are being clicked
		this->_root->OnMouseDown(button, pos, 0);
	}
}


void WCTreeView::OnMouseUp(const WCMouseButton &button, const WPFloat x, const WPFloat y) {
	//If appropriate, see if belongs to scrollbar
	if (this->_scrollbar->IsVisible()) {
		//Pass along mouse event
		this->_scrollbar->OnMouseUp(button, x, y);
	}
}


void WCTreeView::OnMouseEnter(const WPFloat x, const WPFloat y) {
	//Nothing for now
}


void WCTreeView::OnMouseExit(const WPFloat x, const WPFloat y) {
	//Make sure is visible
	if (!this->_isVisible) return;
	//Clear the mouse over (if appropriate)
	if (this->_mouseOver != NULL) {
		this->_mouseOver->IsMouseOver(false);
		this->_mouseOver = NULL;
		//Mark tree as dirty
		this->MarkDirty();
	}
}


void WCTreeView::OnMouseMove(const WPFloat x, const WPFloat y) {
	//Make sure is visible
	if (!this->_isVisible) return;
	//If appropriate, see if belongs to scrollbar
	if (this->_scrollbar->IsSliderSelected()) {
		//Pass along mouse event
		this->_scrollbar->OnMouseMove(x, y);
	}	
	//Otherwise pass to tree
	else {
		//Setup position
		WCVector4 pos = this->ToVirtualWindow(WCVector4(x, y, 0.0, this->_height));
		//See if any elements are being moused over
		WCTreeElement *mouseOver = this->_root->OnMouseMove(pos, 0);
		//See if something needs to change
		if (mouseOver != this->_mouseOver) {
			//Clear _mouseOver if needed
			if (this->_mouseOver != NULL) this->_mouseOver->IsMouseOver(false);
			//Set mouse over
			this->_mouseOver = mouseOver;
			//Color if appropriate
			if (this->_mouseOver != NULL) this->_mouseOver->IsMouseOver(true);
			//Mark this as dirty
			this->MarkDirty();
		}
	}
}


void WCTreeView::OnScroll(const WPFloat &x, const WPFloat &y) {
	//Make sure scrollbar is visible
	if (!this->_scrollbar->IsVisible()) return;
	//See if the mouse is in the treeview
	if ((this->_layer->Scene()->MouseX() < this->_xMin) || (this->_layer->Scene()->MouseX() > this->_xMax) ||
		(this->_layer->Scene()->MouseY() < this->_yMin) || (this->_layer->Scene()->MouseY() > this->_yMax)) return;
	//Otherwise, do something
	this->_scrollbar->OnScroll(x, y);
}


void WCTreeView::OnArrowKeyPress(const WCArrowKey &key) {
	//See if scrollbar is visible and mouse is over it
	if (this->_scrollbar->IsVisible() && this->_scrollbar->HitTest(this->_layer->Scene()->MouseX(), this->_layer->Scene()->MouseY())) {
		std::cout << "Arrow key passed to scrollbar\n";
	}
	//See if in self
	else if (this->HitTest(this->_layer->Scene()->MouseX(), this->_layer->Scene()->MouseY())) {
		std::cout << "Arrow key passed to self\n";
	}

}

void WCTreeView::Render(void) {
	//Make sure is visible and has root element
	if ((!this->_isVisible) || (this->_root == NULL)) return;
	//Check to see if tree is dirty
	if (this->_isDirty) {
		//Generate the tree texture
		this->GenerateTexture();
		//Generate the display buffers
		this->GenerateBuffers();
		//Mark as clean
		this->_isDirty = false;
	}

	//Render the scrollbar
	this->_scrollbar->Render();

	//Enable the texture for use
	glEnable(GL_TEXTURE_RECTANGLE_ARB);	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_tex);
	//Setup vertex array
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBuffer);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	//Setup texture array
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_texCoordBuffer);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);

	//Draw
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glDrawArrays(GL_QUADS, 0, 4);
	
	//Bind back to nothing
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	//Make sure that vertex and normal arrays are disabled
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
	//Clean up
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	
	//Optionally render tie-in lines
	if (this->_scrollbar->IsVisible()) {
		//Setup data arrays
		GLfloat lineVerts[12] = { (GLfloat)this->_xMax - 1.0f, (GLfloat)this->_yMax,
								  (GLfloat)(this->_xMin + this->_scrollbar->Width()), (GLfloat)this->_yMax,
								  (GLfloat)this->_xMin, (GLfloat)this->_yMax,
								  (GLfloat)this->_xMin, (GLfloat)this->_yMin,
								  (GLfloat)(this->_xMin + this->_scrollbar->Width()), (GLfloat)this->_yMin,
								  (GLfloat)this->_xMax - 1.0f, (GLfloat)this->_yMin};
		GLfloat lineColors[24] = { 0.5f, 0.5f, 0.5f, 0.0f,
								   0.5f, 0.5f, 0.5f, 0.4f,
								   0.5f, 0.5f, 0.5f, 0.75f,
								   0.5f, 0.5f, 0.5f, 0.75f,
								   0.5f, 0.5f, 0.5f, 0.4f,
								   0.5f, 0.5f, 0.5f, 0.0f };
		//Setup draw state
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, lineVerts);
		glColorPointer(4, GL_FLOAT, 0, lineColors);
		glLineWidth(1.0);
		//Draw the line
		glDrawArrays(GL_LINE_STRIP, 0, 6);
		//Clean up state
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

	}
	
	//Check for errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeView::Render Error - GL error at Exit.");
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, WCTreeView &tree) {
	//Print info about the tree
	out << "TreeView(" << &tree << ")\n";
	//Print out all registered elements
	std::cout << "\t *** Registered Objects ***\n";
	std::map<WCTreeElement*,WCTreeElement*>::iterator iter;
	for (iter = tree._elementMap.begin(); iter != tree._elementMap.end(); iter++)
		out << "\t" << iter->second->Name() << std::endl;
	//Print out children
	std::cout << "\t *** Children ***\n";
	if (tree._root != NULL) tree._root->Print(1);
	return out;
}


/***********************************************~***************************************************/


