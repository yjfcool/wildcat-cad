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
#include "Scene/selection_manager.h"
#include "Scene/scene.h"
#include "Scene/camera.h"


/***********************************************~***************************************************/


void WCSelectionManager::ReadyTexture(void) {
	//Determine size of texture (based on screen size)
	GLsizei texWidth = this->_scene->WindowWidth();
	GLsizei texHeight = this->_scene->WindowHeight();

	/*** Setup Color Buffer Texture ***/

	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_tex);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::ReadyTexture - Inital Check.");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_tex);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::ReadyTexture - Final Check.");

	/*** Setup Depth RenderBuffer ***/
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->_depthRenderBuffer); 
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, texWidth, texHeight);
	
	/*** Clean up a bit ***/
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::Cleanup.");
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}


WCColor WCSelectionManager::EncodeColor(const WPInt index) {
	GLfloat r = (GLfloat)(index + 1) / 255.0;
//	std::cout << "Index: " << index << ", R: " << r << std::endl;
	return WCColor(r, 0.0, 0.0, 1.0);
}


WPInt WCSelectionManager::DecodeColor(const GLubyte r, const GLubyte g, const GLubyte b) {
	return (WPInt)r - 1;
}


WPUInt WCSelectionManager::ProcessSingleSelect(std::vector<std::pair<WCVisualObject*,WCSelectionObject*> > &selectionArray,
	GLubyte *pixels, bool notify) {
	WPInt index;
	std::map<WPInt, WPInt> counter;
	std::map<WPInt, WPInt>::iterator iter;
	for (int i=0; i < 16; i++) {
		index = this->DecodeColor(pixels[i*4], pixels[i*4+1], pixels[i*4+2]);
		if (index != -1) {
			iter = counter.find(index);
			//Add the index into the map with count = 1
			if (iter == counter.end()) counter.insert( std::make_pair(index, 1) );
			//Otherwise increment the count
			else (*iter).second++;
		}
	}
	//If no entries in counter, return 0
	if (counter.size() == 0) return 0;

	//Otherwise, find the largest count
	WPUInt maxCount = 0;
	for (iter = counter.begin(); iter != counter.end(); iter++) {
		if ((*iter).second > maxCount) {
			index = (*iter).first;
			maxCount = (*iter).second;
		}
	}
	//Index it our selectee
	WCVisualObject *visualSelectee = selectionArray[index].first;
	WCSelectionObject *selectionSelectee = selectionArray[index].second;
	this->_selected.push_back(selectionSelectee);
	this->_selectedVisuals.push_back(selectionArray[index]);
	//Notify if appropriate
	if (notify) selectionSelectee->OnSelection(true, std::list<WCVisualObject*>(1, visualSelectee));
	//Return 1
	return 1;
}


WPUInt WCSelectionManager::ProcessRectangleSelect(std::vector<std::pair<WCVisualObject*,WCSelectionObject*> > &selectionArray,
	GLubyte *pixels, const WPUInt width, const WPUInt height, bool notify) {
	WPInt index;
	std::map<WCSelectionObject*,std::list<WCVisualObject*> > selectees;
	std::map<WCSelectionObject*,std::list<WCVisualObject*> >::iterator iter;
	std::map<WCVisualObject*, WCSelectionObject*> selectedVisuals;
	std::map<WCVisualObject*, WCSelectionObject*>::iterator svIter;
	std::pair<WCVisualObject*,WCSelectionObject*> selectee;
	WCSelectionObject* selectionObject;
	//Look through all pixels and add into counter
	for (int i=0; i < width*height; i++) {
		index = this->DecodeColor(pixels[i*4], pixels[i*4+1], pixels[i*4+2]);
		//If there is a valid index
		if (index != -1) {
			selectee = selectionArray[index];
			//Automatically insert into selectedVisuals
			selectedVisuals.insert( selectee );
			//Try to find the selection object
			iter = selectees.find(selectee.second);
			//If not found, make new entry
			if (iter == selectees.end())
				selectees.insert( std::make_pair(selectee.second, std::list<WCVisualObject*>(1, selectee.first)) );
			//Otherwise, just add .first onto the list of visual objects
			else
				(*iter).second.push_back(selectee.first);
		}
	}
	//If no entries in map, return 0
	if (selectees.size() == 0) return 0;
	//Otherwise, add all entries into the list
	for (iter = selectees.begin(); iter != selectees.end(); iter++) {
		//Index it our selectionObject
		selectionObject = (*iter).first;
		this->_selected.push_back( selectionObject );
	}
	//Add selectedVisuals to master list
	for (svIter = selectedVisuals.begin(); svIter != selectedVisuals.end(); svIter++) this->_selectedVisuals.push_back( *svIter );
	//Now, notify all entries if appropriate
	if (notify)	{
		for (iter = selectees.begin(); iter != selectees.end(); iter++) (*iter).first->OnSelection(true, (*iter).second);
	}
	//Return the number of selected items
	return this->_selected.size();
}


