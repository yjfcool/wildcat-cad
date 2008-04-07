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
#include "RTVisualization/vis_listener.h"
#include "RTVisualization/vis_listener_controller.h"
#include "RTVisualization/visualization.h"
#include "RTVisualization/vis_feature.h"
#include "RTVisualization/vis_packet.h"
#include "RTVisualization/vis_recorder.h"


//Locally Defined Values
#define SERV_MAXLINE							1024


/***********************************************~***************************************************/


void* WCVisListener::ThreadEntryPoint(void* listener) {
	//Make sure we have a valid visListener object
	WCVisListener *lis = (WCVisListener*)listener;
	//Otherwise, start listening
	lis->Listen();
	//When done listening, quit thread
	return NULL;
}


void WCVisListener::Initialize(void) {
	//Initialize the socket
	this->_socket = socket(AF_INET, SOCK_DGRAM, 0);
	//Make sure socket is valid
	if (this->_socket < 0) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisListener::WCVisListener - Socket was not successful.");
		//throw error
		return;
	}
	bzero(&this->_serverAddress, sizeof(this->_serverAddress));
	//Set up server address information
	this->_serverAddress.sin_family      = AF_INET;
	this->_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	this->_serverAddress.sin_port        = htons(this->_port);

	//Bind to the socket
	int retVal = bind(this->_socket, (sockaddr*)&this->_serverAddress, sizeof(this->_serverAddress));
	//Make sure the bind was successful
	if (retVal != 0) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisListener::WCVisListener - Bind was not successful.");
		//throw error
	}
}


/***********************************************~***************************************************/



WCVisListener::WCVisListener(WCVisualization *vis, const std::string &name, const unsigned int &port) :
	::WCVisFeature(vis, name, 0), _isValid(true), _listener(NULL), _port(port), _socket(0), _recorder(NULL) {
	//Initialize the listener
	this->Initialize();
}


WCVisListener::WCVisListener(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCVisFeature( WCSerializeableObject::ElementFromName(element,"VisFeature"), dictionary),
	_isValid(true), _listener(NULL), _port(0), _socket(0), _recorder(NULL) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Set the port
	this->_port = (unsigned int)WCSerializeableObject::GetFloatAttrib(element, "port");
	//See about setting the recorder
	this->_recorder = (WCVisRecorder*)WCSerializeableObject::GetGUIDAttrib(element, "recorder", dictionary);

	//Initialize the listener
	this->Initialize();
}

WCVisListener::~WCVisListener() {
	//Shutdown the thread
	this->_isValid = false;
	//Wait for the thread to exit
	pthread_join(this->_listener, NULL);
	//Close the socket
	int retVal = close(this->_socket);
	//Make sure close was successful
	if (retVal < 0) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisListener::~WCVisListener - Not able to close socket");
}

/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCVisListener &listener) {
	//Print out some info
	out << "Listener(" << &listener << ")\n";
	//Print the scene
//	out << "\t" << *(vis._scene);
//	//Print the tree
//	out << "\t" << *(vis._treeView);
//	//Print all of the features in order
//	out << "\t Features(" << vis._featureMap.size() << ")\n";
//	std::list<WCVisFeature*>::iterator featureIter;
//	for (featureIter = part._featureList.begin(); featureIter != part._featureList.end(); featureIter++) {
//		out << "\t" << *featureIter << std::endl;
//	}
	return out;
}


/***********************************************~***************************************************/

