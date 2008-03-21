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


/*** Included Header Files ***/
#include "Utility/log_appenders.h"


/***********************************************~***************************************************/


WCConsoleAppender::WCConsoleAppender() {
	this->_count[LOGGER_DEBUG] = 0;
	this->_count[LOGGER_INFO] = 0;
	this->_count[LOGGER_WARN] = 0;
	this->_count[LOGGER_ERROR] = 0;
	this->_count[LOGGER_FATAL] = 0;
}


void WCConsoleAppender::ForcedLog(const int &level, const std::string &msg, const int &line,
	const std::string &file) {
	//Switch on level
	switch(level) {
		case LOGGER_DEBUG: 
			this->_count[LOGGER_DEBUG]++;
			std::cout << "(DEBUG) - " << msg << std::endl;
			break;
		case LOGGER_INFO: 
			this->_count[LOGGER_INFO]++;
			std::cout << "(INFO) - " << msg << std::endl; 
			break;
		case LOGGER_WARN: 
			this->_count[LOGGER_WARN]++;
			std::cout << "(WARN) - " << msg << std::endl; 
			break;
		case LOGGER_ERROR: 
			this->_count[LOGGER_ERROR]++;
			std::cout << "(ERROR) - " << msg << std::endl; 
			break;
		case LOGGER_FATAL: 
			this->_count[LOGGER_FATAL]++;
			std::cout << "(FATAL) - " << msg << std::endl; 
			break;
	}
}


/***********************************************~***************************************************/


WCFileAppender::WCFileAppender(const std::string &fileName) {
	//Open a ifstream with the given filename
	this->_stream.open(fileName.c_str(), FILEAPPENDER_FLAGS);
	//Make sure the stream is open for writing
	if (!this->_stream) {
		std::cerr << "WCFileAppender::WCFileAppender - Error: Not able to open file: " << fileName << " for output.\n";
		return;
	}
}


WCFileAppender::~WCFileAppender() {
	//Check to see if the ifstream is open
	if (this->_stream) {
		//Close the ifstream
		this->_stream.close();
	}
}


void WCFileAppender::ForcedLog(const int &level, const std::string &msg, const int &line,
	const std::string &file) {
	//Switch on level
	switch(level) {
		case LOGGER_DEBUG: 
			this->_count[LOGGER_DEBUG]++;
			this->_stream << "(DEBUG) - " << msg << std::endl;
			break;
		case LOGGER_INFO: 
			this->_count[LOGGER_INFO]++;
			this->_stream << "(INFO) - " << msg << std::endl; 
			break;
		case LOGGER_WARN: 
			this->_count[LOGGER_WARN]++;
			this->_stream << "(WARN) - " << msg << std::endl; 
			break;
		case LOGGER_ERROR: 
			this->_count[LOGGER_ERROR]++;
			this->_stream << "(ERROR) - " << msg << std::endl; 
			break;
		case LOGGER_FATAL: 
			this->_count[LOGGER_FATAL]++;
			this->_stream << "(FATAL) - " << msg << std::endl; 
			break;
	}
}


/***********************************************~***************************************************/

