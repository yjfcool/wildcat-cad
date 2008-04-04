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


#ifndef __VIS_RECORDER_H__
#define __VIS_RECORDER_H__


/*** Included Header Files ***/
#include "RTVisualization/wrtv.h"
#include "RTVisualization/vis_feature.h"
#include "RTVisualization/vis_signal.h"


/*** Local Defines ***/
#define VISRECORDER_CLASSNAME					"Recorder"


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCVisRecorder : public WCVisFeature {
protected:
	bool										_clearOnReset;										//!< Clear the buffer on a reset
	bool										_clearOnStart;										//!< Clear the buffer on a start
	std::list<WSSignal>							_recordList;										//!< List of records
	WPUInt										_maxRecords;										//!< Value for maximum allowed records
private:
	void ClearBuffer(void);																			//!< Clear all records
	void Initialize(void);																			//!< Initialize the object
	//Hidden Constructors
	WCVisRecorder();																				//!< Deny access to default constructor
	WCVisRecorder(const WCVisRecorder&);															//!< Deny access to copy constructor
	WCVisRecorder& operator=(const WCVisRecorder&);													//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCVisRecorder(WCVisualization *vis, const std::string &name, const WPUInt &maxRecords);			//!< Primary constructor
	~WCVisRecorder();																				//!< Default destructor

	//Visualization Methods
	virtual void OnReceiveData(const unsigned int &type, void* data);								//!< Receive data from a tracker
	virtual void StartVisualization(void);															//!< Signal start of the visualization
	virtual void PauseVisualization(void);															//!< Signal a pause in the visualization
	virtual void StopVisualization(void);															//!< Signal end of the visualization
	virtual void ResetVisualization(void);															//!< Signal reset of the visualization

	//Required Inherited Methods
	virtual inline std::string RootName(void) const	{ return VISRECORDER_CLASSNAME; }				//!< Get the class name
	virtual bool Regenerate(void)				{ return true; }									//!< Validate and rebuild
};


/***********************************************~***************************************************/


#endif //__VIS_RECORDER_H__