/*
WPUInt WCSelectionManager::ProcessRectangleSelect(WCSelectionObject **selectionArray, GLubyte *pixels, 
	const WPUInt width, const WPUInt height, bool notify) {
	WPInt index;
	std::map<WCSelectionObject*,WCSelectionObject*> selectees;
	WCSelectionObject *selectee;
	//Look through all pixels and add into counter
	for (int i=0; i < width*height; i++) {
		index = this->DecodeColor(pixels[i*4], pixels[i*4+1], pixels[i*4+2]);
		//If there is a valid index
		if (index != -1) {
			selectee = selectionArray[index];
			selectees.insert( std::make_pair(selectee, selectee) );
		}
	}
	//If no entries in map, return 0
	if (selectees.size() == 0) return 0;
	std::map<WCSelectionObject*,WCSelectionObject*>::iterator iter;
	//Otherwise, add all entries into the list
	for (iter = selectees.begin(); iter != selectees.end(); iter++) {
		//Index it our selectee
		selectee = (*iter).first;
		this->_selected.push_back( selectee );
	}
	//Now, notify all entries if appropriate
	if (notify)	{
		for (iter = selectees.begin(); iter != selectees.end(); iter++) (*iter).first->OnSelection(true, std::list<WCVisualObject*>());
	}
	//Return the number of selected items
	return this->_selected.size();
}
*/

/***********************************************~***************************************************/


WCSelectionManager::WCSelectionManager(WCScene *scene) : ::WCObject(), _scene(scene), _tex(0), _depthRenderBuffer(0), _framebuffer(0),
	_objects(), _selected(), _selectedVisuals(), _stack() {
//	std::cout << "Selector: " << this << std::endl;
	//Make sure scene is not null
	if (this->_scene == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::WCSelectionManager - NULL Scene passed.");
		//throw error
		return;
	}
	//Retain the scene
	this->_scene->Retain(*this);
	//Set up generation textures
	glGenTextures(1, &this->_tex);
	glGenRenderbuffersEXT(1, &this->_depthRenderBuffer);
	this->ReadyTexture();
	//Generate the framebuffer object
	glGenFramebuffersEXT(1, &(this->_framebuffer));
}


WCSelectionManager::~WCSelectionManager() {
	//Release the scene if possible
	if (this->_scene != NULL) this->_scene->Release(*this);
}


void WCSelectionManager::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Message must be from scene - reset texture in case of reshape
	this->ReadyTexture();
}


bool WCSelectionManager::AddObject(WCVisualObject *object, WCSelectionObject *selector) {
//	std::cout << "Add object: " << object << ", Selector: " << selector << std::endl;
	//Make sure object is non-null
	if ((object == NULL) || (selector == NULL)){
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::AddObject - NULL Object passed.");
		return false;
	}
	//Make sure object is not already in the map
	std::map<WCVisualObject*,WCSelectionObject*>::iterator iter = this->_objects.find(object);
	if (iter != this->_objects.end()) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::AddObject - Object already selectable.");
		return false;
	}
	//Add object into selection map
	this->_objects.insert( std::make_pair(object, selector) );
	return true;
}


bool WCSelectionManager::RemoveObject(WCVisualObject *object) {
	//Make sure object is non-null
	if (object == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::RemoveObject - Object passed NULL.");
		return false;
	}
	//Make sure object is actually in the map
	std::map<WCVisualObject*,WCSelectionObject*>::iterator iter = this->_objects.find(object);
	if (iter != this->_objects.end()) {
		//Remove the object from the map
		this->_objects.erase(iter);
		//Make sure object is not in selected
		this->_selected.remove((*iter).second);
		//Make sure object is not in selectedVisuals
		this->_selectedVisuals.remove(*iter);
		//Return all good
		return true;
	}
	//Otherwise, there may be a problem
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::AddObject - Object not selectable.");
	return false;
}


