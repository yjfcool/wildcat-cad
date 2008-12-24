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
#include <Constraint/constraint_measure.h>
#include <Kernel/document.h>


/***********************************************~***************************************************/


WCConstraintMeasure::WCConstraintMeasure(WCSketchFeature *parent, const std::string &label, const WCMatrix4 &toPlane,
	const WCMatrix4 &fromPlane,	const WPFloat offset, const WPFloat labelOffset) : ::WCSerializeableObject(), 
	_parent(parent), _toPlane(toPlane), _fromPlane(fromPlane), _offset(offset), _labelOffset(labelOffset),
	_label(label), _text(NULL), _labelPos(), _uUnit(), _vUnit(), _isDirty(true) {
	//Create text for the label
	this->_text = new WCText(this->_parent->Document()->Scene(), this->_label, 
							 WCColor(WCSketchFeature::DefaultTextColor), WCTextFont::Times(), WCTextStyle::Roman(), 18.0);
}


WCConstraintMeasure::WCConstraintMeasure(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : ::WCSerializeableObject(),
	_parent(NULL), _toPlane(), _fromPlane(), _offset(0.0), _labelOffset(0.0),
	_label(""), _text(NULL), _labelPos(), _uUnit(), _vUnit(), _isDirty(true) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Get parent attribute
	this->_parent = (WCSketchFeature*)WCSerializeableObject::GetGUIDAttrib(element, "parent", dictionary);
	//Get offset attribute
	this->_offset = WCSerializeableObject::GetFloatAttrib(element, "offset");
	//Get label offset attribute
	this->_labelOffset = WCSerializeableObject::GetFloatAttrib(element, "labelOffset");
	//Get label attribute
	this->_label = WCSerializeableObject::GetStringAttrib(element, "label");

	//Set toPlane matrix
	this->_toPlane.FromElement( WCSerializeableObject::ElementFromName(element,"ToPlane") );
	//Set fromPlane matrix
	this->_fromPlane.FromElement( WCSerializeableObject::ElementFromName(element,"FromPlane") );
	//Set labelPos vector
	this->_labelPos.FromElement( WCSerializeableObject::ElementFromName(element,"LabelPos") );
	//Set uUnit vector
	this->_uUnit.FromElement( WCSerializeableObject::ElementFromName(element,"UUnit") );
	//Set vUnit vector
	this->_vUnit.FromElement( WCSerializeableObject::ElementFromName(element,"VUnit") );

	//Create text for the label
	this->_text = new WCText(NULL, this->_label, WCColor(WCSketchFeature::DefaultTextColor),
							 WCTextFont::Times(), WCTextStyle::Roman(), 18.0);
}


WCConstraintMeasure::~WCConstraintMeasure() {
	//See if text needs to be deleted
	if (this->_text != NULL) delete this->_text;
}


void WCConstraintMeasure::Offsets(const WPFloat &offset, const WPFloat &labelOffset) {
	//Set the new values
	this->_offset = offset;
	this->_labelOffset = labelOffset;
	//Mark as dirty
	this->_isDirty = true;
}


void WCConstraintMeasure::Matrices(const WCMatrix4 &toPlane, const WCMatrix4 &fromPlane) {
	//Set the new values
	this->_toPlane = toPlane;
	this->_fromPlane = fromPlane;
	//Mark as dirty
	this->_isDirty = true;
}


xercesc::DOMElement* WCConstraintMeasure::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("ConstraintMeasure");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Add parent attribute
	WCSerializeableObject::AddGUIDAttrib(element, "parent", this->_parent, dictionary);
	//Add offset attribute
	WCSerializeableObject::AddFloatAttrib(element, "name", this->_offset);
	//Add label offset attribute
	WCSerializeableObject::AddFloatAttrib(element, "labelOffset", this->_labelOffset);
	//Add label attribute
	WCSerializeableObject::AddStringAttrib(element, "label", this->_label);
	//Add ToPlane
	this->_toPlane.ToElement(element, "ToPlane");
	//Add FromPlane
	this->_fromPlane.ToElement(element, "FromPlane");
	//Add LabelPos
	this->_labelPos.ToElement(element, "LabelPos");
	//Add UUnit
	this->_uUnit.ToElement(element, "UUnit");
	//Add VUnit
	this->_vUnit.ToElement(element, "VUnit");

	//Return the primary element
	return element;
}


/***********************************************~***************************************************/

