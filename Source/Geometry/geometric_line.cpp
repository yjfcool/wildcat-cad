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
#include "Geometry/geometric_line.h"
#include "Geometry/geometric_point.h"
#include "Geometry/geometric_algorithms.h"
#include "Geometry/ray.h"
#include "Geometry/nurbs_curve.h"


/***********************************************~***************************************************/


WCGeometricLine::WCGeometricLine(const WCVector4 &p0, const WCVector4 &p1) : ::WCGeometricCurve(),
	_p0(p0), _p1(p1), _isInfinite(false) {
	//Establish aligned bounding box
	WCVector4 pts[2] = {this->_p0, this->_p1};
	this->_bounds = new WCAlignedBoundingBox(pts, 2);
	//Mark as dirty
	this->_isVisualDirty = true;
}


WCGeometricLine::WCGeometricLine(const WCGeometricLine &line) : ::WCGeometricCurve(line),
	_p0(line._p0), _p1(line._p1), _isInfinite(line._isInfinite) {
	//Establish aligned bounding box
	WCVector4 pts[2] = {this->_p0, this->_p1};
	this->_bounds = new WCAlignedBoundingBox(pts, 2);
	//Mark as dirty
	this->_isVisualDirty = true;
}


WCGeometricLine::WCGeometricLine(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCGeometricCurve( WCSerializeableObject::ElementFromName(element,"GeometricCurve"), dictionary ),
	_p0(), _p1(), _isInfinite(false) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometricLine::WCGeometricLine - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Get infinite attiribute
	this->_isInfinite = WCSerializeableObject::GetBoolAttrib(element, "infinite");
	//Setup p0 vector
	this->_p0.FromElement(WCSerializeableObject::ElementFromName(element,"P0"));
	//Setup p1 vector
	this->_p1.FromElement(WCSerializeableObject::ElementFromName(element,"P1"));

	//Establish aligned bounding box
	WCVector4 pts[2] = {this->_p0, this->_p1};
	this->_bounds = new WCAlignedBoundingBox(pts, 2);
	//Mark as dirty
	this->_isVisualDirty = true;
}


WCGeometricLine::~WCGeometricLine() {
	//Nothing for now
}


void WCGeometricLine::Begin(const WCVector4 &begin) {
	//Update p0 (if needed)
	if (this->_p0 != begin) {
		this->_p0 = begin;
		//Send message 
		this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
	}
}


void WCGeometricLine::End(const WCVector4 &end) {
	//Update p1 (if needed)
	if (this->_p1 != end) {
		this->_p1 = end;
		//Send message 
		this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
	}
}


void WCGeometricLine::Set(const WCVector4 &begin, const WCVector4 &end) {
	//Make sure something is changing
	if ((this->_p0 != begin) || (this->_p1 != end)) {
		//Update p0
		this->_p0 = begin;
		//Update p1
		this->_p1 = end;
		//Send message 
		this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
	}
}


