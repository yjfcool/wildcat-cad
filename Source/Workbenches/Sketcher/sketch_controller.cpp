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
#include <Sketcher/sketch_controller.h>
#include <Sketcher/sketch.h>
#include <Sketcher/sketch_feature.h>
#include <Kernel/document.h>


/***********************************************~***************************************************/


WCSketchController::WCSketchController(WCSketch *sketch) : ::WCEventController(sketch->TreeElement()),
	_sketch(sketch) { 
	//Make sure sketch is not null
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchController::WCSketchController - NULL sketch passed.");
		//throw error
		return;
	}
//	std::cout << "SketchController: " << this << std::endl;
}


inline WCObject* WCSketchController::Associate(void) {
	return this->_sketch;
}


void WCSketchController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	this->_sketch->Document()->Status("Sketch " + this->_sketch->GetName() + " was selected");
//	std::cout << "Sketch " << this->_sketch->GetName() << " was selected.\n";
	//See if in sketcher workbench
	if (this->_sketch->Workbench()->IsActive()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchController::OnSelection - not sure what to do here.");
	}
	else {
		//Is this from the selection manager
		if (!fromManager) {
			//Clear selection buffer if appropriate
			if (!this->_sketch->Document()->ActiveWorkbench()->IsMultiSelect())
				this->_sketch->Document()->ActiveWorkbench()->SelectionManager()->Clear(true);
			//Add this plane to the selection manager
			this->_sketch->Document()->ActiveWorkbench()->SelectionManager()->ForceSelection(this, false);
		}
		//Tell the plane it has been selected
		if (!this->_sketch->IsSelected()) this->_sketch->OnSelection(false, objects);
		//Mark the tree element as selected
		this->_sketch->TreeElement()->IsSelected(true);
	}
}


void WCSketchController::OnDeselection(const bool fromManager) {
//	std::cout << "Sketch: " << this->_sketch->GetName() << " was deselected.\n";
	//See if in sketcher workbench
	if (this->_sketch->Workbench()->IsActive()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchController::OnSelection - not sure what to do here.");
	}
	else {
		if (!fromManager) {
			//Remove the item from the selection list
			this->_sketch->Document()->ActiveWorkbench()->SelectionManager()->ForceDeselection(this, false);
		}
		//Tell the plane it has been deselected
		if (this->_sketch->IsSelected()) this->_sketch->OnDeselection(false);
		//Mark the tree elements as not selected
		this->_sketch->TreeElement()->IsSelected(false);
		this->_sketch->ReferencesTreeElement()->IsSelected(false);
		this->_sketch->FeaturesTreeElement()->IsSelected(false);
		this->_sketch->ConstraintsTreeElement()->IsSelected(false);
		//Clear all feature tree elements
		std::map<std::string, WCSketchFeature*>	featureMap = this->_sketch->FeatureMap();
		std::map<std::string, WCSketchFeature*>::iterator iter;
		for (iter = featureMap.begin(); iter != featureMap.end(); iter++) {
			(*iter).second->TreeElement()->IsSelected(false);
		}
	}
}


void WCSketchController::OnContextClick(void) {
}


/***********************************************~***************************************************/

