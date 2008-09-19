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
#include "RTVisualization/vis_workbench.h"
#include "RTVisualization/visualization.h"
#include "RTVisualization/ground_plane.h"
#include "Application/keymap.h"
#include "Kernel/document.h"
#include "Kernel/workbench_layer.h"
#include "Kernel/selection_mode.h"
#include "Scene/frame_rate_monitor.h"


/*** Included Feature Headers ***/
#include "RTVisualization/vis_motiontracker.h"
#include "RTVisualization/vis_listener.h"
#include "RTVisualization/vis_recorder.h"


#define SERV_PORT								9877
#define RECORDER_MAX							500


/***********************************************~***************************************************/


WCVisWorkbench::WCVisWorkbench(WCVisualization *vis) : ::WCWorkbench(vis, "Real-Time Visualizer"), _visualization(vis) {
	//Make sure vis is not null
	if (this->_visualization == NULL) {
		CLOGGER_FATAL(WCLogManager::RootLogger(), "WCVisWorkbench::WCPartWorkbench - NULL visualization passed.");
		//throw error
		return;
	}

	//Setup the key map
	this->_keyMap->AddMapping( WCKeyEvent('r'), WCUserMessage("reset") );
	this->_keyMap->AddMapping( WCKeyEvent('s'), WCUserMessage("start") );
	this->_keyMap->AddMapping( WCKeyEvent('x'), WCUserMessage("stop") );
	this->_keyMap->AddMapping( WCKeyEvent('p'), WCUserMessage("pause") );
	this->_keyMap->AddMapping( WCKeyEvent('l'), WCUserMessage("listenerCreate") );
	this->_keyMap->AddMapping( WCKeyEvent('`'), WCUserMessage("select") );
	
	//Create UI objects if part is root document
	if (this->_visualization->Document() == this->_visualization) {
		this->_compass = new WCCompass(vis->UserInterfaceLayer());
		this->_compass->SetPlacement(WCPlacement::LowerLeft());
		this->_frameRateMonitor = new WCFrameRateMonitor(vis->UserInterfaceLayer());
		this->_frameRateMonitor->SetPlacement(WCPlacement::LowerRight());
//		frm->IsVisible(true);
	}

	//Set the render program for the layer
	this->_layer->RenderProgram( this->_visualization->Scene()->ProgramFromName("scn_basiclight") );

	//Create the ground plane
	this->_ground = new WCGroundPlane(*this->_visualization, 0.1, 0.1);
	this->_layer->AddObject(this->_ground);
	this->_layer->AddObject(this->_visualization);
}


WCVisWorkbench::~WCVisWorkbench() {
	//Delete the frame rate monitor and compass
	if (this->_frameRateMonitor != NULL) delete this->_frameRateMonitor;
	if (this->_compass != NULL) delete this->_compass;
}


void WCVisWorkbench::CreateInitialObjects(void) {
	//Create a listener
	WCVisListener *listener1 = new WCVisListener(this->_visualization, "", WCVisListenerType::UDP(), SERV_PORT);

	//Create a data recorder
//	WCVisRecorder *recorder = new WCVisRecorder(this->_visualization, "", RECORDER_MAX);
//	listener1->Recorder(recorder);

	//Create trackers
//	WCVisMotionTracker *tracker1 = new WCVisMotionTracker(this->_visualization, "", 1, NULL, 0.25);
	WCVisMotionTracker *tracker2 = new WCVisMotionTracker(this->_visualization, "", 2, NULL, 0.5);

	if ((listener1 == NULL) || (tracker2 == NULL)) {
		CLOGGER_FATAL(WCLogManager::RootLogger(), "WCVisWorkbench::CreateInitialObjects - Not able to create tracker.");
	}
}


bool WCVisWorkbench::OnEnter(void) {
	//Call to the base OnEnter
	this->WCWorkbench::OnEnter();
	//Show part only design toolbars
//	this->_part->Document()->ToolbarManager()->ToolbarFromName("Standard")->IsVisible(true);
//	this->_part->Document()->ToolbarManager()->ToolbarFromName("View")->IsVisible(true);
//	this->_part->Document()->ToolbarManager()->ToolbarFromName("Part Design")->IsVisible(true);
	//Return true
	return true;
}


bool WCVisWorkbench::OnExit(void) {
	//Hide part design toolbars
//	this->_part->Document()->ToolbarManager()->ToolbarFromName("Standard")->IsVisible(false);
//	this->_part->Document()->ToolbarManager()->ToolbarFromName("View")->IsVisible(false);
//	this->_part->Document()->ToolbarManager()->ToolbarFromName("Part Design")->IsVisible(false);
	//Call to the base OnExit
	this->WCWorkbench::OnExit();
	//Return true
	return true;
}


bool WCVisWorkbench::OnUserMessage(const WCUserMessage &message) {
	//See if super wants to handle
	if (this->WCWorkbench::OnUserMessage(message)) return true;

	WCDrawingMode *mode;
//	WCAction *action;

/*** -- Visualization Control -- ***/

	//Is this a select mode message
	if (message == "start") {
		//Start the visualization
		this->_visualization->StartVisualization();
	}
	//Delete the selected elements
	else if (message == "stop") {
		//Stop
		this->_visualization->StopVisualization();
	}
	//Is this a pad mode message
	else if (message == "reset") {
		//Reset
		this->_visualization->ResetVisualization();
	}
	//Is this a pad mode message
	else if (message == "pause") {
		//Pause
		this->_visualization->PauseVisualization();
	}

/*** -- Features -- ***/

	//Set the drawing mode to rectangle
	else if (message == "listenerCreate") {
		//Create a new drawing mode
		mode = WCVisListener::ModeCreate(this);
		//Go into listener create mode
		this->DrawingMode(mode);
	}

/*** -- Other -- ***/

	//Selection mode
	else if (message == "select") {
		//Revert to default drawing mode
//		this->DrawingMode( WCDrawingMode::Selection(this) );
		this->DrawingMode( new WCSelectionMode(this) );
	}
	//Message not captured
	else {
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCVisWorkbench::OnUserMessage - Unknown user message received: " << message);
		return false;
	}
	//Message was captured
	return true;
}


/***********************************************~***************************************************/

