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
#include "RTVisualization/vis_motiontracker.h"
#include "RTVisualization/visualization.h"
#include "RTVisualization/vis_packet.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


void WCVisMotionTracker::GenerateBuffers(void) {
#define bufferSize  24 * 3
	GLfloat vertData[bufferSize] = {
								-0.5, -0.5, -0.5,	// Bottom upper left
								+0.5, -0.5, -0.5,	// Bottom upper right
								+0.5, +0.5, -0.5,	// Bottom lower right
								-0.5, +0.5, -0.5,	// Bottom lower left

								-0.5, -0.5, +0.5,	// Back upper left
								+0.5, -0.5, +0.5,	// Back upper right
								+0.5, -0.5, -0.5,	// Back lower right
								-0.5, -0.5, -0.5,	// Back lower left

								-0.5, +0.5, +0.5,	// Top upper left
								+0.5, +0.5, +0.5,	// Top upper right
								+0.5, -0.5, +0.5,	// Top lower right
								-0.5, -0.5, +0.5,	// Top lower left

								+0.5, +0.5, +0.5,	// Front upper left
								-0.5, +0.5, +0.5,	// Front upper right
								-0.5, +0.5, -0.5,	// Front lower right
								+0.5, +0.5, -0.5,	// Front lower left

								+0.5, -0.5, +0.5,	// Left upper left
								+0.5, +0.5, +0.5,	// Left upper right
								+0.5, +0.5, -0.5,	// Left lower right
								+0.5, -0.5, -0.5,	// Left lower left

								-0.5, +0.5, +0.5,	// Right upper left
								-0.5, -0.5, +0.5,	// Right upper right
								-0.5, -0.5, -0.5,	// Right lower right
								-0.5, +0.5, -0.5,	// Right lower left
							   };
	//Copy the data into the VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSize*sizeof(GLfloat), vertData, GL_STATIC_DRAW);

	GLfloat normData[bufferSize] = {
								-0.0, -0.0, -1.0,	// Bottom upper left
								-0.0, -0.0, -1.0,	// Bottom upper right
								-0.0, -0.0, -1.0,	// Bottom lower right
								-0.0, -0.0, -1.0,	// Bottom lower left

								-0.0, -1.0, -0.0,	// Bottom upper left
								-0.0, -1.0, -0.0,	// Bottom upper right
								-0.0, -1.0, -0.0,	// Bottom lower right
								-0.0, -1.0, -0.0,	// Bottom lower left

								-0.0, -0.0, +1.0,	// Top upper left
								-0.0, -0.0, +1.0,	// Top upper right
								-0.0, -0.0, +1.0,	// Top lower right
								-0.0, -0.0, +1.0,	// Top lower left

								-0.0, +1.0, +0.0,	// Front upper left
								-0.0, +1.0, +0.0,	// Front upper right
								-0.0, +1.0, +0.0,	// Front lower right
								-0.0, +1.0, +0.0,	// Front lower left

								+1.0, +0.0, +0.0,	// Left upper left
								+1.0, +0.0, +0.0,	// Left upper right
								+1.0, +0.0, +0.0,	// Left lower right
								+1.0, +0.0, +0.0,	// Left lower left

								-1.0, +0.0, +0.0,	// Right upper left
								-1.0, +0.0, +0.0,	// Right upper right
								-1.0, +0.0, +0.0,	// Right lower right
								-1.0, +0.0, +0.0,	// Right lower left
							   };
	//Copy the data into the VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->_normBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSize*sizeof(GLfloat), normData, GL_STATIC_DRAW);
}


void WCVisMotionTracker::Initialize(void) {
	//Check feature name
	if (this->_name == "") this->_name = this->_visualization->GenerateFeatureName(this);
	//Set the render program for the layer
	this->_renderProg =  this->_visualization->Scene()->ShaderManager()->ProgramFromName("scn_basiclight");

	//Create the buffers
	glGenBuffers(1, &this->_vertBuffer);
	glGenBuffers(1, &this->_normBuffer);
	this->GenerateBuffers();

	//Create the point controller
	this->_controller = new WCVisMotionTrackerController(this);
	//Add into visualization
	if (!this->_visualization->AddFeature(this, true)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisMotionTracker::Initialize - Problem adding feature to visualization.");
		//Should delete base
	}
	//Create tree element and add into the tree (beneath the features element)
	WSTexture *icon = this->_document->Scene()->TextureManager()->TextureFromName("translate32");
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, icon);
	this->_visualization->FeaturesTreeElement()->AddLastChild(this->_treeElement);

	//Reset the visualization
	this->ResetVisualization();
}


/***********************************************~***************************************************/


