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
#include <Constraint/constraint_vertical.h>
#include <Sketcher/sketch.h>
#include <Sketcher/sketch_line.h>
#include <Kernel/document.h>
#include <PartDesign/part_plane.h>


/***********************************************~***************************************************/


void WCConstraintVertical::GenerateBuffer(void) {
	//Ok, setup everything
	WPFloat iconSize = 16 * SCREEN_PIXEL_WIDTH;
	WCVector4 pos = this->_sketch->ReferencePlane()->InverseTransformMatrix() * this->_line->Base()->Evaluate(0.5);
	WPFloat x = pos.I() + 0.01;
	WPFloat y = pos.J() + 0.01;
	//Calculate box slighly above and right of location
	WCVector4 ll(x, y, 0.0, 1.0);
	WCVector4 xUnit(1.0, 0.0, 0.0, 0.0);
	WCVector4 yUnit(0.0, 1.0, 0.0, 0.0);

	//Project from the plane (adjust for zoom factor to maintain icon size)
	ll = this->_sketch->ReferencePlane()->TransformMatrix() * ll;
	xUnit = this->_sketch->ReferencePlane()->TransformMatrix() * xUnit;
	yUnit = this->_sketch->ReferencePlane()->TransformMatrix() * yUnit;
	xUnit = xUnit * (iconSize / this->_sketch->Document()->Scene()->ActiveCamera()->Zoom());
	yUnit = yUnit * (iconSize / this->_sketch->Document()->Scene()->ActiveCamera()->Zoom());
	WCVector4 ul = ll + yUnit;
	WCVector4 ur = ll + xUnit + yUnit;
	WCVector4 lr = ll + xUnit;

	//Set up vertex data arrays
	this->_vertData[0] = (GLfloat)ll.I();
	this->_vertData[1] = (GLfloat)ll.J();
	this->_vertData[2] = (GLfloat)ll.K();
	this->_vertData[3] = (GLfloat)ul.I();
	this->_vertData[4] = (GLfloat)ul.J();
	this->_vertData[5] = (GLfloat)ul.K();
	this->_vertData[6] = (GLfloat)ur.I();
	this->_vertData[7] = (GLfloat)ur.J();
	this->_vertData[8] = (GLfloat)ur.K();
	this->_vertData[9] = (GLfloat)lr.I();
	this->_vertData[10] = (GLfloat)lr.J();
	this->_vertData[11] = (GLfloat)lr.K();

	//Setup texCoord data array
	GLfloat texCoords[8] = { 0.0, this->_texture->_height, 
							 0.0, 0.0, 
							 this->_texture->_width, 0.0, 
							 this->_texture->_width, this->_texture->_height };
	memcpy(this->_texData, texCoords, 8 * sizeof(GLfloat));
	
	//Mark as clean
	this->IsVisualDirty(false);
}


