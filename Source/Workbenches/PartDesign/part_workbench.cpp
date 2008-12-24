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
#include "PartDesign/part_workbench.h"
#include "PartDesign/part.h"
#include "Application/keymap.h"
#include "Scene/frame_rate_monitor.h"
#include "Kernel/selection_mode.h"
#include "Sketcher/sketch.h"


//Part Feature Included Headers
#include "PartDesign/part_plane.h"
#include "PartDesign/part_body.h"
#include "PartDesign/part_pad.h"
#include "PartDesign/part_shaft.h"


//Temporarily Included Headers
#include "Converters/converter_stl.h"


/***********************************************~***************************************************/


WCPartWorkbench::WCPartWorkbench(WCPart *part) : ::WCWorkbench(part, "Part Designer"), _part(part) {
	//Make sure part is not null
	ASSERT(part);

	//Setup the key map
	this->_keyMap->AddMapping( WCKeyEvent('b'), WCUserMessage("body") );
	this->_keyMap->AddMapping( WCKeyEvent('e'), WCUserMessage("editSketch") );
	this->_keyMap->AddMapping( WCKeyEvent('h'), WCUserMessage("shaft") );
	this->_keyMap->AddMapping( WCKeyEvent('p'), WCUserMessage("pad") );
	this->_keyMap->AddMapping( WCKeyEvent('s'), WCUserMessage("sketch") );
	this->_keyMap->AddMapping( WCKeyEvent(127), WCUserMessage("delete") );
	this->_keyMap->AddMapping( WCKeyEvent('x'), WCUserMessage("export") );
	
	//Create UI objects if part is root document
	if (this->_part->Document() == this->_part) {
		this->_compass = new WCCompass(part->UserInterfaceLayer());
		this->_compass->SetPlacement(WCPlacement::LowerLeft());
		this->_frameRateMonitor = new WCFrameRateMonitor(part->UserInterfaceLayer());
		this->_frameRateMonitor->SetPlacement(WCPlacement::LowerRight());
//		frm->IsVisible(true);
	}
}


WCPartWorkbench::~WCPartWorkbench() {
	//Delete the frame rate monitor and compass
	if (this->_frameRateMonitor != NULL) delete this->_frameRateMonitor;
	if (this->_compass != NULL) delete this->_compass;
}


void WCPartWorkbench::CreateInitialObjects(void) {
	//Create initial child features
//	this->_part->ExecuteAction( WCFeaturePoint::ActionCreate(this->_part, "", 0.0, 0.0, 0.0) );
//	this->_part->ExecuteAction( WCCoordinateSystem::ActionCreate(this->_part, "",
//												dynamic_cast<WCFeaturePoint*>(this->_part->FeatureFromName("Point.1")), 
//												WCVector4(1.0, 0.0, 0.0), WCVector4(0.0, 1.0, 0.0), WCVector4(0.0, 0.0, 1.0)) );
//	this->_part->ExecuteAction( WCFeaturePoint::ActionCreate(this->_part, "", 0.0, 1.0, 0.0) );
//	this->_part->ExecuteAction( WCFeaturePoint::ActionCreate(this->_part, "", 1.0, 0.0, 0.0) );
	this->_part->ExecuteAction( WCPartPlane::ActionCreate(this->_part, "xy plane", WCVector4(0.0, 0.0, 0.0), 
												WCVector4(1.0, 0.0, 0.0), WCVector4(0.0, 1.0, 0.0)) );
	this->_part->ExecuteAction( WCPartPlane::ActionCreate(this->_part, "yz plane", WCVector4(0.0, 0.0, 0.0), 
												WCVector4(0.0, 1.0, 0.0), WCVector4(0.0, 0.0, 1.0)) );
	this->_part->ExecuteAction( WCPartPlane::ActionCreate(this->_part, "zx plane", WCVector4(0.0, 0.0, 0.0), 
												WCVector4(0.0, 0.0, 1.0), WCVector4(1.0, 0.0, 0.0)) );
	this->_part->ExecuteAction( WCPartBody::ActionCreate(this->_part, "PartBody") );
}



