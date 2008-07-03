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


#ifndef __PART_PAD_MODES_H__
#define __PART_PAD_MODES_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/drawing_mode.h"


/*** Locally Defined Values ***/
#define PARTPADMODE_CREATE_NAME					"Pad Create Mode"
#define PARTPADMODE_EDIT_NAME					"Pad Edit Mode"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCPartWorkbench;
class WCPartPad;
class WCSketchProfile;


/***********************************************~***************************************************/


class WCModePartPadCreate : public WCDrawingMode {
private:
	WCPartWorkbench								*_workbench;										//!< Parent part workbench
	WPInt										_stage;												//!< Creation stage (nothing, profiles, 1st click, done)
	WCVector4									_mark;												//!< Mouse location
	WCMatrix4									_plane;												//!< Profile plane
	WCVector4									_direction;											//!< Extrusion direction
	std::list<std::list<WCSketchProfile*> >		_profiles;											//!< Associated list of lists of sketch profiles
	WCVector4									_corners[4];										//!< Bounding box corners
	WPFloat										_posDepth, _negDepth;								//!< Location of suggestion
	std::list<WCNurbsSurface*>					_surfaces;											//!< List of extrusion surfaces

	//Private Methods
	bool CheckSelections(void);																		//!< Private method to check what is selected
	void ProcessProfiles(const std::list<WCSketchProfile*> &profiles);								//!< Private method to process all profiles
	void GenerateSurfaces(void);																	//!< Private method to generate all surfaces
	
	//Deny Access
	WCModePartPadCreate();																			//!< Default constructor
public:
	//Constructors and Destructors
	WCModePartPadCreate(WCPartWorkbench *wb);														//!< Primary constructor
	~WCModePartPadCreate()						{ }													//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseDown(const WCMouseButton &button);													//!< Handle mouse button press
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse moves
	void OnMouseUp(const WCMouseButton &button);													//!< Handle mouse button release
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__PART_PAD_MODES_H__

