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
#include "RTVisualization/vis_feature.h"
#include "RTVisualization/visualization.h"

/*** Included Feature Headers ***/
#include "RTVisualization/vis_recorder.h"
#include "RTVisualization/vis_udp_listener.h"
#include "RTVisualization/vis_motiontracker.h"


/***********************************************~***************************************************/


WCVisFeature::WCVisFeature(WCVisualization *vis, const std::string &name, const unsigned int &id) : ::WCFeature(vis, name),
	_visualization(vis), _id(id) {
	//Initialize thread mutex
	int retVal = pthread_mutex_init(&this->_mutex, NULL);
	//Make sure return is happy
	if (retVal != 0) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisFeature::WCVisFeature - Bad return from pthread_mutex_init: " << retVal);
}


WCVisFeature::WCVisFeature(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCFeature( WCSerializeableObject::ElementFromName(element,"Feature"), dictionary),
	_visualization(NULL), _id(0) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Initialize thread mutex
	int retVal = pthread_mutex_init(&this->_mutex, NULL);
	//Make sure return is happy
	if (retVal != 0) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisFeature::WCVisFeature - Bad return from pthread_mutex_init: " << retVal);

	//Set the visualization attribute
	this->_visualization = (WCVisualization*)WCSerializeableObject::GetGUIDAttrib(element, "visualization", dictionary);
	//Set the feature ID
	this->_id = (unsigned int)WCSerializeableObject::GetFloatAttrib(element, "id");
}


WCVisFeature::~WCVisFeature() {
	//Nothing to do here
}


bool WCVisFeature::Name(const std::string &name) {
	return false;
}


void WCVisFeature::MarkDirty(void) {
}


void WCVisFeature::Lock(void) {
	//Try to acquire the mutex lock
	int retVal = pthread_mutex_lock(&this->_mutex);
	//Make sure return is happy
	if (retVal != 0) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisFeature::Lock - Bad return from pthread_mutex_lock: " << retVal);
}


void WCVisFeature::Unlock(void) {
	//Give up the mutex lock
	int retVal = pthread_mutex_unlock(&this->_mutex);
	//Make sure return is happy
	if (retVal != 0) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisFeature::Unlock - Bad return from pthread_mutex_unlock: " << retVal);
}


xercesc::DOMElement* WCVisFeature::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


bool WCVisFeature::Deserialize(xercesc::DOMElement* featureElement, WCSerialDictionary *dictionary) {
	//Make sure feature element is not null
	if ((featureElement == NULL) || (dictionary == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisFeature::Deserialize - Feature element or dictionary was NULL.");
		//throw error
		return false;
	}
	//Get the feature name
	const XMLCh* xmlString = featureElement->getTagName();
	std::string name( xercesc::XMLString::transcode(xmlString) );

	//Find the feature to create
	if (name == "VisDataRecorder") {
		//Create the data recorder feature
		new WCVisRecorder(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "VisUDPListener") {
		//Create the UDP Listener
		new WCVisUDPListener(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "VisMotionTracker") {
		//Create the plane feature
		new WCVisMotionTracker(featureElement, dictionary);
		//All is good here
		return true;
	}
	//Unknown feature case
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCVisFeature::Deserialize - Unknown part feature: " << name);
	return false;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCVisFeature &feature) {
	return out;
}


/***********************************************~***************************************************/

