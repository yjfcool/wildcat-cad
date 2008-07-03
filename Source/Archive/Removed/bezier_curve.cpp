/*
 *  bezier_curve.cpp
 *  nurbs
 *
 *  Created by ghemingway on 11/16/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "bezier_curve.h"


/***********************************************~***************************************************/


void WCBezierCurve::GenerateCurveLow(void) {
}


/***********************************************~***************************************************/


WCBezierCurve::WCBezierCurve(const WPUInt degree, const std::vector<WCVector4> &controlPoints) {
}


WCBezierCurve::WCBezierCurve(const WCBezierCurve &curve) {
}


WCBezierCurve::~WCBezierCurve() {
}


WPUInt WCBezierCurve::Degree(const WPUInt degree) {
	return this->_degree;
}


WPUInt WCBezierCurve::LevelOfDetail(const WPUInt lod) {
	return this->_lod;
}


bool WCBezierCurve::Intersect(const WCGeometricPoint &point, const WPFloat tolerance) {
	return false;
}


bool WCBezierCurve::Intersect(const WCGeometricCurve &curve, const WPFloat tolerance) {
	return false;
}


WPFloat WCBezierCurve::Length(WPFloat tolerance) {
	return 1.0;
}


WCVector4 WCBezierCurve::Evaluate(const WPFloat u) {
	return WCVector4();
}


WCVisualObject* WCBezierCurve::HitTest(const WCRay &ray, const WPFloat tolerance) {
	return NULL;
}


void WCBezierCurve::ApplyTransform(const WCMatrix4 &transform) {
}


void WCBezierCurve::ApplyTranslation(const WCVector4 &translation) {
}


void WCBezierCurve::Render(const GLuint defaultProg, const WCColor color) {
}


void WCBezierCurve::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
}


WCVector4 WCBezierCurve::Derivative(const WPFloat u, const WPUInt der) {
	return WCVector4();
}


WCRay WCBezierCurve::Tangent(const WPFloat u) {
	return WCRay(WCVector4(), WCVector4());
}


WCVector4 WCBezierCurve::PointInversion(const WCGeometricPoint &point) {
	return WCVector4();
}

	
WCBezierCurve& WCBezierCurve::operator=(const WCBezierCurve &curve) {
	return *this;
}


bool WCBezierCurve::operator==(const WCBezierCurve &curve) {
	return false;
}

	
DOMElement* WCBezierCurve::Serialize(DOMDocument *document) {
	return NULL;
}


WCBezierCurve* WCBezierCurve::Deserialize(DOMElement* obj) {
	return NULL;
}


bool WCBezierCurve::Validate(DOMElement* obj) {
	return false;
}
	

/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCBezierCurve &curve) {
	return out;
}


/***********************************************~***************************************************/