void WCGeometricLine::Color(const WCColor &color) {
	//Set the color
	this->_color = color;
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


std::list<WPFloat> WCGeometricLine::Intersect(WCGeometricPoint *point, const WPFloat &tol) {
	//Get direction vector
	WCVector4 direction = this->_p1 - this->_p0;
	direction.Normalize(true);
	//Get point vector
	WCVector4 pointVector = point->Data() - this->_p0;
	WPFloat u = direction.DotProduct(pointVector);
	//Bounds check u
	if ((u < -tol) || (u > (1.0 + tol))) return std::list<WPFloat>();
	//Bound u [0.0 to 1.0]
	u = STDMAX( STDMIN(u, 1.0), 0.0);
	//Get the point on the line
	WCVector4 pt = this->_p0 + direction * u;
	//Get distance from point on line to point
	WPFloat dist = pt.Distance(point->Data());
	//Return u if dist <= tol
	if (dist > tol) return std::list<WPFloat>();
	//Otherwise, return the intersection
	return std::list<WPFloat>(1, u);
}


std::list<WPIntersectRec> WCGeometricLine::Intersect(WCGeometricCurve *curve, const WPFloat &tol) {
	WPUInt retVal;
	WPFloat uValue, vValue;
	WCVector4 value;
	WPIntersectRec rec;
	//Evaluate this line at its ends
	WCVector4 p1 = this->Evaluate(0.0);
	WCVector4 p2 = this->Evaluate(1.0);

	WCGeometricLine *line = dynamic_cast<WCGeometricLine*>(curve);
	//Check for line-line intersection
	if (line != NULL) {
		//Evaluate both lines at both ends
		WCVector4 p3 = line->Evaluate(0.0);
		WCVector4 p4 = line->Evaluate(1.0);
		retVal = LineLineIntersection(p1, p2, p3, p4, false, tol, value, uValue, vValue);
		if (retVal != INTERSECTION_INTERSECTION) return std::list<WPIntersectRec>();
		rec = std::make_pair( value, std::make_pair(uValue, vValue) );
		//Return the record
		return std::list<WPIntersectRec>(1, rec);
	}
	else {
		WCNurbsCurve *nurbs = dynamic_cast<WCNurbsCurve*>(curve);
		//Double check that curve is NURBS
		if (nurbs == NULL) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometricLine::Intersect - Curve is of unknown type.");
			return std::list<WPIntersectRec>();
		}	
		//Check to see if line intersects nurbs bounding box
		//...
		
		//Map the curve buffer and get number of vertices
		GLuint buffer = nurbs->VertexBuffer();
		WPUInt numVerts = nurbs->LevelOfDetail(), index=4;
		WPFloat paraFactor = 1.0 / (numVerts - 1);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		GLfloat *data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		if (data == NULL) {
			//Unbind and map buffer
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometricLine::Intersect - Curve has NULL buffer.");
			return std::list<WPIntersectRec>();
		}
				
		std::list<WPIntersectRec> hitList;
		//Initialize the first vector
		WCVector4 firstVec((WPFloat)data[0], (WPFloat)data[1], (WPFloat)data[2], (WPFloat)data[3]), secVec;
		//Loop through remaining vertices and test each
		for (WPUInt i=1; i<numVerts; i++) {
			//Determine secVec
			secVec.Set( (WPFloat)data[index], (WPFloat)data[index+1], (WPFloat)data[index+2], (WPFloat)data[index+3] );
//			std::cout << "First: " << firstVec << std::endl;
//			std::cout << "Secnd: " << secVec << std::endl;
			//See if there is an intersection between line and curve segment
			retVal = LineLineIntersection(p1, p2, firstVec, secVec, false, tol, value, uValue, vValue);
			//Intersection point found
			if (retVal == INTERSECTION_INTERSECTION) {
//				std::cout << "Line-Curve Intersect\n";
//				std::cout << "Hit found at: " << value << std::endl;
//				std::cout << "U-Value: " << uValue << ", V-Value: " << vValue << std::endl;
				//Modify vValue depending on i
				vValue = (i - 1 + vValue) * paraFactor;
				//Create hit record
				rec = std::make_pair( value, std::make_pair(uValue, vValue) );
				//Add to hit list
				hitList.push_back(rec);
			}
			//Move secVec to firstVec
			firstVec = secVec;
			index += 4;
		}
		//Unbind and map buffer
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//Check for GL errors
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometricLine::Intersect - GL error at Cleanup.");
		//Return list of hits
		return hitList;
	}
}

	
WPFloat WCGeometricLine::Length(const WPFloat &tolerance) {
	//Return the distance from p0 to p1
	return this->_p0.Distance(this->_p1);
}


WCVector4 WCGeometricLine::Evaluate(const WPFloat &u) {
	//Make sure u in bounds
	if ((u < 0.0) || (u > 1.0)) CLOGGER_WARN(WCLogManager::RootLogger(), "WCGeometricLine::Evaluate - U out of bounds: " << u);
	WCVector4 i1 = this->_p1 * u;
	WCVector4 i0 = this->_p0 * (1.0 - u);
	return i0 + i1;
}


WCVector4 WCGeometricLine::Derivative(const WPFloat &u, const WPUInt &der) {
	//Make sure u is valid
	if ((u < 0.0) || (u > 1.0)) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCGeometricLine::Derivative - U out of bounds: " << u);
		//throw error
		return WCVector4();
	}
	//No derivatives exist for a line above 1
	if (der > 1) return WCVector4();
	//Determine the tangent
	WCVector4 dir = this->_p1 - this->_p1;
	dir.Normalize(true);
	return dir;
}


