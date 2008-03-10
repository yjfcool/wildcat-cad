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
#include "PartDesign/part_plane.h"
#include "PartDesign/part_plane_controller.h"
#include "PartDesign/part.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


void WCPartPlane::GenerateVBO(void) {
	GLfloat invZoom = 1.0 / this->_document->Scene()->ActiveCamera()->Zoom();
	GLfloat zoom = invZoom * PARTPLANE_SCALE;

	//Calculate the corners
	WCVector4 ll = this->_base - this->_uAxis*zoom - this->_vAxis*zoom;
	WCVector4 lr = this->_base + this->_uAxis*zoom - this->_vAxis*zoom;
	WCVector4 ul = this->_base - this->_uAxis*zoom + this->_vAxis*zoom;
	WCVector4 ur = this->_base + this->_uAxis*zoom + this->_vAxis*zoom;
	
	//Setup the array
	GLfloat data[16];
	data[0] = (GLfloat)ll.I();
	data[1] = (GLfloat)ll.J();
	data[2] = (GLfloat)ll.K();
	data[3] = 1.0;
	data[4] = (GLfloat)ul.I();
	data[5] = (GLfloat)ul.J();
	data[6] = (GLfloat)ul.K();
	data[7] = 1.0;
	data[8] = (GLfloat)ur.I();
	data[9] = (GLfloat)ur.J();
	data[10] = (GLfloat)ur.K();
	data[11] = 1.0;
	data[12] = (GLfloat)lr.I();
	data[13] = (GLfloat)lr.J();
	data[14] = (GLfloat)lr.K();
	data[15] = 1.0;
	
	//Copy the data into the VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	glBufferData(GL_ARRAY_BUFFER, 16*sizeof(GLfloat), data, GL_STATIC_DRAW);
}


void WCPartPlane::GenerateMatrices(void) {
	//Setup matrix
	WCVector4 wAxis = this->_uAxis.CrossProduct(this->_vAxis).Normalize();
	WCMatrix4 mat(this->_uAxis.I(), this->_vAxis.I(), wAxis.I(), this->_base.I(),
				 this->_uAxis.J(), this->_vAxis.J(), wAxis.J(), this->_base.J(),
				 this->_uAxis.K(), this->_vAxis.K(), wAxis.K(), this->_base.K(),
				 0.0, 0.0, 0.0, 1.0);
	this->_matrix = mat;
	//Calculate inverse
	this->_inverseMatrix = mat.Inverse();
}


/***********************************************~***************************************************/


WCPartPlane::WCPartPlane(WCFeature *creator, const std::string &name, const WCVector4 &p0, const WCVector4 &p1, const WCVector4 &p2) : 
	::WCPartFeature(creator, name), ::WCVisualObject(), _buffer(0), _base(p0), _uAxis(), _vAxis(), _matrix(), _inverseMatrix() {

	//Check feature name
	if (this->_name == "") this->_name = this->_part->GenerateFeatureName(this);
	//Create event handler
	this->_controller = new WCPartPlaneController(this);
	//Create tree element
	WSTexture* planeIcon = this->_document->Scene()->TextureManager()->TextureFromName("plane32");
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, planeIcon);
	//Add tree view element
	this->_creator->TreeElement()->AddLastChild(this->_treeElement);
	
	//Add the plane to the part (true for visualize and select)
	this->_part->AddFeature(this, true);
	//Set default visibility
	this->_isVisible = PARTPLANE_DEFAULT_VISIBILITY;
	
	//Generate the VBO
	glGenBuffers(1, &this->_buffer);
	//Mark as dirty
	this->_isFeatureDirty = true;
	//Retain the the camera) (get ModelView updates)
	this->_document->Scene()->ActiveCamera()->Retain(*this);
	
	//uAxis, and vAxis
	this->_uAxis = p1 - p0;
	this->_uAxis.Normalize();
	WCVector4 vAxis = p2 - p0;
	vAxis.Normalize();
	WCVector4 wAxis = this->_uAxis.CrossProduct(vAxis);
	wAxis.Normalize();
	this->_vAxis = wAxis.CrossProduct(this->_uAxis);
	this->_vAxis.Normalize();
	//Generate the matrices
	this->GenerateMatrices();
}


