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
#include "Geometry/ray.h"


/***********************************************~***************************************************/


WCRay::WCRay(const WCRay &ray) : ::WCVisualObject(), _base(ray._base), _direction(ray._direction) {
	//Nothing else to do for now
}


WCRay::WCRay(const WCVector4 &base, const WCVector4 &direction) : ::WCVisualObject(), _base(base), _direction(direction) {
	//Make sure to normailze direction vector
	this->_direction.Normalize();
}


WCRay::WCRay(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : ::WCVisualObject(), _base(), _direction() {
	//Restore from element
	//...
}


WCRay::~WCRay() {
	//Nothing to do	
}	
	

void WCRay::Base(const WCVector4 &base) {
	//Copy in the base
	this->_base = base;
	//Mark as dirty
	this->IsSerialDirty(true);
}


void WCRay::Direction(const WCVector4 &direction) {
	//Set the new direction - make sure to normalize (just in case)
	this->_direction = direction;
	this->_direction.Normalize();
	//Mark as dirty
	this->IsSerialDirty(true);
}


void WCRay::ApplyTransform(const WCMatrix4 &transform) {
	//Adjust the base
	this->_base = transform * this->_base;
	//Adjust the direction and renormalize
	this->_direction = transform * this->_direction;
	this->_direction.Normalize();
	//Mark as dirty
	this->IsSerialDirty(true);
}


void WCRay::ApplyTranslation(const WCVector4 &tvector) {
	//Mark as dirty
	this->IsSerialDirty(true);
}


void WCRay::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Set color appropriately
	if (color == WCColor::Default()) this->_color.Enable();
	else color.Enable();
	//Draw the ray
	glBegin(GL_LINES);
		glVertex3f((GLfloat)this->_base.I(), (GLfloat)this->_base.J(), (GLfloat)this->_base.K());
		glVertex3f((GLfloat)(this->_base.I()+this->_direction.I()),
				   (GLfloat)(this->_base.J()+this->_direction.J()),
				   (GLfloat)(this->_base.K()+this->_direction.K()));
	glEnd();
}
	
	
WCRay WCRay::FromScreenCoords(const int &xMouse, const int &yMouse, const int &width, const int &height, 
	const GLfloat &zFront, const GLfloat &zBack) {
/*	//Get the modelview matrix inversion
	GLfloat data[16];
	glGetFloatv(GL_MODELVIEW_MATRIX , mvData);
	WCMatrix4 tmpMat = WCMatrix4::FromGLMatrix(data);
	WCMatrix4 invMV = tmpMat.Inverse();
//	invMV.Print();
	//Get the projection matrix inversion
	glGetFloatv(GL_PROJECTION_MATRIX , data);
	tmpMat = WCMatrix4::FromGLMatrix(data);
	WCMatrix4 invProj = tmpMat.Inverse();
//	invProj.Print();	
*/
	//Get matrices
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLdouble modelview[16];	
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	//Get screen coordinates
	GLfloat winX, winY, winZ;				
	winX = (float)xMouse;					
	winY = (float)yMouse;					
	winY = (float)viewport[3] - winY;
	//Setup output variables
	GLdouble posX, posY, posZ;
	glReadPixels( GLint(winX), GLint(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
	//Convert front point, convert back point
/*** DEBUG ***/
//Need to implement on our own
	printf("Need to implement WCRay::FromScreenCoords\n");
//	gluUnProject( winX, winY, 0.01, modelview, projection, viewport, &posX, &posY, &posZ);
	WCVector4 front(posX, posY, posZ, 1.0);
//	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
/*** DEBUG ***/
	WCVector4 back(posX, posY, posZ, 1.0);
	WCVector4 direction = back - front;
	direction.Normalize();
	
	//Create a vector
	return WCRay(front, direction);	
}
	
	

WCRay& WCRay::operator=(const WCRay &ray) {
	//Set the base and direction
	this->_base = ray._base;
	this->_direction = ray._direction;
	//Return this object
	return *this;
}


bool WCRay::operator==(const WCRay &ray) {
	return (this->_base == ray._base) && (this->_direction == ray._direction);
}


xercesc::DOMElement* WCRay::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Create the base element for the object
	xercesc::DOMElement*  rayElem = document->createElement(xercesc::XMLString::transcode("Ray"));
	return rayElem;
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCRay &ray) {
	void* addr = (void*)&ray;
	
	//Print out basic surface info
	out << "Ray(" << addr << ")\n";
	out << "\t Base: " << ray._base;
	out << "\t Direction: " << ray._direction;
	return out;
}


/***********************************************~***************************************************/


