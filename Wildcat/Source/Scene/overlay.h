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


#ifndef __OVERLAY_H__
#define __OVERLAY_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"
#include "Scene/layer.h"
#include "Scene/widget.h"


/*** Local Defines ***/
//Overlay defaults
#define OVERLAY_BOUNDARY						8
#define OVERLAY_DEFAULT_COLOR					0.20, 0.20, 0.20, 0.55
#define OVERLAY_WIDTH_MIN						0.5
#define OVERLAY_HEIGHT_MIN						OVERLAY_WIDTH_MIN
#define OVERLAY_PLACEMENT_BORDER				0.25
#define OVERLAY_DEFAULT_PLACEMENT				WCPlacement::LowerLeft()


/*** Class Predefines ***/
class WCColor;


/***********************************************~***************************************************/


class WCOverlay : public WCWidget {
protected:
	bool										_isMouseOver;										//!< Is the mouse over
	WCColor										_color;												//!< Color of the overlay
	GLuint										_buffer;											//!< Buffer for vertex info
	WPUInt										_numVerts;											//!< Number of vertices in the overlay
	WCPlacement									_placement;											//!< Placement object
private:
	void GenerateVBO(void);																			//!< Generate the display VBO

	WCOverlay();																					//!< Deny access to the default constructor
public:
	WCOverlay(WCUserInterfaceLayer *layer, bool resize=true);										//!< Primary constructor
	virtual ~WCOverlay();																			//!< Default destructor
	
	//Inherited Member Functions
	virtual void SetSize(const WPFloat width, const WPFloat height);								//!< Set the size of the overlay
	virtual void SetPlacement(const WPFloat lowerLeft, const WPFloat lowerRight);					//!< Set the placement 
	virtual void SetPlacement(WCPlacement placement);												//!< Set the placement
	virtual void OnReshape(void);																	//!< Modify placement on resize
	virtual void OnMouseDown(const WCMouseButton &button, const WPFloat x, const WPFloat y);		//!< Receive message about mouse button being pressed	
	virtual void OnMouseEnter(const WPFloat x, const WPFloat y);									//!< Modified MouseEnter
	virtual void OnMouseExit(const WPFloat x, const WPFloat y);										//!< Modified MouseExit
	virtual void OnMouseMove(const WPFloat x, const WPFloat y);										//!< Receive message about mouse being moved
	virtual void Render(void);																		//!< Primary render entry point
	
	//Static Render Methods
	static void RenderRectangle(const WPFloat xMin, const WPFloat yMin, const WPFloat xMax,			//!< Render a square rectangle
												const WPFloat yMax, const WCColor &fillColor, const WCColor &edgeColor, const WCColor &shadeColor);
	static void RenderRoundedRectangle(const WPFloat xMin, const WPFloat yMin, const WPFloat xMax,	//!< Render a rounded rectangle
												const WPFloat yMax, const WCColor &fillColor, const WCColor &edgeColor, const WCColor &shadeColor);
};


/***********************************************~***************************************************/


#endif //__OVERLAY_H__

