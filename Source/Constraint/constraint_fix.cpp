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
#include <Constraint/constraint_fix.h>
#include <Constraint/constraint_edge.h>
#include <Constraint/constraint_node.h>
#include <Sketcher/sketch.h>
#include <Sketcher/sketch_align_suggestion.h>
#include <Sketcher/sketch_point.h>
#include <Sketcher/sketch_line.h>
#include <Kernel/document.h>
#include <PartDesign/part_plane.h>


/***********************************************~***************************************************/


void WCConstraintFix::GenerateBuffer(void) {
	//Ok, setup everything
	WPFloat iconSize = 32 * SCREEN_PIXEL_WIDTH;
	//Determine location based on type
	WCVector4 pos;
	WCSketchPoint *point = dynamic_cast<WCSketchPoint*>(this->_feature);
	//Check point case
	if (point != NULL) pos = point->Base()->Data();
	//Check line case
	else {
		WCSketchLine *line = dynamic_cast<WCSketchLine*>(this->_feature);
		if (line != NULL) pos = line->Base()->Evaluate(0.5);
	}
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
	xUnit = xUnit * iconSize / this->_sketch->Document()->Scene()->ActiveCamera()->Zoom();
	yUnit = yUnit * iconSize / this->_sketch->Document()->Scene()->ActiveCamera()->Zoom();
	WCVector4 ul = ll + yUnit;
	WCVector4 ur = ll + xUnit + yUnit;
	WCVector4 lr = ll + xUnit;

	//Set up data arrays
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
	GLfloat texCoords[8] = { 0.0, 0.0, 
							 0.0, this->_texture->_height, 
							 this->_texture->_width, this->_texture->_height, 
							 this->_texture->_width, 0.0 };
	memcpy(this->_texData, texCoords, 8 * sizeof(GLfloat));
	//Mark as clean
	this->IsVisualDirty(false);
}


/***********************************************~***************************************************/


WCConstraintFix::WCConstraintFix(WCSketch *sketch, const std::string &name, WCSketchFeature *feature) : ::WCSketchConstraint(sketch, name),
	_feature(feature), _texture(NULL) {
	//Create the length controller
	this->_controller = new WCConstraintFixController(this);
	//Make sure base is not null
	if ((this->_feature == NULL) || (this->_controller == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintFix::WCConstraintFix - NULL feature or controller.");
		//throw error
		return;
	}
	
	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	//Retain the feature
	this->_feature->Retain(*this);
	//Make the feature fixed
	this->_feature->IsFixed(true);
	//Add into sketch
	if (!this->_sketch->AddConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintFix::WCConstraintFix - Problem adding constraint to sketch.");
		//Should delete base
		//throw error
		return;
	}
	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture* fixedIcon = this->_document->Scene()->TextureManager()->TextureFromName("fixed32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, fixedIcon);
	this->_sketch->ConstraintsTreeElement()->AddLastChild(this->_treeElement);
	//Inject constraints into sketch planner
	this->InjectConstraints(this->_sketch->ConstraintPlanner());
	//Setup the icon texture
	this->_texture = fixedIcon;
	//Initialize the render data
	this->GenerateBuffer();
}


WCConstraintFix::WCConstraintFix(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSketchConstraint( WCSerializeableObject::ElementFromName(element,"SketchConstraint"), dictionary ),
	_feature(NULL) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);
	//Set the sketch attribute
	this->_feature = (WCSketchFeature*)WCSerializeableObject::GetGUIDAttrib(element, "feature", dictionary);

	//Create the length controller
	this->_controller = new WCConstraintFixController(this);
	//Add into sketch
	if (!this->_sketch->AddConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintFix::WCConstraintFix - Problem adding constraint to sketch.");
		//Should delete base
		//throw error
		return;
	}
	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture* fixedIcon = this->_document->Scene()->TextureManager()->TextureFromName("fixed32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, fixedIcon);
	this->_sketch->ConstraintsTreeElement()->AddLastChild(this->_treeElement);
	//Inject constraints into sketch planner
	this->InjectConstraints(this->_sketch->ConstraintPlanner());
	//Setup the icon texture
	this->_texture = fixedIcon;
	//Initialize the render data
	this->GenerateBuffer();
}


WCConstraintFix::~WCConstraintFix() {
	//Unfix the feature
	this->_feature->IsFixed(false);
	//Release the feature
	this->_feature->Release(*this);
	//Remove from the sketch
	if (!this->_sketch->RemoveConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintFix::~WCConstraintFix - Problem removing constraint from sketch.");	
	}
}


void WCConstraintFix::InjectConstraints(WCConstraintPlanner *planner) {
	//Get the feature
	WCConstraintNode *node = this->_feature->ConstraintNodes().front();
	//Set the node weight to zero
	node->Weight(0);
}


void WCConstraintFix::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Check to see if delete or update
	if (msg == OBJECT_NOTIFY_UPDATE) {
		//Re-initialize the render data
		this->GenerateBuffer();
	}
}


bool WCConstraintFix::Regenerate(void) {
	return false;
}


std::list<WCObject*> WCConstraintFix::DeletionDependencies(void) {
	return std::list<WCObject*>();
}


void WCConstraintFix::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Setup the icon texture
	this->_texture = this->_document->Scene()->TextureManager()->TextureFromName("fixedSelected32");
	//Mark this as selected
	this->_isSelected = true;
}


void WCConstraintFix::OnDeselection(const bool fromManager) {
	//Setup the icon texture
	this->_texture = this->_document->Scene()->TextureManager()->TextureFromName("fixed32");
	//Mark this as selected
	this->_isSelected = false;
}


void WCConstraintFix::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
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
		//Clean up a bit
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(this->_texture->_target, 0);
		glDisable(this->_texture->_target);
	}
	//Selection manager - must render color quad
	else {
		color.Enable();
		glDrawArrays(GL_QUADS, 0, 4);
	}
	//Clean up state
	glDisableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_DEPTH_TEST);
}

	
xercesc::DOMElement* WCConstraintFix::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("ConstraintFix");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the part feature element
	xercesc::DOMElement* featureElement = this->WCSketchConstraint::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Add part attribute
	WCSerializeableObject::AddGUIDAttrib(element, "feature", this->_feature, dictionary);
	//Return the new element
	return element;
}


/***********************************************~***************************************************/


WCAction* WCConstraintFix::ActionCreate(WCSketch *sketch, const std::string &constraintName, WCSketchFeature *feature) {
	//Create new action to create constraint
	return new WCActionConstraintFixCreate(sketch, constraintName, feature);
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCConstraintFix &constraint) {
	std::cout << "Constraint Fix(" << &constraint << ")\n";
	return out;
}


/***********************************************~***************************************************/