void WCConstraintVertical::Initialize(void) {
	//Create the constraint controller
	this->_controller = new WCConstraintVerticalController(this);
	//Make sure base is not null
	if ((this->_line == NULL) || (this->_controller == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintVertical::Initialize - NULL Line or controller.");
		//throw error
		return;
	}
	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	//Retain the line
	this->_line->Retain(*this);
	//Add into sketch
	if (!this->_sketch->AddConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintVertical::Initialize - Problem adding constraint to sketch.");
		//Should delete base
		//throw error
		return;
	}
	//Setup the icon texture
	this->_texture = this->_document->Scene()->TextureManager()->TextureFromName("vertical32");
	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture* hvIcon = this->_document->Scene()->TextureManager()->TextureFromName("hvConstraint32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, hvIcon);
	this->_sketch->ConstraintsTreeElement()->AddLastChild(this->_treeElement);
	//Inject constraints into sketch planner
	this->InjectConstraints(this->_sketch->ConstraintPlanner());
	//Mark as dirty
	this->IsVisualDirty(true);
}


/***********************************************~***************************************************/


WCConstraintVertical::WCConstraintVertical(WCSketch *sketch, const std::string &name, WCSketchLine *line) :
	::WCSketchConstraint(sketch, name), _line(line), _texture(NULL) {
	//Set the color
	this->_color = WCSketchFeature::ConstraintColor;
	//Initialize
	this->Initialize();
}


WCConstraintVertical::WCConstraintVertical(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	WCSketchConstraint( WCSerializeableObject::ElementFromName(element, "SketchConstraint"), dictionary),
	_line(NULL), _texture(NULL) {
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Setup line
	this->_line = (WCSketchLine*)WCSerializeableObject::GetGUIDAttrib(element, "line", dictionary);
	//Initialize the object
	this->Initialize();
}


WCConstraintVertical::~WCConstraintVertical() {
	//Release the line
	this->_line->Release(*this);
	//Remove from the sketch
	if (!this->_sketch->RemoveConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintVertical::~WCConstraintVertical - Problem removing constraint from sketch.");	
	}
}


void WCConstraintVertical::InjectConstraints(WCConstraintPlanner *planner) {
	//Get the list of constraint nodes from the line
	std::list<WCConstraintNode*> nodeList = this->_line->ConstraintNodes();
	//Make sure there are three nodes
	if (nodeList.size() != 3) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintVertical::InjectConstraints - Not three nodes in line's list.");
		return;
	}
	std::list<WCConstraintNode*>::iterator iter = nodeList.begin();
	//Get the starting point (second node in list) and ending point (third node in list) of the line
	iter++;
	WCConstraintNode *p0Node = *iter++;
	WCConstraintNode *p1Node = *iter;
	//Add a constraint edge to the planner
	planner->AddEdge(this, p0Node, p1Node, WCConstraintEdgeType::HorizontalDistance(), 1, WCVector4(0.0));
}


void WCConstraintVertical::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark as dirty
	this->IsVisualDirty(true);
}


bool WCConstraintVertical::Regenerate(void) {
	return false;
}


void WCConstraintVertical::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Setup the icon texture
	this->_texture = this->_document->Scene()->TextureManager()->TextureFromName("verticalSelected32");
	//Mark this as selected
	this->_isSelected = true;
}


void WCConstraintVertical::OnDeselection(const bool fromManager) {
	//Setup the icon texture
	this->_texture = this->_document->Scene()->TextureManager()->TextureFromName("vertical32");
	//Mark this as not selected
	this->_isSelected = false;
}


void WCConstraintVertical::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Do a visibility check
	if (!this->_isVisible) return;
	//Regenerate the measure if necessary
	if (this->IsVisualDirty()) this->GenerateBuffer();	

	//Enable state
	glDisable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, this->_vertData);
	//Check the color
	if (color == WCColor::Default()) {
		//Render textured quad
		glEnable(this->_texture->_target);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		//Draw textured quad
		glBindTexture(this->_texture->_target, this->_texture->_id);
		glTexCoordPointer(2, GL_FLOAT, 0, this->_texData);
		glDrawArrays(GL_QUADS, 0, 4);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		//Clean up a bit
		glBindTexture(this->_texture->_target, 0);
		glDisable(this->_texture->_target);
	}
	//Selection manager - must render color quad
	else {
		color.Enable();
		glDrawArrays(GL_QUADS, 0, 4);
	}
	//Clean up state
	glEnable(GL_DEPTH_TEST);
	glDisableClientState(GL_VERTEX_ARRAY);
}

	
xercesc::DOMElement* WCConstraintVertical::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("ConstraintVertical");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCSketchConstraint::Serialize(document, dictionary);
	element->appendChild(featureElement);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add line attribute
	WCSerializeableObject::AddGUIDAttrib(element, "line", this->_line, dictionary);

	//Return the new element
	return element;
}


/***********************************************~***************************************************/


WCActionConstraintVerticalCreate* WCConstraintVertical::ActionCreate(WCSketch *sketch, const std::string &constraintName, WCSketchLine *line) {
	//Create new action to create constraint
	return new WCActionConstraintVerticalCreate(sketch, constraintName, line);
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCConstraintVertical &constraint) {
	std::cout << "Constraint Vertical(" << &constraint << ")\n";
	return out;
}


/***********************************************~***************************************************/

