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


#ifndef __VIS_WORKBENCH_H__
#define __VIS_WORKBENCH_H__


/*** Included Header Files ***/
#include "RTVisualization/wrtv.h"
#include "Kernel/workbench.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCVisualization;
class WCCompass;
class WCFrameRateMonitor;
class WCGroundPlane;


/***********************************************~***************************************************/


class WCVisWorkbench : public WCWorkbench {
protected:
	WCVisualization								*_visualization;									//!< Associated visualization
	WCCompass									*_compass;											//!< Display compass widget
	WCFrameRateMonitor							*_frameRateMonitor;									//!< Display frame rate monitor widget
	WCGroundPlane								*_ground;											//!< Visualization ground plane
private:
	//Hidden Constructors
	WCVisWorkbench();																				//!< Deny access to default constructor
	WCVisWorkbench(const WCVisWorkbench&);															//!< Deny access to copy constructor
	WCVisWorkbench& operator=(const WCVisWorkbench&);												//!< Deny access to operator equals
public:
	//Constructors and Destructors
	WCVisWorkbench(WCVisualization *vis);															//!< Primary constructor
	~WCVisWorkbench();																				//!< Default destructor
	
	//Member Access Methods
	inline WCVisualization* Visualization(void)	{ return this->_visualization; }					//!< Get the current visualization

	//Methods
	void CreateInitialObjects(void);																//!< Crappy way to do this

	//Inherited Member Methods
	bool OnEnter(void);																				//!< Entrance method
	bool OnExit(void);																				//!< Forced exit method
	bool OnUserMessage(const WCUserMessage &message);												//!< React to user message
};


/***********************************************~***************************************************/


#endif //__VIS_WORKBENCH_H__

