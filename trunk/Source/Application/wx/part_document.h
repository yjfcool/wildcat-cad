// part_document.h
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


#ifndef __PART_DOCUMENT_H__
#define __PART_DOCUMENT_H__


/*** Included Header Files ***/
#include "PartDesign/part.h"
#include <wx/docview.h>



/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefinitions ***/
//None


/***********************************************~***************************************************/


class WCPartDocument: public wxDocument {
	DECLARE_DYNAMIC_CLASS(WCPartDocument)
protected:
	WCPart										*_part;												//!< Associated Wildcat part
	WCDocument									*_document;											//!< Associated Wildcat document
public:
	//Constructors and Destructors
	WCPartDocument(void);																			//!< Default constructor
	virtual ~WCPartDocument(void);																	//!< Default destructor

	//Member Access Methods
	inline WCPart* Part(void)					{ return this->_part; }								//!< Get the associated part
	inline WCDocument* Document(void)			{ return this->_document; }							//!< Get the associated document

	//wxDocument Virtual Functions
	bool OnCreate(const wxString& path, long flags);												//!<
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__PART_DOCUMENT_H__

