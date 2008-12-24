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
#include <Utility/log_manager.h>
#include <Utility/log_appenders.h>
#include <Utility/assert_exception.h>


/*** Static Member Initialization ***/
int WCLogManager::_refCount = 0;
WCLogger* WCLogManager::_rootLogger = NULL;
WCLogAppender* WCLogManager::_rootAppender = NULL;
std::map<std::string,WCLogger*>* WCLogManager::_loggerMap = NULL;


/***********************************************~***************************************************/


void __WILDCAT_NAMESPACE__::_LogCheck(int level, WCLogger* logger, std::string msg, int line, std::string file) {
	//Check to see if the logger is valid or not
	if (!WCLogManager::CheckLogger(logger)) {
		std::cerr << "LogCheck Error - Tried logging to an invalid logger...\n";
		std::cerr << "File: " << file << " On line: " << line << std::endl;
		return;	
	}
	//If it is valid, then log away
	switch(level) {
		case LOGGER_DEBUG:
			if (logger->IsDebugEnabled()) logger->ForcedLog(LOGGER_DEBUG, msg, __LINE__, __FILE__);
			break;
		case LOGGER_INFO:
			if (logger->IsInfoEnabled()) logger->ForcedLog(LOGGER_INFO, msg, __LINE__, __FILE__);
			break;
		case LOGGER_WARN:
			if (logger->IsWarnEnabled()) logger->ForcedLog(LOGGER_WARN, msg, __LINE__, __FILE__);
			break;
		case LOGGER_ERROR:
			if (logger->IsErrorEnabled()) logger->ForcedLog(LOGGER_ERROR, msg, __LINE__, __FILE__);
			break;
		case LOGGER_FATAL:
			if (logger->IsFatalEnabled()) logger->ForcedLog(LOGGER_FATAL, msg, __LINE__, __FILE__);
			break;
	}
}


/***********************************************~***************************************************/


WCLogger::WCLogger(const std::string &name, const WCLoggerLevel &level, WCLogAppender *appender) : 
	_refCount(1), _name(name), _level(level), _appender(appender) { 
}


WCLogger::~WCLogger() {
}


void WCLogger::Appender(WCLogAppender *appender) {
	//Make sure appender exists
	ASSERT(appender);
	//Set the appender
	this->_appender = appender;
}	


void WCLogger::ForcedLog(const int &level, const std::string &msg, const int &line, const std::string &file) {
	this->_appender->ForcedLog(level, msg, line, file);
}


/***********************************************~***************************************************/


bool WCLogManager::Initialize(const WCLoggerLevel &level, WCLogAppender *defaultAppender) {
	//Increment the reference counter
	WCLogManager::_refCount++;
	//Check to see if the LogManager has already been started
	if (WCLogManager::_rootLogger != NULL) {
		return true;
	}
	//See if default appender was attached
	if (defaultAppender) {
		WCLogManager::_rootAppender = defaultAppender;
	}
	//Otherwise, create the default LogAppender
	else {
		WCLogManager::_rootAppender = new WCConsoleAppender();
	}
	//Now set up the logger mapping table
	WCLogManager::_loggerMap = new std::map<std::string,WCLogger*>;
	//Create the default Logger
	WCLogManager::_rootLogger = WCLogManager::CreateLogger("Default", WCLogManager::_rootAppender, level);
	CLOGGER_WARN(WCLogManager::_rootLogger, "CLogManager::Initializing...");
	return true;
}


bool WCLogManager::Terminate(void) {
	//Check to make sure the log manager has been started
	if (WCLogManager::_refCount == 0) {
		std::cerr << "WCLogManager::Terminate Error - LogManager already Terminate completely\n";
		return false;
	}
	//Check to see if this is just a reference count decrement or a true Terminate
	if (WCLogManager::_refCount > 1) {
		//Decrement the reference count
		WCLogManager::_refCount--;
		CLOGGER_DEBUG(WCLogManager::_rootLogger, "WCLogManager::Terminate - " << WCLogManager::_refCount << " references remaining");
		return false;
	}
	CLOGGER_WARN(WCLogManager::_rootLogger, "WCLogManager::Shutting Down (" << WCLogManager::_loggerMap->size() << ":loggers) ...");
	//Delete the default Appender
	delete WCLogManager::_rootAppender;
	//Delete all of the loggers
	while (!WCLogManager::_loggerMap->empty()) {
		//Delete the logger
		delete (WCLogManager::_loggerMap->begin()->second);
		//Remove it from the map
		WCLogManager::_loggerMap->erase( WCLogManager::_loggerMap->begin() );
	}
	//Delete the logger map
	delete WCLogManager::_loggerMap;
	//Make sure to set rootLogger to NULL, in case the LogManager needs to be restarted
	WCLogManager::_rootLogger = NULL;
	//Also reset the reference counter
	WCLogManager::_refCount = 0;
	//Return true
	return true;
}


