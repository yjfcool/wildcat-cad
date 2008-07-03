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


#ifndef __WTPKL_FACE_USE_H__
#define __WTPKL_FACE_USE_H__


/*** Included Header Files ***/
#include "wtpkl.h"
#include "shell_use.h"
#include "loop_use.h"


/*** Locally Defined Values ***/
//None


/*** Class Order ***/
//---CFaceUse


/*****************************************************************************/


class CFaceUse : public CTopologyUse<CSurface*, CShellUse*, CFaceUse*> {
private:
	int				_orientation;											//!< Orientation of FaceUse compared to Face
	CLoopUse		*_loopUses;												//!< Pointer to start of linked list of this face's LoopUses
private:
	CFaceUse();																//!< Deny access to default constructor
	CFaceUse(const CFaceUse&);												//!< Deny access to copy constructor
	CFaceUse& operator=(const CFaceUse&);									//!< Deny access to equals operator
public:
	CFaceUse(CSurface* pair, CShellUse* parent, COrient orientation);		//!< Primary constructor for the FaceUse
	~CFaceUse();															//!< Default destructor for FaceUse
	
	int GetOrientation(void)		{ return this->_orientation; }			//!< Return the value of the orientation
	CLoopUse* GetLoopUses(void)		{ return this->_loopUses; }				//!< Return a pointer to the linked list of related LoopUses
	void SetLoopUses(CLoopUse* loopUse) { this->_loopUses = loopUse; }		//!< Set the loopUse pointer
	void AddLoopUse(CLoopUse* loopUse);										//!< Add a LoopUse into the linked list
	void Remove(void);														//!< Remove this FaceUse from the parent shell's list
	void Print(int indent=0);												//!< Print the contents of this FaceUse
};



/*****************************************************************************/


#endif //__WTPKL_FACE_USE_H__

