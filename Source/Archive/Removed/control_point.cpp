#include "control_point.h"


/***********************************************~***************************************************/


WCCollection<WCControlPoint*>* WCControlPoint::CollectionFromArray(const WPUInt count, const WPFloat *data) {
	//Check to make sure data is not null
	if (data == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCControlPoint::CollectionFromArray - Null data value passed.");
		//Return nothing
		return NULL;
	}
	//Create a new collection
	WCCollection<WCControlPoint*> *collection = new WCCollection<WCControlPoint*>();
	WCControlPoint *point;
	//Create control points from the data and add to the collection
	for (int i=0; i<count; i++) {
		//Create a new control point
		point = new WCControlPoint(data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
		//Add it to the collection
		collection->Add(point);
	}
	return collection;
}


/***********************************************~***************************************************/
	

WCControlPoint::WCControlPoint(const WPFloat x, const WPFloat y, const WPFloat z, const WPFloat w) :
	:: WCGeometricPoint(x, y, z, w) {
	//Set colors
	this->_color.Set(CONTROLPOINT_DEFAULT_COLOR);
}


WCControlPoint::WCControlPoint(const WCVector4 vec) : ::WCGeometricPoint(vec) {
	//Nothing else to do
}


WCControlPoint::WCControlPoint(const WCControlPoint &point) : ::WCGeometricPoint(point) {
	//Nothing else to do
}


WCControlPoint::WCControlPoint(const WPUInt size, const WPFloat *data) : ::WCGeometricPoint(size, data) {
	//Nothing else to do
}


void WCControlPoint::Render(const GLuint defaultProg, const WCColor color) {
	//Make sure is visible
	if (!this->_isVisible) return;
	//Set color appropriately
	if (color == WCColor::Default()) this->_color.Enable();
	else color.Enable();
	
	//Make sure no programs are active
	if (this->_renderProg != 0) glUseProgram(this->_renderProg);
	else glUseProgram(defaultProg);
	//Set point size
	glPointSize(CONTROLPOINT_POINTSIZE);
	//Render the point
	glBegin(GL_POINTS);
		glVertex3f(this->X(), this->Y(), this->Z());
	glEnd();
	//Restore active program
//	glUseProgram(defaultProg);
}


DOMElement* WCControlPoint::Serialize(DOMDocument *document) {
	//Create the base element for the object
	DOMElement*  pointElem = document->createElement(XMLString::transcode("ControlPoint"));
	//Add attributes
	if (this->_isConstruction)pointElem->setAttribute(XMLString::transcode("IsConstruction"), XMLString::transcode("true"));
	else				pointElem->setAttribute(XMLString::transcode("IsConstruction"), XMLString::transcode("false"));
	std::ostringstream os;
	os.precision(8);
	os << this->_size; std::string iStr = os.str();
	pointElem->setAttribute(XMLString::transcode("Size"), XMLString::transcode(iStr.c_str()));

	//Add base object
	DOMElement* baseElement = this->WCSerializeableObject::Serialize(document);
	pointElem->appendChild(baseElement);
	
	//Add base vector
	DOMElement* vecElem = document->createElement(XMLString::transcode("Base"));
	this->_data.ToElement(vecElem);
	pointElem->appendChild(vecElem);
	//Return the element
	return pointElem;
}


WCControlPoint* WCControlPoint::Deserialize(DOMElement* obj) {
	return NULL;
}


bool WCControlPoint::Validate(DOMElement* obj) {
	return false;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCControlPoint &point) {
	//Print out basic control point info
	out << "ControlPoint(" << &point << ") {" << point.X() << ", " << point.Y() << ", " << point.Z() << ": " << point.W() << "}\n";
	return out;
}
	
	
/***********************************************~***************************************************/


	