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
#include "Geometry/geometric_point.h"
#include "Geometry/ray.h"


/***********************************************~***************************************************/


WCGeometricPoint::WCGeometricPoint(const WPFloat &x, const WPFloat &y, const WPFloat &z, const WPFloat &w) : 
	::WCGeometricObject(), _data(x, y, z, w), _size(POINT_POINTSIZE) {
	//Establish aligned bounding box
	this->_bounds = new WCAlignedBoundingBox(&this->_data, 1);
}


WCGeometricPoint::WCGeometricPoint(const WCVector4 &vec) : 
	::WCGeometricObject(), _data(vec), _size(POINT_POINTSIZE) {
	//Establish aligned bounding box
	this->_bounds = new WCAlignedBoundingBox(&this->_data, 1);
}


WCGeometricPoint::WCGeometricPoint(const WCGeometricPoint &point) : 
	::WCGeometricObject(), _data(point._data), _size(POINT_POINTSIZE) {
	//Establish aligned bounding box
	this->_bounds = new WCAlignedBoundingBox(&this->_data, 1);
}


WCGeometricPoint::WCGeometricPoint(const WPUInt &size, const WPFloat *data) : 
	::WCGeometricObject(), _data(size, data), _size(POINT_POINTSIZE) {
	//Establish aligned bounding box
	this->_bounds = new WCAlignedBoundingBox(&this->_data, 1);
}


WCGeometricPoint::WCGeometricPoint(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCGeometricObject( WCSerializeableObject::ElementFromName(element,"GeometricObject"), dictionary),
	_data(), _size(POINT_POINTSIZE) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometricPoint::WCGeometricPoint - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);
	//Get the size value
	this->_size = WCSerializeableObject::GetFloatAttrib(element, "size");
	//Get the base vector
	this->_data.FromElement( WCSerializeableObject::ElementFromName(element,"Base") );
	//Establish aligned bounding box
	this->_bounds = new WCAlignedBoundingBox(&this->_data, 1);
}


