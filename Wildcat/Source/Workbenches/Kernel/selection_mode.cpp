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
#include "Kernel/selection_mode.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCSelectionMode::WCSelectionMode(WCWorkbench *wb) : ::WCDrawingMode(wb->Feature(), SELECTIONMODE_NAME), 
	_workbench(wb), _isSelecting(false), _markX(0.0), _markY(0.0) { 
	//Nothing else to do for now
}


void WCSelectionMode::OnEntry(void) {
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering Selection Mode:" << this);
	//Clear the document status text
	this->_creator->Document()->Status("Ready");
	//Set the icon to active
	this->_creator->Document()->ToolbarManager()->ButtonFromName("select")->IsActive(true);
}


void WCSelectionMode::OnExit(void) {
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting Selection Draw Mode:" << this);
	//Set the icon to not active
	this->_creator->Document()->ToolbarManager()->ButtonFromName("select")->IsActive(false);
}


void WCSelectionMode::OnMouseMove(const WPFloat &x, const WPFloat &y) {
}


void WCSelectionMode::OnMouseUp(const WCMouseButton &button) {
	//We are no longer selecting
	this->_isSelecting = false;
	WCScene *scene = this->_workbench->Feature()->Document()->Scene();
	if ((scene->MouseX() == this->_markX) && (scene->MouseY() == this->_markY)) return;
	//Get the bounds of the selection
	WPFloat xMin = std::min(scene->MouseX(), this->_markX);
	WPFloat xMax = std::max(scene->MouseX(), this->_markX);
	WPFloat yMin = std::min(scene->MouseY(), this->_markY);
	WPFloat yMax = std::max(scene->MouseY(), this->_markY);
	//Convert scene position to screen position
	WPUInt xMinPos = (xMin + (scene->Width()/2.0)) * (scene->WindowWidth() / scene->Width());
	WPUInt xMaxPos = (xMax + (scene->Width()/2.0)) * (scene->WindowWidth() / scene->Width());
	WPUInt yMinPos = scene->WindowHeight() - (yMin - scene->Height() / 2.0) * (-1.0 * scene->WindowHeight() / scene->Height());
	WPUInt yMaxPos = scene->WindowHeight() - (yMax - scene->Height() / 2.0) * (-1.0 * scene->WindowHeight() / scene->Height());
	//Clear if appropriate
	if (!this->_workbench->IsMultiSelect()) this->_workbench->SelectionManager()->Clear(true);
	//Call selection
	this->_workbench->SelectionManager()->Select(xMinPos, xMaxPos, yMinPos, yMaxPos, true);
	//If multiple objects selected adjust document status
	WPUInt count = this->_workbench->SelectionManager()->Count();
	if (count > 1) {
		//Convert count to string value
		std::ostringstream os;
		os << count;
		std::string countStr = os.str();
		//Set document status text
		this->_creator->Document()->Status(countStr + " objects selected");
	}
}


void WCSelectionMode::OnMouseDown(const WCMouseButton &button) {
	//Clear the document status text
	this->_creator->Document()->Status("Ready");
	//We are now selecting
	this->_isSelecting = true;
	WCScene *scene = this->_workbench->Feature()->Document()->Scene();
	//Set the mark
	this->_markX = scene->MouseX();
	this->_markY = scene->MouseY();
	//Clear if appropriate
	if (!this->_workbench->IsMultiSelect()) this->_workbench->SelectionManager()->Clear(true);
	//Get pixel position
	WPUInt x = (this->_markX + (scene->Width()/2.0)) * (scene->WindowWidth() / scene->Width());
	WPUInt y = scene->WindowHeight() - (this->_markY - scene->Height() / 2.0) * (-1.0 * scene->WindowHeight() / scene->Height());
	//Call single select
	this->_workbench->SelectionManager()->Select(x, x, y, y, true);
}


void WCSelectionMode::Render(void) {
	//Check to make sure we are selecting
	if (!this->_isSelecting) return;
	//Get the bounds of the selection
	WPFloat xMin = std::min(this->_workbench->Feature()->Document()->Scene()->MouseX(), this->_markX);
	WPFloat xMax = std::max(this->_workbench->Feature()->Document()->Scene()->MouseX(), this->_markX);
	WPFloat yMin = std::min(this->_workbench->Feature()->Document()->Scene()->MouseY(), this->_markY);
	WPFloat yMax = std::max(this->_workbench->Feature()->Document()->Scene()->MouseY(), this->_markY);
	
	//Set up billboarding
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	//Draw the selection rectangle
	WCOverlay::RenderRectangle(xMin, yMin, xMax, yMax, 
		WCColor(SELECTIONMODE_COLOR),  WCColor(SELECTIONMODE_COLOR_BORDER), WCColor(0.0, 0.0, 0.0, 0.0));
	//Cleanup and exit
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
}


/***********************************************~***************************************************/

