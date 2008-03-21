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


#ifndef __WUTIL_LOG_APPENDERS_H__
#define __WUTIL_LOG_APPENDERS_H__


/*** Included Wildcat Header Files ***/
#include "Utility/wutil.h"


/*** Locally Defined Values ***/
#define FILEAPPENDER_FLAGS		std::ios::out | std::ios::ate | std::ios::app


/*** Class PreDefines ***/
//None


/***********************************************~***************************************************/


class WCLogAppender {
public:
	virtual void ForcedLog(const int &level, const std::string &msg, const int &line,				//!< Required method
												const std::string &file)=0;
	virtual ~WCLogAppender()					{ }
};


/***********************************************~***************************************************/


class WCConsoleAppender : public WCLogAppender {
private:
	int											_count[6];											//!< Count of logging messages at each level
public:
	WCConsoleAppender();																			//!< Default constructor
	~WCConsoleAppender()						{ }													//!< Default destructor
	void ForcedLog(const int &level, const std::string &msg, const int &line,						//!< Required logging method
												const std::string &file);
};


/***********************************************~***************************************************/


class WCFileAppender : public WCLogAppender {
private:
	std::ofstream							_stream;												//!< File stream object
	int										_count[6];												//!< Count of logging messages at each level
public:
	WCFileAppender(const std::string &fileName);													//!< Primary constructor
	~WCFileAppender();																				//!< Default destructor
	void ForcedLog(const int &level, const std::string &msg, const int &line,						//!< Required logging method
												const std::string &file);
};


/***********************************************~***************************************************/


/*** To-Do List ***/
// - Logger not thread safe (see ::RootLogger)


/***********************************************~***************************************************/


#endif //__WUTIL_LOG_APPENDERS_H__

