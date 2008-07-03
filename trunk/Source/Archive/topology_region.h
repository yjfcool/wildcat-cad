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


#ifndef __WTPKL_REGION_USE_H__
#define __WTPKL_REGION_USE_H__


/*** Included Header Files ***/
#include "wtpkl.h"
#include "model_use.h"
#include "shell_use.h"


/*** Locally Defined Values ***/
//None


/*** Class Order ***/
//---CRegionUse


/*****************************************************************************/


class CRegionUse : public CTopologyUse<void*, CModelUse*, CRegionUse*> {
private:
	CShellUse							*_shellUses;						//!< Downward point list of ShellUses
private:
	CRegionUse();															//!< Deny access to the default constructor
	CRegionUse(const CRegionUse&);											//!< Deny access to the copy constructor
	CRegionUse& operator=(const CRegionUse&);								//!< Deny access to the equals operator
public:
	CRegionUse(CModelUse* parent);											//!< Primary constructor
	~CRegionUse();															//!< Default destructor
	
	CShellUse* GetShellUses(void)		{ return this->_shellUses; }		//!< Get the base pointer to this RegionUse's list of ShellUses
	void SetShellUses(CShellUse *shell)	{ this->_shellUses = shell; }		//!< Set the ShellUse pointer
	void AddShellUse(CShellUse *shell);										//!< Add a ShellUse to the list
	void Remove(void);														//!< Remove this RegionUse from the list of ru's in the ModelUse
	void Print(int indent=0);												//!< Print out the contents of this region
};


/*****************************************************************************/


#endif //__WTPKL_REGION_USE_H__