bool WCPartWorkbench::OnEnter(void) {
	//Call to the base OnEnter
	this->WCWorkbench::OnEnter();
	//Show part only design toolbars
	this->_part->Document()->ToolbarManager()->ToolbarFromName("Standard")->IsVisible(true);
	this->_part->Document()->ToolbarManager()->ToolbarFromName("View")->IsVisible(true);
	this->_part->Document()->ToolbarManager()->ToolbarFromName("Part Design")->IsVisible(true);
	//Return true
	return true;
}


bool WCPartWorkbench::OnExit(void) {
	//Hide part design toolbars
	this->_part->Document()->ToolbarManager()->ToolbarFromName("Standard")->IsVisible(false);
	this->_part->Document()->ToolbarManager()->ToolbarFromName("View")->IsVisible(false);
	this->_part->Document()->ToolbarManager()->ToolbarFromName("Part Design")->IsVisible(false);
	//Call to the base OnExit
	this->WCWorkbench::OnExit();
	//Return true
	return true;
}


bool WCPartWorkbench::OnUserMessage(const WCUserMessage &message) {
	//See if super wants to handle
	if (this->WCWorkbench::OnUserMessage(message)) return true;

	//Get controller for top selected object
	WCSelectionObject* selected = this->SelectionManager()->Top();
	WCEventController* controller = dynamic_cast<WCEventController*>(selected);
	WCDrawingMode *mode;

	//Is this a select mode message
	if (message == "select") {
		//Revert to default drawing mode
//		this->DrawingMode( WCDrawingMode::Selection( this->_part->ActiveWorkbench() ));
		this->DrawingMode( new WCSelectionMode( this->_part->ActiveWorkbench() ));
	}
	//Export the current part
	else if (message == "export") {
		//Export the part
		WCConverterSTL converter;
		converter.Export(this->_part);
	}
	//Delete the selected elements
	else if (message == "delete") {
		//Delete selected sketch features
		WCAction *action = WCPart::ActionDelete(this->_part, this->SelectionManager()->FilterSelected<WCPartFeature>(true) );
		this->_part->Document()->ExecuteAction( action );	
	}
	//Is this a pad mode message
	else if (message == "body") {
		//Create a new body
		this->_part->ExecuteAction( WCPartBody::ActionCreate(this->_part, "") );
	}
	//Is this a pad mode message
	else if (message == "pad") {
		//Create a new drawing mode
		mode = WCPartPad::ModeCreate(this);
		//Go into pad mode
		this->DrawingMode(mode);
	}
	//Is this a shaft mode message
	else if (message == "shaft") {
		//Create a new drawing mode
		mode = WCPartShaft::ModeCreate(this);
		//Go into shaft mode
		this->DrawingMode(mode);
	}
	//Is this a sketch editing mode message
	else if (message == "editSketch") {
		//See if there is a selected sketch
		std::list<WCSketch*> sketchList = this->SelectionManager()->FilterSelected<WCSketch>(true);
		//Make sure there is just one sketch selected
		if (sketchList.size() != 1) {
			_part->Status("Must have one sketch selected to enter sketcher workbench.");
			return true;
		}
		//Enter sketcher workbench
		_part->EnterWorkbench(sketchList.front()->Workbench());	
	}
	//Is this a sketcher mode message
	else if (message == "sketch") {
		WCPartPlane *refPlane;
		//Make sure something is selected
		if ((controller == NULL) || 
			(this->SelectionManager()->Count() != 1) ||
			((refPlane = dynamic_cast<WCPartPlane*>(controller->Associate())) == NULL)) {
			_part->Status("Must have one plane selected to create a sketch.");
			return true;
		}

		//Create a sketch
		_part->ExecuteAction( WCSketch::ActionCreate(_part->Body(), "", refPlane) );
	}
	//Message not captured
	else {
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCPartWorkbench::OnUserMessage - Unknown user message received: " << message);
		return false;
	}
	//Message was captured
	return true;
}


/***********************************************~***************************************************/