WCVisMotionTracker::WCVisMotionTracker(WCVisualization *vis, const std::string &name, const unsigned int &id, 
	WCVisMesh *mesh, const WPFloat &scale) : ::WCVisFeature(vis, name, id),
	_position(0.0, 0.0, 0.0), _rotation(0.0, 0.0, 0.0), _initPosition(0.0, 0.0, 0.0), _initRotation(0.0, 0.0, 0.0),
	_scale(scale), _mesh(mesh), _vertBuffer(0), _normBuffer(0) {
	//Load the visualization mesh
	//...
	//Initialize the object
	this->Initialize();
}


WCVisMotionTracker::WCVisMotionTracker(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCVisFeature( WCSerializeableObject::ElementFromName(element,"VisFeature"), dictionary),
	_position(0.0, 0.0, 0.0), _rotation(0.0, 0.0, 0.0), _initPosition(0.0, 0.0, 0.0), _initRotation(0.0, 0.0, 0.0),
	_scale(1.0), _mesh(NULL), _vertBuffer(0), _normBuffer(0) {
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Get the mesh name
	std::string meshName = WCSerializeableObject::GetStringAttrib(element, "mesh");
	//Setup scale
	this->_scale = WCSerializeableObject::GetFloatAttrib(element, "scale");
	//Setup initial position
	this->_initPosition.FromElement(WCSerializeableObject::ElementFromName(element,"InitialPosition"));
	//Setup initi rotation
	this->_initRotation.FromElement(WCSerializeableObject::ElementFromName(element,"InitialRotation"));

	//Initialize the object
	this->Initialize();
}


WCVisMotionTracker::~WCVisMotionTracker() {
	//Remove from the visualization
	if (!this->_visualization->RemoveFeature(this, true)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisMotionTracker::~WCVisMotionTracker - Problem removing feature from visualization.");	
	}
	//If there are any remaining references, send out a broadcast
	if (this->_refSet.size() > 0) this->SendBroadcastNotice(OBJECT_NOTIFY_DELETE);
}


void WCVisMotionTracker::OnReceiveData(const unsigned int &type, void* data) {
	//Make sure data is not null
	if (data == NULL) return;

	//Lock the tracker
	this->Lock();

	//Copy data into the packet structure
	WSVisualizationPacket packet;
	unsigned int size = sizeof(WSVisualizationPacket) - sizeof(WSVisualizationHeader);
	memcpy(&(packet.sequence), data, size);

	//Set the visualization values
	this->_position.Set( packet.xPosition, packet.yPosition, packet.zPosition );
	this->_rotation.Set( packet.xAngle, packet.yAngle, packet.zAngle );

	//Free the data
	free(data);

	//Unlock the tracker
	this->Unlock();
}


void WCVisMotionTracker::StartVisualization(void) {
}


void WCVisMotionTracker::PauseVisualization(void) {
}


void WCVisMotionTracker::StopVisualization(void) {
}


void WCVisMotionTracker::ResetVisualization(void) {
	//Reset position
	this->_position.Set(0.0, 0.0, 0.0);
	//Reset rotation
	this->_rotation.Set(0.0, 0.0, 0.0);
}


void WCVisMotionTracker::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Nothing to do for now...
}


bool WCVisMotionTracker::Regenerate(void) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCVisMotionTracker::Regenerate - Not yet implemented.");
	return false;
}


void WCVisMotionTracker::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Mark this as selected
	this->_isSelected = true;
}


void WCVisMotionTracker::OnDeselection(const bool fromManager) {
	//Mark this as not selected
	this->_isSelected = false; 
}


void WCVisMotionTracker::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Push the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//Lock the tracker
	this->Lock();
	
	//Set mesh position
	glTranslated(this->_position.I(), this->_position.J(), 1.0);
	//Set mesh rotation (just z angle for now)
	glRotated(this->_rotation.K(), 0.0, 0.0, 1.0);
	//Set mesh scale
	GLfloat scale = (GLfloat)this->_scale;
	glScalef(scale, scale, scale);
	
	//Unlock the tracker
	this->Unlock();

	//Set the mesh color
	if (color != WCColor::Default()) color.Enable();
	else if (this->_isSelected)	glColor4f(1.0, 1.0, 0.0, 1.0);
	else glColor4f(1.0, 1.0, 1.0, 1.0);

	//Set the shader
	if (this->_renderProg != 0)	glUseProgram(this->_renderProg);

	//Set up vertex buffer state
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	//Set up normal buffer state
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_normBuffer);
	glNormalPointer(GL_FLOAT, 0, 0);

	//Draw the mesh
	glDrawArrays(GL_QUADS, 0, 24);

	//Bind back to nothing
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Make sure that vertex and normal arrays are disabled
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	//Restore the modelview matrix and render program
	glUseProgram(0);
	glPopMatrix();
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCVisMotionTracker &tracker) {
	std::cout << "VisMotionTracker(" << &tracker << ")\n";
	return out;
}


/***********************************************~***************************************************/