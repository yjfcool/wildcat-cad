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
#include "RTVisualization/vis_listener_modes.h"
#include "RTVisualization/vis_workbench.h"
#include "RTVisualization/visualization.h"
#include "RTVisualization/vis_udp_listener.h"

//#include "Sketcher/sketch_point.h"
//#include "Sketcher/sketch_align_suggestion.h"
//#include "Constraint/constraint_distance.h"
//#include "Kernel/document.h"


/***********************************************~***************************************************/


class WCDialogNewListener : public WCDialogController {
private:
	WCVisualization								*_visualization;
public:
	WCDialogNewListener(WCVisualization *vis) : _visualization(vis) { }
	virtual void ReceiveMessage(const std::string &message) {										//!< Receive message from a dialog
		//See what type of message
		if (message == "CreateListener") {
			std::string name = this->_dialog->StringFromScript("listenerName");
			std::string type = this->_dialog->StringFromScript("listenerType");
			unsigned int port = this->_dialog->UnsignedIntFromScript("listenerPort");

			//Execute action to create listener
			if (type == "UDP") {
				//Need to execute action to create UDP Listener
				//Create the horizontal distance constraint
				WCAction *action = WCVisUDPListener::ActionCreate(this->_visualization, name, port);
				this->_visualization->Document()->ExecuteAction( action);
			}
			else {
				//Need to execute action to create TCP listener
				CLOGGER_WARN(WCLogManager::RootLogger(), "WCDialogNewListener::ReceiveMessage - TCP Listeners are not yet implemented.");
			}

			//Close the dialog
			WCDialogManager::CloseDialog(this->_dialog);
			//Exit the workbench
			this->_visualization->Workbench()->DrawingMode( new WCSelectionMode(this->_visualization->Workbench()) );
			return;
		}
		//Check for closing dialog
		else if (message == "CloseDialog") {
			//Close the dialog
			WCDialogManager::CloseDialog(this->_dialog);
			//Exit the workbench
			this->_visualization->Workbench()->DrawingMode( new WCSelectionMode(this->_visualization->Workbench()) );
			return;
		}
		else {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialogNewListener::ReceiveMessage - Unknown message: " << message);
		}
	}
};


/***********************************************~***************************************************/


WCModeVisListenerCreate::WCModeVisListenerCreate(WCVisWorkbench *wb) : ::WCDrawingMode(wb->Visualization(), VISLISTENERMODE_CREATE_NAME), 
	_workbench(wb), _dialog(NULL) { 
	//Nothing for now
	//...
}


void WCModeVisListenerCreate::OnEntry(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering VisListener Create Mode.");
	//Create the dialog controller
	this->_controller = new WCDialogNewListener(this->_workbench->Visualization());
	//Open the dialog
	this->_dialog = WCDialogManager::DisplayDialog("newListener", this->_controller);
}


void WCModeVisListenerCreate::OnExit(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting VisListener Create Mode.");
	//Delete the controller
	if (this->_controller != NULL) {
		delete this->_controller;
		this->_controller = NULL;
	}
}


/***********************************************~***************************************************/

