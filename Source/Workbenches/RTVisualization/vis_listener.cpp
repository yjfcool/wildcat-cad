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


/***********************************************~***************************************************/



WCVisListener::WCVisListener(WCVisualization *vis, const std::string &name, const unsigned int &port) :
	::WCVisFeature(vis, name, 0), _isValid(true), _listener(NULL), _port(port), _socket(0) {
	//Initialize the socket
	this->_socket = socket(AF_INET, SOCK_DGRAM, 0);
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


WCVisListener::~WCVisListener() {
	//Shutdown the thread
	this->_isValid = false;
	//Wait for the thread to exit
	pthread_join(this->_listener, NULL);
	//Close the port
	//...
}


/***********************************************~***************************************************/

