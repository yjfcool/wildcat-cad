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
#include <Constraint/constraint_length_modes.h>
#include <Constraint/constraint_length.h>
#include <Constraint/constraint_measure_twopoint.h>
#include <Sketcher/sketch.h>
#include <Kernel/selection_mode.h>
#include <Kernel/document.h>
#include <PartDesign/part_plane.h>


/***********************************************~***************************************************/


WCModeConstraintLengthEdit::WCModeConstraintLengthEdit(WCConstraintLength *constraint) : 
	::WCDrawingMode(constraint->Sketch(), CONSTRAINTLENGTHMODE_EDIT_NAME), _constraint(constraint), _position() {
	//Nothing else for now
}

	
void WCModeConstraintLengthEdit::OnEntry(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCModeConstraintLengthEdit::OnEntry - Entering ConstraintLength Edit Mode.");
	//Get current mouse position
	this->_position.Set(this->_constraint->Sketch()->Workbench()->MouseX(), this->_constraint->Sketch()->Workbench()->MouseY(), 0.0, 1.0);
	//Record the current offsets
	this->_offset = this->_constraint->Measure()->Offset();
	this->_labelOffset = this->_constraint->Measure()->LabelOffset();
}


void WCModeConstraintLengthEdit::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Get current mouse position	
	WPFloat mouseX = this->_constraint->Sketch()->Workbench()->MouseX();
	WPFloat mouseY = this->_constraint->Sketch()->Workbench()->MouseY();
	//Get current position
	WPFloat xSlew = mouseX - this->_position.I();
	WPFloat ySlew = mouseY - this->_position.J();
	//Move measure back to initial position
	this->_constraint->Measure()->Offsets(this->_offset, this->_labelOffset);
	//Slew the measure offsets
	this->_constraint->Measure()->SlewOffsets(xSlew, ySlew);
}


void WCModeConstraintLengthEdit::OnMouseUp(const WCMouseButton &button) {
	//Get the moved offset and labelOffset
	WPFloat offset = this->_constraint->Measure()->Offset();
	WPFloat labelOffset = this->_constraint->Measure()->LabelOffset();
	//Move measure back to initial position
	this->_constraint->Measure()->Offsets(this->_offset, this->_labelOffset);

	//Make sure measure has moved
	if ((offset != this->_offset) || (labelOffset != this->_labelOffset)) {
		//Move measure to current location with an action
		WCAction *action = WCConstraintLength::ActionMove(this->_constraint, offset, labelOffset);
		this->_constraint->Sketch()->Document()->ExecuteAction( action );
	}

	//Revert back to old mode
	this->_constraint->Sketch()->Workbench()->DrawingMode( new WCSelectionMode(this->_constraint->Sketch()->Workbench()) );
}


/***********************************************~***************************************************/

