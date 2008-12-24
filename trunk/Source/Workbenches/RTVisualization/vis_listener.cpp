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
#include <RTVisualization/vis_listener.h>
#include <RTVisualization/visualization.h>
#include <RTVisualization/vis_packet.h>
#include <RTVisualization/vis_recorder.h>


//Locally Defined Values
#define SERV_MAXLINE							1024


/***********************************************~***************************************************/


void WCVisListener::UDPInitialize(void) {
	//Initialize the socket (normal sockets)
	this->_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//Make sure socket is valid
	if (this->_socket < 0) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisListener::UDPInitialize - Socket was not successful.");
		//throw error
		return;
	}
	//Zero out the server address
	memset(&this->_serverAddress, 0, sizeof(this->_serverAddress));
	//Set up server address information
	this->_serverAddress.sin_family      = AF_INET;
	this->_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	this->_serverAddress.sin_port        = htons(this->_port);

	//Bind to the socket
	int retVal = bind(this->_socket, (sockaddr*)&this->_serverAddress, sizeof(this->_serverAddress));

	//Make sure the bind was successful
	if (retVal != 0) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisListener::UDPInitialize - Bind was not successful.");
		//throw error
		return;
	}
	//Otherwise, set isValid to true
	this->_isValid = true;
}


void WCVisListener::UDPListen(void) {
	WSVisualizationHeader *header;
	unsigned int headerSize = sizeof(WSVisualizationHeader);
	int n;
	void *data = malloc(SERV_MAXLINE), *payload;
	WCVisFeature *feature;

	//Setup receive timeout for socket
#ifndef __WIN32__
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(this->_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#else
	DWORD tv = 1000;
	setsockopt(this->_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
#endif

	//Continue until no longer valid
	while (this->_isValid) {
		//Try to get some data (remember there is a one second timeout)
		n = recvfrom(this->_socket, (char*)data, SERV_MAXLINE, 0, NULL, NULL);
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "Received: " << n);
		//Make sure visualization is valid, has read something, and is running
		if ((this->_isValid) && (n > 0) && 
			(this->_visualization->State() == WCVisualizationState::Running())) {
			//Set data to a header
			header = (WSVisualizationHeader*)data;
			CLOGGER_DEBUG(WCLogManager::RootLogger(), "Received data(ID: " << header->objectID << ", Size: " << header->size << ", Type: " << header->type << ")");
			//Check that header size and n agree
			if (n < (int)header->size) n = header->size;
			//Allocate space for remaining data
			payload = calloc(1, n);
			//Copy message body into data
			memcpy(payload, (unsigned char*)data + headerSize, n);
			//Send data to vis feature
			feature = this->_visualization->FeatureFromID(header->objectID);
			if (feature != NULL) feature->OnReceiveData(header->type, payload);

			//See about recording the data
			if (this->_recorder != NULL) this->_recorder->OnReceiveData(header->type, data);
		}
	}
	//Free the data
	free(data);
	//Close the socket
#ifndef __WIN32__
	int retVal = close(this->_socket);
#else
	int retVal = closesocket(this->_socket);
#endif
	//Make sure close was successful
	if (retVal < 0) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisListener::UDPListen - Not able to close socket");
	//Exit the thread
	pthread_exit(NULL);
}


void WCVisListener::TCPInitialize(void) {
#ifndef __WIN32__
	//Initialize the socket
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	//Make sure socket is valid
	if (this->_socket < 0) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisListener::TCPInitialize - Socket was not successful.");
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
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisListener::TCPInitialize - Bind was not successful.");
		//throw error
		return;
	}
	//Otherwise, set isValid to true
	this->_isValid = true;
#endif
}