WPUInt WCSelectionManager::Select(const WPUInt &xMin, const WPUInt &xMax, const WPUInt &yMin, const WPUInt &yMax, const bool &notify) {
	//Make sure not more than 255 selection objects
	if (this->_objects.size() >= 255) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::Select - More than 255 selection objects not supported.");
		return 0;
	}

	//Bind to framebuffer and bind texture
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_framebuffer);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, this->_tex, 0);
	// initialize depth renderbuffer 
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->_depthRenderBuffer); 
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->_depthRenderBuffer);
	GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTreeView::WCTreeView - Framebuffer is not complete.");
		return 0;
	}
	//Set the draw buffer
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glDisable(GL_BLEND);
	//Clear the texture
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Render each visual object with a distinct color
//	std::cout << "Selection objects: " << this->_objects.size() << std::endl;
	WPInt index = 0;
	WCColor indexColor;
	std::vector<std::pair<WCVisualObject*,WCSelectionObject*> > selectionArray;
	std::map<WCVisualObject*,WCSelectionObject*>::iterator iter;
	for (iter = this->_objects.begin(); iter != this->_objects.end(); iter++) {
		//Get the index color
		indexColor = this->EncodeColor(index);
		//Put the selection object into the array
		selectionArray.push_back( *iter );
		//Render the object
		(*iter).first->Render(0, indexColor, this->_scene->ActiveCamera()->Zoom());
		//Check for errors
		if (glGetError() != GL_NO_ERROR) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::WCSelectionManager - At rendering.");
			return 0;
		}
		//Increment the index
		index++;
	}

	//Clean up
	glEnable(GL_BLEND);

	//Evaluate the texture compared to the bounding box
	GLsizei width = std::max(xMax - xMin, (WPUInt)1);
	GLsizei height = std::max(yMax - yMin, (WPUInt)1);
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	
	GLubyte *pixels;
	//Handle single click special
	if ((width == 1) && (height == 1)) {
		pixels = new GLubyte[64];
		//Read from the texture
		glReadPixels(xMin-1, yMin-1, 4, 4, GL_RGBA, GL_UNSIGNED_BYTE , pixels);
	}
	//Standard case
	else {
		//Adjust minimum width and height
		width = std::max(xMax - xMin, (WPUInt)3);
		height = std::max(yMax - yMin, (WPUInt)3);		
		pixels = new GLubyte[width * height * 4];
		//Read from the texture
		glReadPixels(xMin, yMin, width, height, GL_RGBA, GL_UNSIGNED_BYTE , pixels);
	}

	//Exit from the framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0); 
	if (glGetError() != GL_NO_ERROR) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::WCSelectionManager - At clean up.");
		return 0;
	}
	//Process the selection results
	if ((width == 1) && (height == 1)) this->ProcessSingleSelect(selectionArray, pixels, notify);
	else this->ProcessRectangleSelect(selectionArray, pixels, width, height, notify);

	//Delete pixel data
	delete pixels;
	//Return the number of selected items
	return this->_selected.size();
}


bool WCSelectionManager::ForceSelection(WCSelectionObject *object, const bool &notify) {
	//Make sure object is non-null
	if (object == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::ForceSelection - NULL Object passed.");
		return false;
	}
	//Make sure object is not already selected
	std::list<WCSelectionObject*>::iterator iter;
	for (iter = this->_selected.begin(); iter != this->_selected.end(); iter++) {
		if ((*iter) == object) {
//			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::ForceSelection - Object already selected.");
			return false;			
		}
	}
	//Add to end of selected list
	this->_selected.push_back(object);
	//Call onSelection
	if (notify) object->OnSelection(true, std::list<WCVisualObject*>());
	//Be happy
	return true;
}


bool WCSelectionManager::ForceDeselection(WCSelectionObject *object, const bool &notify) {
	//Make sure object is non-null
	if (object == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::ForceDeselection - NULL Object passed.");
		return false;
	}
	//Make sure object is already selected
	std::list<WCSelectionObject*>::iterator iter;
	for (iter = this->_selected.begin(); iter != this->_selected.end(); iter++) {
		if ((*iter) == object) {
			//Remove the selected object
			this->_selected.erase(iter);
//			this->_selected.remove(object);
			//Call onDeselection
			if (notify) object->OnDeselection(true);
			//Be happy
			return true;
		}
	}
	//Object was not selected
//	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSelectionManager::ForceSelection - Object already selected.");
	return false;
}


void WCSelectionManager::PushObjects(void) {
}


void WCSelectionManager::PopObjects(void) {
}


void WCSelectionManager::Clear(const bool &notify) {
	//Loop through the list of selected items and call OnDeselection
	std::list<WCSelectionObject*>::iterator iter;
	WCSelectionObject* selectee;
	for (iter = this->_selected.begin(); iter != this->_selected.end(); iter++) {
		selectee = (*iter);
		if (notify) selectee->OnDeselection(true);
	}
	//Now clear the lists
	this->_selected.clear();
	this->_selectedVisuals.clear();
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, WCSelectionManager &sel) {
	out << "Selection Manager(" << &sel << ")\n";
	
	//Show the registered objects
	out << "\tObjects Registered(" << sel._objects.size() << "):\n";
	std::map<WCVisualObject*,WCSelectionObject*>::iterator objIter;
	for (objIter = sel._objects.begin(); objIter != sel._objects.end(); objIter++) {
		out << "\t\t" << (*objIter).first << ", " << (*objIter).second << std::endl;
	}
	
	//Show the selected objects
	out << "\tSelected Objects(" << sel._selected.size() << "):\n";
	std::list<WCSelectionObject*>::iterator selIter;
	for (selIter = sel._selected.begin() ; selIter != sel._selected.end(); selIter++) {
		out << "\t\t" << (*selIter) << std::endl;
	}
	return out;
}


/***********************************************~***************************************************/

