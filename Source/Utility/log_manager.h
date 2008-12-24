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


#ifndef __WUTIL_LOG_MANAGER_H__
#define __WUTIL_LOG_MANAGER_H__


/*** Included Wildcat Header Files ***/
#include <Utility/wutil.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class PreDefines ***/
class WCLogAppender;
class WCLogger;


/***********************************************~***************************************************/


/*** The CStringBuffer class is very useful in the logger.  It allows two easy functions
   * to be used.  Users can input data to the logger using << operators (via the std::basic_ostream)
   * and can get std::string output from the std::stringbuf class.
   */
class WCStringBuffer : public std::basic_ostream<char> {
private:
	std::stringbuf								_buffer;											//!< Internal string buffer for *.str() output capabilities
public:
	WCStringBuffer() : std::basic_ostream<char>(&_buffer) {	this->init(&(this->_buffer)); }			//!< Constructor initializes the base ostream
	inline std::basic_string<char> str(void) const	{ return this->_buffer.str(); }					//!< Return the contents of the string buffer
	inline void str(std::basic_string<char> str)	{ this->_buffer.str(str); }						//!< Set the contents of the string buffer
};


/***********************************************~***************************************************/


void _LogCheck(int level, WCLogger* logger, std::string msg, int line, std::string file);


/***********************************************~***************************************************/


//Primary definition of the logging macros (when logging is enabled at compile time)
#ifdef __WILDCAT_DEBUG_LOGGER__
#define CLOGGER_DEBUG(logger, msg) { WCStringBuffer os; os << msg; __WILDCAT_NAMESPACE__::_LogCheck(LOGGER_DEBUG, logger, os.str(), __LINE__, __FILE__); }
#define CLOGGER_INFO(logger, msg)  { WCStringBuffer os; os << msg; __WILDCAT_NAMESPACE__::_LogCheck(LOGGER_INFO, logger, os.str(), __LINE__, __FILE__); }
#define CLOGGER_WARN(logger, msg)  { WCStringBuffer os; os << msg; __WILDCAT_NAMESPACE__::_LogCheck(LOGGER_WARN, logger, os.str(), __LINE__, __FILE__); }
#define CLOGGER_ERROR(logger, msg) { WCStringBuffer os; os << msg; __WILDCAT_NAMESPACE__::_LogCheck(LOGGER_ERROR, logger, os.str(), __LINE__, __FILE__); }
#define CLOGGER_FATAL(logger, msg) { WCStringBuffer os; os << msg; __WILDCAT_NAMESPACE__::_LogCheck(LOGGER_FATAL, logger, os.str(), __LINE__, __FILE__); }
#endif

#ifdef __WILDCAT_LOGGER__
#define CLOGGER_DEBUG(logger, msg) { \
		if (logger->IsDebugEnabled()) { WCStringBuffer os; os << msg; \
			logger->ForcedLog(LOGGER_DEBUG, os.str(), __LINE__, __FILE__); }}
#define CLOGGER_INFO(logger, msg) { \
		if (logger->IsInfoEnabled()) { WCStringBuffer os; os << msg; \
			logger->ForcedLog(LOGGER_INFO, os.str(), __LINE__, __FILE__); }}
#define CLOGGER_WARN(logger, msg) { \
		if (logger->IsWarnEnabled()) { WCStringBuffer os; os << msg; \
			logger->ForcedLog(LOGGER_WARN, os.str(), __LINE__, __FILE__); }}
#define CLOGGER_ERROR(logger, msg) { \
		if (logger->IsErrorEnabled()) { WCStringBuffer os; os << msg; \
			logger->ForcedLog(LOGGER_ERROR, os.str(), __LINE__, __FILE__); }}
#define CLOGGER_FATAL(logger, msg) { \
		if (logger->IsFatalEnabled()) { WCStringBuffer os; os << msg; \
			logger->ForcedLog(LOGGER_FATAL, os.str(), __LINE__, __FILE__); }}
#endif

/*
//Alternate definition for logging functions if all logging is turned off
#ifndef __WILDCAT_LOGGER__
#define CLOGGER_DEBUG(logger, msg)	//
#define CLOGGER_INFO(logger, msg)	//
#define CLOGGER_WARN(logger, msg)	//
#define CLOGGER_ERROR(logger, msg)	//
#define CLOGGER_FATAL(logger, msg)	//
#endif
*/

/***********************************************~***************************************************/


class WCLoggerLevel {
private:
	int											_level;												//!< Int variable for the level of the logger
	WCLoggerLevel();																				//!< Deny access to default constructor
	WCLoggerLevel(int level) : _level(level)	{}													//!< Private constructor - must use the static methods
	friend class WCLogger;
public:
	//Constructors and Destructors
	WCLoggerLevel(const WCLoggerLevel &level) : _level(level._level) { }							//!< Copy constructor
	//Static Public Constructors
	static WCLoggerLevel Debug(void)			{ return WCLoggerLevel(LOGGER_DEBUG); }				//!< Create a Debug logger level
	static WCLoggerLevel Info(void)				{ return WCLoggerLevel(LOGGER_INFO); }				//!< Create a Info logger level
	static WCLoggerLevel Warn(void)				{ return WCLoggerLevel(LOGGER_WARN); }				//!< Create a Warn logger level
	static WCLoggerLevel Error(void)			{ return WCLoggerLevel(LOGGER_ERROR); }				//!< Create a Error logger level
	static WCLoggerLevel Fatal(void)			{ return WCLoggerLevel(LOGGER_FATAL); }				//!< Create a Fatal logger level
	static WCLoggerLevel None(void)				{ return WCLoggerLevel(LOGGER_NONE); }				//!< Create a No-Logging logger level
	//Overridden Operators
	WCLoggerLevel& operator=(const WCLoggerLevel &level) { this->_level = level._level; return *this; }	//!< Equals operator
	bool operator==(const WCLoggerLevel &level)	{ return this->_level == level._level; }				//!< Equality operator
	bool operator!=(const WCLoggerLevel &level)	{ return this->_level != level._level; }				//!< Inequality operator
	
};


