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


#ifndef __WILDCAT_UTILITY_LIBRARY_H__
#define __WILDCAT_UTILITY_LIBRARY_H__


/***********************************************~***************************************************/


/*** Included STDLIB Headers ***/
#include <iostream>
#include <ostream>
#include <vector>
#include <list>
#include <queue>
#include <set>
#include <map>
#include <stack>
#include <fstream>
#include <sstream>
#include <math.h>


/*** Included OpenGL Headers ***/
#ifdef __APPLE__
#include <Utility/OSX/gl_osx.h>
#define STDMIN	std::min
#define STDMAX	std::max
#define STDFABS	std::fabs
#endif

#ifdef __WXWINDOWS__
#include "Application/Win32/stdafx.h"	//Generic header file For Windows
#include "Application/Win32/win_gl.h"	//Header file for windows platform
#include <wx/wx.h>
#include <wx/docview.h>
#include <wx/stdpaths.h>
#include <wx/config.h>


//Defines for std:: problem
#define STDMIN	min
#define STDMAX	max
#define STDFABS	fabs

//Stupid workaround since Windows can't find round
inline double round(const double &n) {
	double flr = floor(n);
	if ((n - flr) >= 0.5) return flr + 1.0;
	else return flr;
}

#endif


/*** Included Xerces Headers ***/
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>


/*** Locally Defined Values ***/
//Namespace declaration
#define __WILDCAT_NAMESPACE__					Wildcat

//Configuration Values
#define __WILDCAT_DEBUG_LOGGER__
#define __WILDCAT_NO_THREADS__

//Define Logger Values
#define LOGGER_DEBUG							1
#define LOGGER_INFO								2
#define LOGGER_WARN								3
#define LOGGER_ERROR							4
#define LOGGER_FATAL							5
#define LOGGER_NONE								99


/***********************************************~***************************************************/


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


/*** Related Header Files ***/
#include <Utility/types.h>


/*** External or Global Variables ***/
//None


/*** Used Namespaces ***/
using namespace __WILDCAT_NAMESPACE__;


/***********************************************~***************************************************/


/*** Directory Functions ***/
std::string _ResourceDirectory(void);
std::string _ApplicationDirectory(void);
std::string _FontDirectory(void);
std::string _UserSettingsDirectory(void);
std::string _UserDocumentsDirectory(void);


/***********************************************~***************************************************/


#endif //__WILDCAT_UTILITY_LIBRARY_H__