WCLogger* WCLogManager::CreateLogger(const std::string &name, WCLogAppender* appender, const WCLoggerLevel &level) {
	WCLogger *newLogger;
	
	//Get a reference to the logger map
	std::map<std::string,WCLogger*> *map = WCLogManager::_loggerMap;
	//Check to make sure that the LogManager has been initialized
	if (WCLogManager::_refCount == 0) {
		std::cerr << "WCLogManager::CreateLogger Error - LogManager must be initialized first\n";
		return NULL;
	}
	//First, search to see if the name already exists
	if (map->find(name) != map->end()) {
		CLOGGER_ERROR(WCLogManager::_rootLogger, "WCLogManager::CreateLogger Error - Name already exists");
		return NULL;
	}
	//Check to see if a non-default appender was passed
	if (appender != NULL) 
		newLogger = new WCLogger(name, level, appender);
	else
		newLogger = new WCLogger(name, level, WCLogManager::_rootAppender);
	//Put the name and newLogger into the map
	(*map)[name] = newLogger;	
	//Finally, return the newLogger pointer
	return newLogger;
}


WCLogger* WCLogManager::ReferenceLogger(WCLogger* ref) {
	std::map<std::string,WCLogger*>::iterator iter;
	
	//Check to make sure that the LogManager has been initialized
	if (WCLogManager::_refCount == 0) {
		std::cerr << "WCLogManager::ReferenceLogger Error - LogManager must be initialized first\n";
		return NULL;
	}
	//Make sure that the reference is not NULL
	if (ref == NULL) {
		CLOGGER_ERROR(WCLogManager::_rootLogger, "WCLogManager::ReferenceLogger Error - Null value passed.");
		return NULL;	
	}
	//Find the logger in the map
	iter = WCLogManager::_loggerMap->begin();
	while (iter != WCLogManager::_loggerMap->end()) {
		if (iter->second == ref) {
			//Reference the logger
			ref->Reference();
			return ref;
		}
		//Move to the next record
		iter++;
	}
	//If you are here, then the logger doesn't exist
	CLOGGER_ERROR(WCLogManager::_rootLogger, "WCLogManager::ReferenceLogger Error - Logger does not exist.");
	return NULL;
}


bool WCLogManager::CheckLogger(WCLogger* ref) {
	std::map<std::string,WCLogger*>::iterator iter;

	//Check to make sure that the LogManager has been initialized
	if (WCLogManager::_refCount == 0) {
		std::cerr << "WCLogManager::CheckLogger Error - LogManager must be initialized first\n";
		return false;
	}
	//Make sure that the reference is not NULL
	if (ref == NULL) return false;	
	//Find the logger in the map
	iter = WCLogManager::_loggerMap->begin();
	while (iter != WCLogManager::_loggerMap->end()) {
		if (iter->second == ref)
			//Reference the logger
			return true;
		//Move to the next record
		iter++;
	}
	//If you are here than the logger doesn't exist
	return false;
}


void WCLogManager::DereferenceLogger(WCLogger* ref) {
	std::map<std::string,WCLogger*>::iterator iter;

	//Check to make sure that the LogManager has been initialized
	if (WCLogManager::_refCount == 0) {
		std::cerr << "WCLogManager::DereferenceLogger Error - LogManager must be initialized first\n";
		return;
	}
	//Make sure that the reference is not NULL
	if (ref == NULL) {
		CLOGGER_ERROR(WCLogManager::_rootLogger, "WCLogManager::ReferenceLogger Error - Null value passed.");
		return;	
	}
	//Find the logger in the map
	iter = WCLogManager::_loggerMap->begin();
	while (iter != WCLogManager::_loggerMap->end()) {
		if (iter->second == ref) {
			//Dereference, and if returns zero, destroy the logger
			if(ref->Dereference()) {
				CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCLogManager::DereferenceLogger - There are still references");				
				return;
			}
			//Remove the logger from the map
			WCLogManager::_loggerMap->erase(iter);
			//Time to destroy the logger
			delete ref;
			return;
		}
		//Move to the next record
		iter++;
	}
	//If you are here, then the logger doesn't exist
	CLOGGER_ERROR(WCLogManager::_rootLogger, "WCLogManager::DereferenceLogger Error - Logger does not exist.");
	return;
}


WCLogger* WCLogManager::GetLogger(const std::string &name) {
	std::map<std::string,WCLogger*>::iterator iter;

	//Get a reference to the logger map
	std::map<std::string,WCLogger*> *map = WCLogManager::_loggerMap;
	//Check to make sure that the LogManager has been initialized
	if (WCLogManager::_refCount == 0) {
		std::cerr << "WCLogManager::CreateLogger Error - LogManager must be initialized first\n";
		return NULL;
	}	
	//Find to see if there is a logger with the given name
	iter = map->find(name);
	//If the iterator is at the end, then no logger was found
	if (iter == map->end()) {
		return WCLogManager::_rootLogger;
	}
	return iter->second;
}


/***********************************************~***************************************************/