void WCVisListener::TCPListen(void) {
#ifndef __WIN32__
	socklen_t	len;
//	WSVisualizationHeader *header;
//	size_t headerSize = sizeof(WSVisualizationHeader);
	void *data = malloc(SERV_MAXLINE);
//	void *payload;
//	WCVisFeature *feature;
	int connFD = -1;
	ssize_t n = -1;

	//Make accept not block
	int val = fcntl(this->_socket, F_SETFL, 0);
	fcntl(this->_socket, F_SETFL, val | O_NONBLOCK);
	//Setup receive timeout for socket
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(this->_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	//Continue until no longer valid
	while (this->_isValid) {
		//Wait for a connection (allow very little backlog for now)
		listen(this->_socket, 3);
		
		//Accept an inbound connection (spin a lot)
		while ((connFD == -1) && this->_isValid)
			connFD = accept(this->_socket, NULL, &len);
		if (connFD != -1) std::cout << "ConnFD: " << connFD << std::endl;
		
		//Close the socket
		close(this->_socket);
		
		//Continue again until no longer valid
		while ((connFD != -1) && (n != 0) && this->_isValid) {
			//Try to get some data (remember there is a one second timeout)
			n = recvfrom(this->_socket, data, SERV_MAXLINE, 0, NULL, &len);
			std::cout << "Received: " << n << std::endl;
			//Make sure visualization is valid, has read something, and is running
			if ((this->_isValid) && (n > 0) && (this->_visualization->State() == WCVisualizationState::Running())) {
//				//Set data to a header
//				header = (WSVisualizationHeader*)data;
//				std::cout << "Received data(ID: " << header->objectID << ", Size: " << header->size << ", Type: " << header->type << ")\n";
//				//Check that header size and n agree
//				if (n < header->size) n = header->size;
//				//Allocate space for remaining data
//				payload = calloc(1, n);
//				//Copy message body into data
//				memcpy(payload, (unsigned char*)data + headerSize, n);
//				//Send data to vis feature
//				feature = this->_visualization->FeatureFromID(header->objectID);
//				if (feature != NULL) feature->OnReceiveData(header->type, payload);
//
//				//See about recording the data
//				if (this->_recorder != NULL) this->_recorder->OnReceiveData(header->type, data);
			}
		}
	}
	//Free the data
	free(data);
	//Exit the thread
	pthread_exit(NULL);
#endif
}


void* WCVisListener::ThreadEntryPoint(void* listener) {
	//Make sure we have a valid visListener object
	WCVisListener *lis = (WCVisListener*)listener;

	//Otherwise, start listening (based on type)
	if (lis->Type() == WCVisListenerType::UDP()) lis->UDPListen();
	else if (lis->Type() == WCVisListenerType::TCP()) lis->TCPListen();

	//When done listening, quit thread
	return NULL;
}


void WCVisListener::Initialize(void) {
	//Check feature name
	if (this->_name == "") this->_name = this->_visualization->GenerateFeatureName(this);
	//Create listener controller
	this->_controller = new WCVisListenerController(this);
	//Create tree element and add into the tree (beneath the listeners element)
	WSTexture *icon = this->_document->Scene()->TextureManager()->TextureFromName("listener32");
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, icon);
	this->_visualization->ListenersTreeElement()->AddLastChild(this->_treeElement);
	//Add self to visualization
	this->_visualization->AddFeature(this, false);

	//Initialize based on type
	if (this->_type == WCVisListenerType::UDP()) this->UDPInitialize();
	else if (this->_type == WCVisListenerType::TCP()) this->TCPInitialize();
	//Create thread and let it get to listening
	pthread_create(&this->_listener, NULL, WCVisListener::ThreadEntryPoint, this);
}


/***********************************************~***************************************************/



WCVisListener::WCVisListener(WCVisualization *vis, const std::string &name, const WCVisListenerType &type, const unsigned int &port) :
	::WCVisFeature(vis, name, 0),  _type(type), _isValid(false), _listener(), _port(port), _socket(0), _recorder(NULL) {
	//Initialize the listener
	this->Initialize();
}


WCVisListener::WCVisListener(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCVisFeature( WCSerializeableObject::ElementFromName(element,"VisFeature"), dictionary),
	 _type(WCVisListenerType::UDP()), _isValid(false), _listener(), _port(0), _socket(0), _recorder(NULL) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Set the port
	this->_port = (unsigned int)WCSerializeableObject::GetFloatAttrib(element, "port");
	//Set the type
	//...

	//See about setting the recorder
	this->_recorder = (WCVisRecorder*)WCSerializeableObject::GetGUIDAttrib(element, "recorder", dictionary);

	//Initialize the listener
	this->Initialize();
}

WCVisListener::~WCVisListener() {
	//Shutdown the thread
	this->_isValid = false;
#ifndef __WIN32__
	//Wait for the thread to exit
	pthread_join(this->_listener, NULL);
#endif

	//Remove from the sketch
	if (!this->_visualization->RemoveFeature(this, false)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisListener::~WCVisListener - Problem removing feature from visualization.");	
	}
}


bool WCVisListener::Regenerate(void) {
	return true;
}


xercesc::DOMElement* WCVisListener::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCDrawingMode* WCVisListener::ModeCreate(WCVisWorkbench *wb) {
	//Create a new creation mode
	return new WCModeVisListenerCreate(wb);
}


WCDrawingMode* WCVisListener::ModeModify(WCVisListener *listener) {
	//Create a new modify mode
	return NULL;
}


WCActionVisListenerCreate* WCVisListener::ActionCreate(WCVisualization *vis, const std::string &listenerName, 
	const WCVisListenerType &type, const unsigned int &port) {
	//Create new action for creating a listener
	return new WCActionVisListenerCreate(vis, listenerName, type, port);
}


WCActionVisListenerModify* WCVisListener::ActionModify(WCVisListener *listener, const std::string &listenerName,
	const WCVisListenerType &type, const unsigned int &port) {
	//Create new action for modifying a listener
	return new WCActionVisListenerModify(listener, listenerName, type, port);
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCVisListener &listener) {
	//Print out some info
	out << "Listener(" << &listener << ")\n";

	//Print name info
	out << "\tName: " << listener._name << std::endl;
	//Print type info
	out << "\tType: ";
	if (listener._type == WCVisListenerType::UDP()) out << "UDP\n";
	if (listener._type == WCVisListenerType::TCP()) out << "TCP\n";
	//Print the port info
	out << "\tPort: " << listener._port << std::endl;

	//Return the out
	return out;
}


/***********************************************~***************************************************/