WCGeometricPoint::~WCGeometricPoint() {
	//Nothing else to do for now
}

	
void WCGeometricPoint::Set(const WPFloat &x, const WPFloat &y, const WPFloat &z, const WPFloat &w) {
	//Update variable
	this->_data.Set(x, y, z, w);
	//Mark as dirty
	this->IsSerialDirty(true);
	//Update the bounding box
	this->_bounds->Set(&this->_data, 1);
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCGeometricPoint::Set(const WCVector4 &vector) {
	//Update variable
	this->_data = vector;
	//Mark as dirty
	this->IsSerialDirty(true);
	//Update the bounding box
	this->_bounds->Set(&this->_data, 1);
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCGeometricPoint::X(const WPFloat &x) {
	//Update variable
	this->_data.I(x);
	//Mark as dirty
	this->IsSerialDirty(true);
	//Update the bounding box
	this->_bounds->Set(&this->_data, 1);
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCGeometricPoint::Y(const WPFloat &y) {
	//Update variable
	this->_data.J(y);
	//Mark as dirty
	this->IsSerialDirty(true);
	//Update the bounding box
	this->_bounds->Set(&this->_data, 1);
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCGeometricPoint::Z(const WPFloat &z) {
	//Update variable
	this->_data.K(z);
	//Mark as dirty
	this->IsSerialDirty(true);
	//Update the bounding box
	this->_bounds->Set(&this->_data, 1);
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCGeometricPoint::W(const WPFloat &w) {
	//Update variable
	this->_data.L(w);
	//Mark as dirty
	this->IsSerialDirty(true);
	//Update the bounding box
	this->_bounds->Set(&this->_data, 1);
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCGeometricPoint::Color(const WCColor &color) {
	//Set the color
	this->_color = color;
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


WCVisualObject* WCGeometricPoint::HitTest(const WCRay &ray, const WPFloat &tolerance) {
	WCVector4 x1 = ray.Base();
	WCVector4 x2 = x1 + ray.Direction() * POINT_HITTEST_RAYCAST;
	WCVector4 x0 = this->_data;
	
	//Calculate distance from ray to point
	WCVector4 x1mx0 = x1 - x0;
	WCVector4 x2mx1 = x2 - x1;
	WCVector4 crs = x2mx1.CrossProduct(x1mx0);
	WPFloat top = crs.Magnitude();
	WPFloat btm = x2mx1.Magnitude();
	WPFloat dist = top/btm;
//	std::cout << "Dist: " << dist << std::endl;
	//Check to see if distance is within tolerance
	if (dist <= tolerance) {
//		std::cout << "T Value: " << dist << std::endl;	
		top = x1mx0.DotProduct(x2mx1);
		btm = btm * btm;
		dist = -top / btm;
//		std::cout << "U Value: " << dist << std::endl;
		return this;
	}
	return NULL;
}

void WCGeometricPoint::ApplyTransform(const WCMatrix4 &transform) {
	//Update the position of the point
	this->_data = transform * this->_data;
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCGeometricPoint::ApplyTranslation(const WCVector4 &translation) {
	//Update the position of the point
	this->_data = this->_data + translation;
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCGeometricPoint::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Make sure is visible
	if (!this->_isVisible) return;
	//Set color appropriately
	if (color == WCColor::Default()) this->_color.Enable();
	else color.Enable();
	//Setup the render program
	if (this->_renderProg != 0) glUseProgram(this->_renderProg);
	else if(defaultProg != 0)glUseProgram(defaultProg);
	//Set point size
	glPointSize((GLfloat)this->_size);
	GLfloat data[3] = { (GLfloat)this->X(), (GLfloat)this->Y(), (GLfloat)this->Z() };
	glEnableClientState(GL_VERTEX_ARRAY);
	//Draw the line
	glVertexPointer(3, GL_FLOAT, 0, data);
	glDrawArrays(GL_POINTS, 0, 1);
	//Clean up state
	glDisableClientState(GL_VERTEX_ARRAY);
}


bool WCGeometricPoint::Intersect(WCGeometricPoint &point, const WPFloat &tolerance) {
	return this->_data.Distance(point._data) <= tolerance;
}


WCGeometricPoint WCGeometricPoint::operator+(const WCGeometricPoint &point) {
	return WCGeometricPoint(this->_data + point._data);
}


WCGeometricPoint WCGeometricPoint::operator-(const WCGeometricPoint &point) {
	return WCGeometricPoint(this->_data - point._data);
}


WCGeometricPoint WCGeometricPoint::operator*(const WPFloat &scalar) {
	return WCGeometricPoint(this->_data * scalar);
}


WCGeometricPoint WCGeometricPoint::operator/(const WPFloat &scalar) {
	return WCGeometricPoint(this->_data / scalar);
}


WCGeometricPoint& WCGeometricPoint::operator=(const WCGeometricPoint &point) {
	this->_data = point._data;
	return *this;
}


bool WCGeometricPoint::operator==(const WCGeometricPoint &point) {
	//Use distance within epsilon
	WPFloat distance = this->_data.Distance(point._data);
	return (distance < POINT_EQUALITY_EPSILON);
}


xercesc::DOMElement* WCGeometricPoint::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("GeometricPoint");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Include the parent element
	xercesc::DOMElement* geometricElement = this->WCGeometricObject::Serialize(document, dictionary);
	element->appendChild(geometricElement);

	//Add Size attribute
	WCSerializeableObject::AddFloatAttrib(element, "size", this->_size);
	//Add base vector
	this->_data.ToElement(element, "Base");

	//Return the element
	return element;
}


/***********************************************~***************************************************/


std::vector<WCGeometricPoint*> WCGeometricPoint::VectorFromArray(const WPUInt &count, const WPFloat *data) {
	//Check to make sure data is not null
	if (data == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometricPoint::CollectionFromArray - Null data value passed.");
		//Return nothing
		return std::vector<WCGeometricPoint*>();
	}
	//Create a new collection
	std::vector<WCGeometricPoint*> collection;
	WCGeometricPoint *point;
	//Create points from the data and add to the collection
	for (WPUInt i=0; i<count; i++) {
		//Create a new point
		point = new WCGeometricPoint(data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
		//Add it to the vector
		collection.push_back(point);
	}
	return collection;
}


/***********************************************~***************************************************/
		
	
std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCGeometricPoint &point) {
	//Print out basic surface info
	out << "GeometricPoint(" << (void*)&point << ") {" << point.X() << ", " << point.Y() << ", " << point.Z() << ", " << point.W() << "}\n";
	return out;
}


/***********************************************~***************************************************/