WCRay WCGeometricLine::Tangent(const WPFloat &u) {
	//Make sure u is valid
	if ((u < 0.0) || (u > 1.0)) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCGeometricLine::Evaluate - U out of bounds: " << u);
		//throw error
		return WCRay(WCVector4(), WCVector4());
	}
	//Determine the tangent
	WCVector4 dir = this->_p1 - this->_p0;
	dir.Normalize(true);
	//Evaluate at the u value
	WCVector4 pt = (this->_p1 * u) + (this->_p0 * (1.0 - u));
	//Return a ray
	return WCRay(pt, dir);
}


WCVector4 WCGeometricLine::PointInversion(const WCVector4 &point, const WPFloat &u) {
	WPFloat localU = u;
	//Get direction vector
	WCVector4 direction = this->_p1 - this->_p0;
	direction.Normalize(true);
	//Get point vector
	WCVector4 pointVector = point - this->_p0;
	localU = direction.DotProduct(pointVector);
	//Divide u by the magnitude of p0 - pt
	localU /= this->_p1.Distance(this->_p0);
	//Check end conditions
	if (localU < 0.0) return this->_p0;
	if (localU > 1.0) return this->_p1;
	//Get the point on the line
	WCVector4 pt = this->_p0 * (1.0 - localU) + this->_p1 * localU;
	return pt;
}


WCVisualObject* WCGeometricLine::HitTest(const WCRay &ray, const WPFloat &tolerance) {
	return NULL;
}


void WCGeometricLine::ApplyTransform(const WCMatrix4 &transform) {
	//Apply transform to p0
	this->_p0 = transform * this->_p0;
	//Apply transform to p1
	this->_p1 = transform * this->_p1;
	//Mark as dirty
	this->_isVisualDirty = true;
	//Send message 
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCGeometricLine::ApplyTranslation(const WCVector4 &translation) {
	//Apply translation to p0
	this->_p0 += translation;
	//Apply translation to p1
	this->_p1 += translation;
	//Mark as dirty
	this->_isVisualDirty = true;
	//Send message 
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCGeometricLine::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Make sure is visible
	if (!this->_isVisible) return;
	//Mark as clean
	this->_isVisualDirty = false;
	//Set the rendering program
	if (this->_renderProg != 0) glUseProgram(this->_renderProg);
	else glUseProgram(defaultProg);
	//Set color appropriately
	if (color == WCColor::Default()) {
		this->_color.Enable();
	}
	else {
		color.Enable();
		glUseProgram(0);
	}
	//Set the line thickness
	glLineWidth((GLfloat)this->_thickness);
	//Setup data for the line
	GLfloat data[6] = { (GLfloat)this->_p0.I(), (GLfloat)this->_p0.J(), (GLfloat)this->_p0.K(),
						(GLfloat)this->_p1.I(), (GLfloat)this->_p1.J(), (GLfloat)this->_p1.K() };
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_LINE_SMOOTH);
	//Draw the line
	glVertexPointer(3, GL_FLOAT, 0, data);
	glDrawArrays(GL_LINES, 0, 2);
	//Clean up state
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_LINE_SMOOTH);
	//Check for errors
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCGeometricLine::Render Error - Unspecified error.\n";
}


xercesc::DOMElement* WCGeometricLine::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("GeometricLine");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Include the parent element
	xercesc::DOMElement* curveElement = this->WCGeometricCurve::Serialize(document, dictionary);
	element->appendChild(curveElement);

	//Add Infinite attribute
	WCSerializeableObject::AddBoolAttrib(element, "infinite", this->_isInfinite);

	//Add p0 vector
	this->_p0.ToElement(element, "P0");
	//Add p1 vector
	this->_p1.ToElement(element, "P1");

	//Return the element
	return element;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCGeometricLine &line) {
	//Print out basic surface info
	out << "GeometricLine(" << (void*)&line << "):\n";
	out << "\t Begin: " << line._p0 << std::endl;
	out << "\t End:" << line._p1 << std::endl;
	return out;
}


/***********************************************~***************************************************/

