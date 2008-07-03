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


#ifndef __PART_WORKBENCH_H__
#define __PART_WORKBENCH_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/workbench.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCPart;
class WCCompass;
class WCFrameRateMonitor;


/***********************************************~***************************************************/


class WCPartWorkbench : public WCWorkbench {
protected:
	WCPart										*_part;												//!< Part the workbench is working with
	WCCompass									*_compass;											//!< Display compass widget
	WCFrameRateMonitor							*_frameRateMonitor;									//!< Display frame rate monitor widget

private:
	//Deny Access
	WCPartWorkbench(const WCPartWorkbench& bench);													//!< Deny access to copy constructor
	WCPartWorkbench& operator=(const WCPartWorkbench& bench);										//!< Deny access to operator equals
	
public:
	//Constructors and Destructors
	WCPartWorkbench(WCPart *part);																	//!< Primary constructor
	~WCPartWorkbench();																				//!< Default destructor
	
	//Member Access Methods
	WCPart* Part(void)							{ return this->_part; }								//!< Get the current part

	//Methods
	void CreateInitialObjects(void);																//!< Crappy way to do this

	//Inherited Member Methods
	bool OnEnter(void);																				//!< Entrance method
	bool OnExit(void);																				//!< Forced exit method
	bool OnUserMessage(const WCUserMessage &message);												//!< React to user message
//	bool DrawingMode(WCDrawingMode *mode);															//!< Set the mode

//	void OnPanPress(void);																			//!< React to pan press event
//	void OnPanRelease(void);																		//!< React to pan release event
//	void OnZoomPress(void);																			//!< React to zoom press event
//	void OnZoomRelease(void);																		//!< React to zoom release event
//	void OnRotatePress(void);																		//!< React to rotate press event
//	void OnRotateRelease(void);																		//!< React to rotate release event
//	void OnMultiSelectPress(void);																	//!< React to multi-select press event
//	void OnMultiSelectRelease(void);																//!< React to multi-select release event
//	void OnIdle(void);																				//!< React to idle event
//	void OnMouseMove(const int x, const int y);														//!< React to mouse movement even
//	void OnMouseDown(WCMouseButton button);															//!< React to mouse down event
//	void OnMouseUp(WCMouseButton button);															//!< React to mouse up event
//	void OnReshape(const int width, const int height);												//!< React to window reshape event
//	void Render(void);																				//!< Render the workbench
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__PART_WORKBENCH_H__