WCPartPlane::WCPartPlane(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCPartFeature( WCSerializeableObject::ElementFromName(element,"PartFeature"), dictionary), 
	::WCVisualObject(), _buffer(0), _base(), _uAxis(), _vAxis(), _matrix(), _inverseMatrix() {

	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Create event handler
	this->_controller = new WCPartPlaneController(this);
	//Create tree element
	WSTexture* planeIcon = this->_document->Scene()->TextureManager()->TextureFromName("plane32");
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, planeIcon);
	//Add tree view element
	this->_creator->TreeElement()->AddLastChild(this->_treeElement);
	
	//Add the plane to the part (true for visualize and select)
	this->_part->AddFeature(this, true);
	//Set default visibility
	this->_isVisible = PARTPLANE_DEFAULT_VISIBILITY;
	
	//Generate the VBO
	glGenBuffers(1, &this->_buffer);
	//Mark as dirty
	this->_isFeatureDirty = true;
	//Retain the the camera) (get ModelView updates)
	this->_document->Scene()->ActiveCamera()->Retain(*this);
	
	//Base, uAxis, and vAxis
	this->_base.FromElement(  WCSerializeableObject::ElementFromName(element,"Base") );
	this->_uAxis.FromElement( WCSerializeableObject::ElementFromName(element,"UAxis") );
	this->_vAxis.FromElement( WCSerializeableObject::ElementFromName(element,"VAxis") );
	//Generate the matrices
	this->GenerateMatrices();
}


WCPartPlane::~WCPartPlane() {
	//Remove from the part
	if (!this->_part->RemoveFeature(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartPlane::~WCPartPlane - Problem removing feature from part.");	
	}
	//Delete the VBO
	glDeleteBuffers(1, &this->_buffer);
}


void WCPartPlane::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
//	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartPlane::ReceiveNotice - Not yet implemented.");
	//Mark as dirty
	this->_isVisualDirty = true;
}


bool WCPartPlane::Regenerate(void) {
	return false;
}


xercesc::DOMElement* WCPartPlane::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("Plane");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the part feature element
	xercesc::DOMElement* featureElement = this->WCPartFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	
	//Add the base vector
	xmlString = xercesc::XMLString::transcode("Base");
	xercesc::DOMElement* baseElement = document->createElement(xmlString);
	this->_base.ToElement(baseElement);
	element->appendChild(baseElement);
	xercesc::XMLString::release(&xmlString);

	//Add the uAxis vector
	xmlString = xercesc::XMLString::transcode("UAxis");
	xercesc::DOMElement* uAxisElement = document->createElement(xmlString);
	this->_uAxis.ToElement(uAxisElement);
	element->appendChild(uAxisElement);
	xercesc::XMLString::release(&xmlString);
	
	//Add the vAxis vector
	xmlString = xercesc::XMLString::transcode("VAxis");
	xercesc::DOMElement* vAxisElement = document->createElement(xmlString);
	this->_vAxis.ToElement(vAxisElement);
	element->appendChild(vAxisElement);
	xercesc::XMLString::release(&xmlString);

	//Return the primary element
	return element;
}


void WCPartPlane::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Make sure to check if is visible
	if (!this->_isVisible) return;
	//Check if dirty
	if (this->_isVisualDirty) {
		this->GenerateVBO();
		this->GenerateMatrices();
		//Mark as clean
		this->_isVisualDirty = false;
	}
	else glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);

	//Set up rendering environment
	glEnableClientState(GL_VERTEX_ARRAY);	
	glVertexPointer(4, GL_FLOAT, 0, 0);
	glEnable(GL_LINE_SMOOTH);
	//Set the color and thickness appropriately
	if (color == WCColor::Default()) this->_color.Enable();
	else color.Enable();
	glLineWidth(PARTPLANE_LINE_THICKNESS);
	//Draw the geometry
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	
	//Clean up the environment
	glDisable(GL_LINE_SMOOTH);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
}


void WCPartPlane::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Change the color
	this->_color = WCPartFeature::SelectedColor;
	//Mark as selected
	this->_isSelected = true;
}


void WCPartPlane::OnDeselection(const bool fromManager) {
	//Change the color
	this->_color.Set(1.0, 1.0, 1.0, 1.0);
	//Mark as not selected
	this->_isSelected = false;
}


/***********************************************~***************************************************/

	
WCActionPartPlaneCreate* WCPartPlane::ActionCreate(WCFeature *creator, const std::string &planeName, const WCVector4 &p0, 
	const WCVector4 &p1, const WCVector4 &p2) {
	//Create new action for plane creation
	return new WCActionPartPlaneCreate(creator, planeName, p0, p1, p2);
}



/***********************************************~***************************************************/
	

std::ostream& operator<<(std::ostream& out, const WCPartPlane &plane) {
	return out;
}


/***********************************************~***************************************************/