/***********************************************~***************************************************/


class WCLogger {
private:
	int											_refCount;											//!< Reference counter for the logger
	std::string									_name;												//!< Name of the logger
	WCLoggerLevel								_level;												//!< Logging output level of the logger
	WCLogAppender								*_appender;											//!< The current appender for the logger
private:
	WCLogger();																						//!< Deny access to default constructor
	~WCLogger();																					//!< Private destructor (must use WCLogManager::Terminate() )
	WCLogger(const WCLogger&);																		//!< Deny access to copy constructor
	WCLogger& operator=(const WCLogger&);															//!< Deny access to equals operator
	WCLogger(const std::string &name, const WCLoggerLevel &level, WCLogAppender* appender);			//!< Private constructor (must use WCLogManager::CreateLogger() )
	int Reference(void)							{ return ++this->_refCount; }						//!< Increment the reference counter
	int Dereference(void)						{ return --this->_refCount; }						//!< Decrement the reference counter
	int GetRefCount(void)						{ return this->_refCount; }							//!< Return the reference count for the logger
	std::string GetName(void)					{ return this->_name; }								//!< Return the name of the logger
	friend class WCLogManager;																		//!< Let WCLogManager be a friend
public:
	//General use functions
	void Level(const WCLoggerLevel &level)		{ this->_level = level; }							//!< Function to set the level of the logger
	void Appender(WCLogAppender* appender=NULL);													//!< Set the log appender

	//Functions used by the logging macros
	inline bool IsDebugEnabled(void)			{ return (this->_level._level <= LOGGER_DEBUG); }	//!< Quick check to see if debug is enabled
	inline bool IsInfoEnabled(void)				{ return (this->_level._level <= LOGGER_INFO); }	//!< Quick check to see if info is enabled
	inline bool IsWarnEnabled(void)				{ return (this->_level._level <= LOGGER_WARN); }	//!< Quick check to see if warn is enabled
	inline bool IsErrorEnabled(void)			{ return (this->_level._level <= LOGGER_ERROR); }	//!< Quick check to see if error is enabled
	inline bool IsFatalEnabled(void)			{ return (this->_level._level <= LOGGER_FATAL); }	//!< Quick check to see if fatal is enabled
	void ForcedLog(const int &level, const std::string &msg, const int &line, const std::string &file);	//!< Primary function used to send data to the logger
};


/***********************************************~***************************************************/


class WCLogManager {
private:
	static int									_refCount;											//!< Reference count for startup-Terminate
	static WCLogger								*_rootLogger;										//!< Static pointer to a singularity logger
	static WCLogAppender						*_rootAppender;										//!< Static pointer to a singularity stdout appender
	static std::map<std::string,WCLogger*>		*_loggerMap;										//!< Table for the root logger to keep track of other loggers
private:
	WCLogManager();																					//!< Deny access to default constructor
	~WCLogManager();																				//!< Deny access to default destructor
	WCLogManager(const WCLogManager&);																//!< Deny access to copy constructor
	WCLogManager& operator=(const WCLogManager&);													//!< Deny access to equals operator
public:
	//Startup - Terminate Functions
	static bool Started(void)					{ return WCLogManager::_refCount > 0; }				//!< Check to see if the LogManager has been started
	static bool Initialize(const WCLoggerLevel &level, WCLogAppender *defaultAppender=NULL);		//!< Initialize the logger 
	static bool Terminate(void);																	//!< Terminate the logger
	//General Access Functions
	static WCLogger* RootLogger(void)			{ return WCLogManager::_rootLogger; }				//!< Get the global root logger
	static WCLogAppender* RootAppender(void)	{ return WCLogManager::_rootAppender; }				//!< Get the global log appender
	static WCLogger* CreateLogger(const std::string &name, WCLogAppender* appender, const WCLoggerLevel &level);	//!< Create a new logger
	static WCLogger* ReferenceLogger(WCLogger* ref);												//!< Add a reference to the logger
	static bool WCLogManager::CheckLogger(WCLogger* ref);											//!< Check to make sure the logger is a valid logger
	static void DereferenceLogger(WCLogger* ref);													//!< Remove a reference or destroy the logger (if zero)
	static WCLogger* GetLogger(const std::string &name);											//!< Get the logger named Name
};


/***********************************************~***************************************************/


/*** To-Do List ***/
// - Logger not thread safe (see ::RootLogger)


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__WUTIL_LOG_MANAGER_H__

