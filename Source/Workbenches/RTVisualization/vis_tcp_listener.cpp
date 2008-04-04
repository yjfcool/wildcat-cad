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
#include "RTVisualization/vis_tcp_listener.h"
#include "RTVisualization/vis_listener_controller.h"
#include "RTVisualization/visualization.h"
#include "RTVisualization/vis_feature.h"
#include "RTVisualization/vis_packet.h"
#include "RTVisualization/vis_recorder.h"


//Locally Defined Values
#define SERV_MAXLINE							1024


/***********************************************~***************************************************/


void WCVisTCPListener::Listen(void) {
	socklen_t	len;
	WSVisualizationHeader *header;
	size_t headerSize = sizeof(WSVisualizationHeader);
	ssize_t n;
	void *data = malloc(SERV_MAXLINE), *payload;
	WCVisFeature *feature;
	
	//Setup receive timeout for socket
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(this->_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	//Continue until no longer valid
	while (this->_isValid) {
		//Try to get some data (remember there is a one second timeout)
		n = recvfrom(this->_socket, data, SERV_MAXLINE, 0, NULL, &len);
		//Make sure visualization is valid, has read something, and is running
		if ((this->_isValid) && (n > 0) && 
			(this->_visualization->State() == WCVisualizationState::Running())) {
			//Set data to a header
			header = (WSVisualizationHeader*)data;
//			std::cout << "Received data(ID: " << header->objectID << ", Size: " << header->size << ", Type: " << header->type << ")\n";
			//Allocate space for remaining data
			payload = calloc(1, header->size);
			//Copy message body into data
			memcpy(payload, (unsigned char*)data + headerSize, header->size);
			//Send data to vis feature
			feature = this->_visualization->FeatureFromID(header->objectID);
			feature->OnReceiveData(header->type, payload);

			//See about recording the data
			if (this->_recorder != NULL) this->_recorder->OnReceiveData(header->type, data);
		}
	}
	//Exit the thread
	pthread_exit(NULL);
}


/***********************************************~***************************************************/


WCVisTCPListener::WCVisTCPListener(WCVisualization *vis, const std::string &name, const unsigned int &port) :
	::WCVisListener(vis, name, port) {
	//Check feature name
	if (this->_name == "") this->_name = this->_visualization->GenerateFeatureName(this);
	//Create listener controller
	this->_controller = new WCVisListenerController(this);

	//Create tree element and add into the tree (beneath the listeners element)
	WSTexture *icon = this->_document->Scene()->TextureManager()->TextureFromName("listener32");
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, icon);
	this->_visualization->ListenersTreeElement()->AddLastChild(this->_treeElement);

	//Create thread and let it get to listening
	pthread_create(&this->_listener, NULL, WCVisListener::ThreadEntryPoint, this);
}


WCVisTCPListener::~WCVisTCPListener() {
	//Nothing for now (VisListener will shutdown all child threads)
}


/***********************************************~***************************************************/

