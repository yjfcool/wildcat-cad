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


#ifndef __VIS_LISTENER_H__
#define __VIS_LISTENER_H__


/*** Included Header Files ***/
#include "RTVisualization/wrtv.h"
#include "RTVisualization/vis_feature.h"


/*** Local Defines ***/
#define VISLISTENER_CLASSNAME					"Listener"


/*** Class Predefines ***/
class WCVisRecorder;


/***********************************************~***************************************************/


class WCVisListener : public WCVisFeature {
protected:
	bool										_isValid;											//!< Valid flag
	pthread_t									_listener;											//!< Listener thread
	unsigned int								_port;												//!< Port to listen on
	int											_socket;											//!< Socket file descriptor
	struct sockaddr_in							_serverAddress;										//!< Server address structure
	WCVisRecorder								*_recorder;											//!< Options data recorder
protected:
	virtual void Listen(void)=0;																	//!< Listen for data on the port
	static void* ThreadEntryPoint(void* lisener);													//!< Primary thread entry callback
	void Initialize(void);																			//!< Initialization method
private:
	//Hidden Constructors
	WCVisListener();																				//!< Deny access to default constructor
	WCVisListener(const WCVisListener&);															//!< Deny access to copy constructor
	WCVisListener&operator=(const WCVisListener&);													//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCVisListener(WCVisualization *vis, const std::string &name, const unsigned int &port);			//!< Primary constructor
	WCVisListener(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	virtual ~WCVisListener();																		//!< Default destructor
	
	//Member Access Methods
	virtual unsigned int Port(void)				{ return this->_port; }								//!< Handle renaming the feature
	virtual inline WCVisRecorder* Recorder(void){ return this->_recorder; }							//!< Get the data recorder
	virtual inline void Recorder(WCVisRecorder *recorder){ this->_recorder = recorder; }			//!< Set the data recorder

	//Visualization Methods
	virtual void OnReceiveData(const unsigned int &type, void* data) { }							//!< Receive data from a tracker
	virtual void StartVisualization(void)		{ }													//!< Signal start of the visualization
	virtual void PauseVisualization(void)		{ }													//!< Signal a pause in the visualization
	virtual void StopVisualization(void)		{ }													//!< Signal end of the visualization
	virtual void ResetVisualization(void)		{ }													//!< Signal reset of the visualization

	//Required Inherited Methods
	virtual bool Regenerate(void)				{ return true; }									//!< Validate and rebuild

	/*** Friend Methods ***/
	friend std::ostream& operator<<(std::ostream& out, const WCVisListener &listener);				//!< Overloaded output operator	
};


/***********************************************~***************************************************/


#endif //__VIS_LISTENER_H__

