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


#ifndef __WILDCAT_TOPOLOGY_KERNEL_LIBRARY_H__
#define __WILDCAT_TOPOLOGY_KERNEL_LIBRARY_H__


/*****************************************************************************/


//Included Utility Headers
#include "Utility/wutil.h"
#include "Utility/adapter.h"
#include "Utility/bounding_object.h"
#include "Utility/color.h"
#include "Utility/log_manager.h"
#include "Utility/matrix.h"
#include "Utility/object.h"
#include "Utility/quaternion.h"
#include "Utility/serializeable_object.h"
#include "Utility/shader_manager.h"
#include "Utility/vector.h"
#include "Utility/visual_object.h"


/*****************************************************************************


//Defines Needed for COrient
#define ORIENT_UNSPECIFIED		-1
#define ORIENT_SAME				1
#define ORIENT_OPPOSITE			0

class COrient {
private:
	int							_orientation;
	COrient(int orientation) : _orientation(orientation) {}					//!<
public:
	int GetOrientation(void)		{ return this->_orientation; }			//!<
	static COrient Unspecified(void){ return COrient(ORIENT_UNSPECIFIED); }	//!<
	static COrient Same(void)		{ return COrient(ORIENT_SAME); }		//!<
	static COrient Opposite(void)	{ return COrient(ORIENT_OPPOSITE); }	//!<
};


/*****************************************************************************


//Defines Needed for CDirection
#define DIRECTION_UNSPECIFIED				-1
#define DIRECTION_CLOCKWISE					1
#define DIRECTION_COUNTERCLOCKWISE			0

class CDirection {
private:
	int							_direction;
	CDirection(int direction) : _direction(direction) {}										//!<
public:
	int GetDirection(void)					{ return this->_direction; }						//!<
	static CDirection Unspecified(void)		{ return CDirection(DIRECTION_UNSPECIFIED); }		//!<
	static CDirection Clockwise(void)		{ return CDirection(DIRECTION_CLOCKWISE); }			//!<
	static CDirection CounterClockwise(void){ return CDirection(DIRECTION_COUNTERCLOCKWISE); }	//!<
};


/*****************************************************************************/


/*** External or Global Variables ***/
//None


/*****************************************************************************/


/*** To-Do List ***/
//Rework CDirection and COrient 
//Create optimized topology (especially eu) search functions
//Check for m_keml deleting the reference edge to the parent LoopUse
//Look into what it will take to flip the orientation of a face (see the problem in m_keml)


/*****************************************************************************/


#endif //__WILDCAT_TOPOLOGY_KERNEL_LIBRARY_H__

