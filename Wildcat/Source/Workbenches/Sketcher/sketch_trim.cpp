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
#include "Sketcher/sketch_trim.h"
#include "Sketcher/sketch.h"


/***********************************************~***************************************************/




/***********************************************~***************************************************/


WCDrawingMode* WCSketchTrim::ModeCreate(WCSketchWorkbench *wb) {
	//Create a new drawing mode
	return new WCModeSketchTrimCreate(wb);
}


/*
WCDrawingMode* WCSketchLine::ModeEdit(WCSketchLine *line) {
	//Create a new editing mode
	return new WCModeSketchLineEdit(line);
}


WCActionSketchLineCreate* WCSketchLine::ActionCreate(WCSketch *sketch, const std::string &lineName, 
	const WCVector4 &begin, const WCVector4 &end) {
	//Create new action to create line
	return new WCActionSketchLineCreate(sketch, lineName, begin, end);
}


WCAction* WCSketchLine::ActionMove(WCSketchLine *line, const WCVector4 &begin, const WCVector4 &end) {
	//Create new action to move the line
	return new WCActionSketchLineMove(line, begin, end);
}


/***********************************************~***************************************************/

	
std::ostream& operator<<(std::ostream& out, const WCSketchTrim &trim) {
	return out;
}


/***********************************************~***************************************************/
