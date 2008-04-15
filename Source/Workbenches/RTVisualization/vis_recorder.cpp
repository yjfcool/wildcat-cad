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
#include "RTVisualization/vis_recorder.h"
#include "RTVisualization/vis_recorder_controller.h"
#include "RTVisualization/visualization.h"


/***********************************************~***************************************************/


void WCVisRecorder::ClearBuffer(void) {
	//Need to lock the recorder
	this->Lock();

	//Clear all records
	while (!this->_recordList.empty()) {
		//Free the data
		free(this->_recordList.front().data);
		//Remove it from the list
		this->_recordList.pop_front();
	}

	//Unlock the recorder
	this->Unlock();
}


void WCVisRecorder::Initialize(void) {
	//Check feature name
	if (this->_name == "") this->_name = this->_visualization->GenerateFeatureName(this);
	//Add into visualization
	if (!this->_visualization->AddFeature(this, false)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisRecorder::Initialize - Problem adding recorder to visualization.");
		//Should delete base
	}
	//Create the recorder controller
	this->_controller = new WCVisRecorderController(this);

	//Create tree element and add into the tree (beneath the recorders element)
	WSTexture *icon = this->_document->Scene()->TextureManager()->TextureFromName("record32");
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, icon);
	this->_visualization->RecordersTreeElement()->AddLastChild(this->_treeElement);
}


/***********************************************~***************************************************/


WCVisRecorder::WCVisRecorder(WCVisualization *vis, const std::string &name, const WPUInt &maxRecords) :
	::WCVisFeature(vis, name, 0), _clearOnReset(true), _clearOnStart(false), _recordList(), _maxRecords(maxRecords) {
	//Initialize the recorder
	this->Initialize();
}


WCVisRecorder::WCVisRecorder(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCVisFeature( WCSerializeableObject::ElementFromName(element,"VisFeature"), dictionary),
	_clearOnReset(true), _clearOnStart(false), _recordList(), _maxRecords(0) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Set the clearOnReset flag
	this->_clearOnReset = WCSerializeableObject::GetBoolAttrib(element, "clearOnReset");
	//Set the clearOnStart flag
	this->_clearOnStart = WCSerializeableObject::GetBoolAttrib(element, "clearOnStart");
	//Set the max records
	this->_maxRecords = (WPUInt)WCSerializeableObject::GetFloatAttrib(element, "maxRecords");

	//Initialize the recorder
	this->Initialize();
}


WCVisRecorder::~WCVisRecorder() {
	//Remove from the visualization
	if (!this->_visualization->RemoveFeature(this, false)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisRecorder::~WCVisRecorder - Problem removing feature from visualization.");	
	}
	//Make sure buffer is cleared
	this->ClearBuffer();
}


void WCVisRecorder::OnReceiveData(const unsigned int &type, void* data) {
	unsigned int size;

/*** DEBUG ***/
	//Set size based on type
	size = 88;
/*** DEBUG ***/

	//Create a local copy of the data
	void* localCopy = malloc(size);
	memcpy(localCopy, data, size);

	//Create a record
	WSSignal record;
#ifdef __APPLE__
	record.receivedTime = UpTime();
#elif __WIN32__
	record.receivedTime = GetTickCount();
#endif
	record.descriptor = NULL;
	record.data = localCopy;

	//Need to lock the recorder
	this->Lock();

	//Insert the record into the list
	this->_recordList.push_back(record);
	//Make sure size doesn't exceed max
	while (this->_recordList.size() > this->_maxRecords) {
		//Delete the data
		free(this->_recordList.front().data);
		//Remove record
		this->_recordList.pop_front();
	}

	//Unlock the recorder
	this->Unlock();
}


void WCVisRecorder::StartVisualization(void) {
	//See if buffer needs to be cleared
	if (this->_clearOnStart) this->ClearBuffer();
}


void WCVisRecorder::PauseVisualization(void) {
	//Nothing for now
}


void WCVisRecorder::StopVisualization(void) {
	//Show some info
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCVisRecorder::StopVisualization - " << this->_recordList.size() << " records written.");
}


void WCVisRecorder::ResetVisualization(void) {
	//See if buffer needs to be cleared
	if (this->_clearOnReset) this->ClearBuffer();
}


/***********************************************~***************************************************/

