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
#include <Sketcher/sketch_rectangle.h>
#include <Sketcher/sketch.h>
#include <Sketcher/sketch_line.h>


/***********************************************~***************************************************/


WCDrawingMode* WCSketchRectangle::ModeCreate(WCSketchWorkbench *wb) {
	//Make sure sketch is not null
	if (wb == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchRectangle::ModeCreate - NULL Workbench passed.");
		//throw error
		return NULL;
	}
	//Create a new drawing mode
	return new WCModeSketchRectangleCreate(wb);
}


WCActionSketchRectangleCreate* WCSketchRectangle::ActionCreate(WCSketch *sketch, const WCVector4 &lowerLeft, const WCVector4 &upperRight) {
	//Create new action to a new pre-defined sketch rectangle
	return new WCActionSketchRectangleCreate(sketch, lowerLeft, upperRight);
}


/***********************************************~***************************************************/


WCActionSketchRectangleCreate::WCActionSketchRectangleCreate(WCSketch *sketch, const WCVector4 &lowerLeft, 
	const WCVector4 &upperRight) : WCAction("Create Sketch Rectangle", sketch), _sketch(sketch),
	_lowerLeft(lowerLeft), _upperRight(upperRight) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchRectangleCreate::WCActionSketchRectangleCreate Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchRectangleCreate::WCActionSketchRectangleCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary),
	_sketch(NULL), _lowerLeft(), _upperRight() {
	//Do something here
}


WCFeature* WCActionSketchRectangleCreate::Execute(void) {
	//Create the four lines - left first
	WCVector4 ll = this->_lowerLeft;
	WCVector4 ul(this->_lowerLeft.I(), this->_upperRight.J(), 0.0, 1.0);
	WCVector4 ur = this->_upperRight;
	WCVector4 lr(this->_upperRight.I(), this->_lowerLeft.J(), 0.0, 1.0);
	
	this->_lines[0] = new WCSketchLine(this->_sketch, "", ll, ul);
	//Top line
	this->_lines[1] = new WCSketchLine(this->_sketch, "", ul, ur);
	//Right line
	this->_lines[2] = new WCSketchLine(this->_sketch, "", ur, lr);
	//Bottom line
	this->_lines[3] = new WCSketchLine(this->_sketch, "", lr, ll);

	//Make sure lines are all not null
	if ( (this->_lines[0] == NULL) || (this->_lines[1] == NULL) || (this->_lines[2] == NULL) || (this->_lines[3] == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchRectangleAction::Execute Error - Sketch Rectangle could not be created.");
		//throw error
		return NULL;
	}
	
	//Return the sketch
	return this->_sketch;
}


bool WCActionSketchRectangleCreate::Merge(WCAction *action) { 
	return false; 
}


bool WCActionSketchRectangleCreate::Rollback(void) {
	return false;
}


xercesc::DOMElement* WCActionSketchRectangleCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Do something here
	return NULL;
}


/***********************************************~***************************************************/

