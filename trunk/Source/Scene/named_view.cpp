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
#include "Scene/named_view.h"


/***********************************************~***************************************************/


WCNamedView::WCNamedView(const std::string name, const WCVector4 &origin, const WCVector4 &target, bool isOrtho, WPFloat angle) :
	::WCSerializeableObject(), _name(name), _isOrtho(isOrtho), _isAuto(true), _origin(origin), _target(target), _angle(angle), 
	_zoom(1.0), _panX(0.0), _panY(0.0), _quaternion() {
	//Nothing else for now
	this->Retain(*this);
}


WCNamedView::WCNamedView(const std::string name, const WCQuaternion &quat, bool isOrtho, WPFloat angle) : ::WCSerializeableObject(),
	_name(name), _isOrtho(isOrtho), _isAuto(true), _origin(), _target(), _angle(angle), 
	_zoom(1.0), _panX(0.0), _panY(0.0), _quaternion(quat) {
	//Nothing else for now
	this->Retain(*this);
}


WCNamedView::WCNamedView(const WCNamedView &view) : ::WCSerializeableObject(), 
	_name(view._name), _isOrtho(view._isOrtho), _isAuto(view._isAuto), _origin(view._origin), _target(view._target), 
	_angle(view._angle), _zoom(view._zoom), _panX(view._panX), _panY(view._panX), _quaternion(view._quaternion) {
	//Nothing else for now
	this->Retain(*this);
}


WCNamedView::WCNamedView(xercesc::DOMElement* element, WCSerialDictionary *dictionary) : ::WCSerializeableObject(),
	_name(""), _isOrtho(true), _isAuto(true), _origin(), _target(), _angle(0.0), 
	_zoom(1.0), _panX(0.0), _panY(0.0), _quaternion() {

	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Get the name
	this->_name = WCSerializeableObject::GetStringAttrib(element, "name");
	//Get the panX attribute
	this->_panX = WCSerializeableObject::GetFloatAttrib(element, "panx");
	//Get the panY attribute
	this->_panY = WCSerializeableObject::GetFloatAttrib(element, "pany");
	//Get the zoom attribute
	this->_zoom = WCSerializeableObject::GetFloatAttrib(element, "zoom");
	//Get the isAuto attribute
	this->_isAuto = WCSerializeableObject::GetBoolAttrib(element, "auto");
	//Get the isOrtho attribute
	this->_isOrtho = WCSerializeableObject::GetBoolAttrib(element, "ortho");
	//Get the quaternion object
	this->_target.FromElement( WCSerializeableObject::ElementFromName(element,"Target") );
	//Get the quaternion object
	this->_origin.FromElement( WCSerializeableObject::ElementFromName(element,"Origin") );
	//Get the quaternion object
	this->_quaternion.FromElement( WCSerializeableObject::ElementFromName(element,"Quaternion") );
}


WCNamedView::~WCNamedView() {
	//Release self
	this->Release(*this);
}


WCNamedView& WCNamedView::operator=(const WCNamedView &view) {
	//Copy every component
//	this->_name = view._name;
	this->_isOrtho = view._isOrtho;
	this->_isAuto = view._isAuto;
	this-> _origin = view._origin;
	this->_target = view._target; 
	this->_angle = view._angle;
	this->_zoom = view._zoom;
	this->_panX = view._panX;
	this->_panY = view._panY;
	this->_quaternion = view._quaternion;
	return *this;
}


xercesc::DOMElement* WCNamedView::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("NamedView");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add base object
//	xercesc::DOMElement* baseElement = this->WCSerializeableObject::Serialize(document);
//	element->appendChild(baseElement);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add Name attribute
	WCSerializeableObject::AddStringAttrib(element, "name", this->_name);
	//Add isOrtho attribute
	WCSerializeableObject::AddBoolAttrib(element, "ortho", this->_isOrtho);
	//Add isAuto attribute
	WCSerializeableObject::AddBoolAttrib(element, "auto", this->_isAuto);
	//Add Angle attribute
	WCSerializeableObject::AddFloatAttrib(element, "angle", this->_angle);
	//Add Zoom attribute
	WCSerializeableObject::AddFloatAttrib(element, "zoom", this->_zoom);
	//Add panX attribute
	WCSerializeableObject::AddFloatAttrib(element, "panx", this->_panX);
	//Add panY attribute
	WCSerializeableObject::AddFloatAttrib(element, "pany", this->_panY);
	//Add Origin vector
	this->_origin.ToElement(element, "Origin");
	//Add Target vector
	this->_target.ToElement(element, "Target");
	//Add Quaternion
	this->_quaternion.ToElement(element, "Quaternion");

	//Return the primary element
	return element;
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCNamedView &view) {
	//Print out some info
	out << "NamedView(" << &view << ") : " << view._name << std::endl;
	out << "\tAuto: "; view._isAuto?out<<"True":out<<"False"; out<<std::endl;
	out << "\tOrtho: "; view._isOrtho?out<<"True":out<<"False"; out<<std::endl;
	//if not using origin-target print out pan, zoom, quat
	if (WCVector4() == view._origin) {
		out << "\tZoom: " << view._zoom << std::endl;
		out << "\tPanX: " << view._panX << std::endl;
		out << "\tPanY: " << view._panY << std::endl;
		out << "\t" << view._quaternion << std::endl;
	}
	//Otherwise, print origin, target, zoom
	else {
		out << "Blah, blah, blah\n";
	}
	return out;
}


/***********************************************~***************************************************/

