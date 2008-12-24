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


#ifndef __VIS_SIGNAL_H__
#define __VIS_SIGNAL_H__


/*** Included Header Files ***/
#include <RTVisualization/wrtv.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


//Values for signal types (Meant to mirror Matlab fundamental types)
#define SIGNALTYPE_INVALID						0
#define SIGNALTYPE_ARRAY						1
#define SIGNALTYPE_INT8							2
#define SIGNALTYPE_UINT8						3
#define SIGNALTYPE_INT16						4
#define SIGNALTYPE_UINT16						5
#define SIGNALTYPE_INT32						6
#define SIGNALTYPE_UINT32						7
#define SIGNALTYPE_INT64						8
#define SIGNALTYPE_UINT64						9
#define SIGNALTYPE_SINGLE						10
#define SIGNALTYPE_DOUBLE						11
#define SIGNALTYPE_LOGICAL						12
#define SIGNALTYPE_CHAR							13
#define SIGNALTYPE_CELL							14

class WCSignalType {
private:
	unsigned int								_type;												//!< Signal data type
	//Hidden Constructors
	WCSignalType();																					//!< Deny access to default constructor 
	WCSignalType(const unsigned int &type)		{ this->_type = type; }								//!< Hidden primary constructor
public:
	//Constructors and Destructors
	WCSignalType(const WCSignalType &type) : _type(type._type)	{ }									//!< Copy constructor
	static WCSignalType Array(void)				{ return WCSignalType(SIGNALTYPE_ARRAY); }			//!< Array data type
	static WCSignalType Int8(void)				{ return WCSignalType(SIGNALTYPE_INT8); }			//!< Int8 data type
	static WCSignalType UInt8(void)				{ return WCSignalType(SIGNALTYPE_UINT8); }			//!< UInt8 data type
	static WCSignalType Int16(void)				{ return WCSignalType(SIGNALTYPE_INT16); }			//!< Int16 data type
	static WCSignalType UInt16(void)			{ return WCSignalType(SIGNALTYPE_UINT16); }			//!< UInt16 data type
	static WCSignalType Int32(void)				{ return WCSignalType(SIGNALTYPE_INT32); }			//!< Int32 data type
	static WCSignalType UInt32(void)			{ return WCSignalType(SIGNALTYPE_UINT32); }			//!< UInt32 data type
	static WCSignalType Int64(void)				{ return WCSignalType(SIGNALTYPE_INT64); }			//!< Int64 data type
	static WCSignalType UInt64(void)			{ return WCSignalType(SIGNALTYPE_UINT64); }			//!< UInt64 data type
	static WCSignalType Single(void)			{ return WCSignalType(SIGNALTYPE_SINGLE); }			//!< Single data type
	static WCSignalType Double(void)			{ return WCSignalType(SIGNALTYPE_DOUBLE); }			//!< Double data type
	static WCSignalType Logical(void)			{ return WCSignalType(SIGNALTYPE_LOGICAL); }		//!< Logical data type
	static WCSignalType Char(void)				{ return WCSignalType(SIGNALTYPE_CHAR); }			//!< Char data type
	static WCSignalType Cell(void)				{ return WCSignalType(SIGNALTYPE_CELL); }			//!< Cell data type
	
	//Overridder Operators
	WCSignalType& operator=(const WCSignalType &type)	{ this->_type = type._type; return *this; }	//!< Data type equation				
	bool operator==(const WCSignalType &type) const		{ return this->_type == type._type; }		//!< Equals operator
	bool operator!=(const WCSignalType &type) const		{ return this->_type != type._type; }		//!< Inequality operator
};


/***********************************************~***************************************************/


class WCSignalDescriptor {
private:
	std::string									_name;												//!< Descriptor name
	WPUInt										_size;												//!< Size of the signal data
public:
	//Constructors and Destructors
	WCSignalDescriptor()						{ }													//!< Primary constructor
	~WCSignalDescriptor()						{ }													//!< Default destructor

	//Member Access Methods
	inline WPUInt Size(void) const				{ return this->_size; }								//!< Get the descriptor size
};


/***********************************************~***************************************************/


struct WSSignal {
	WPTime										receivedTime;										//!< Receievd timestamp
	WCSignalDescriptor							*descriptor;										//!< Pointer to descriptor
	void										*data;												//!< Signal data
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__VIS_SIGNAL_H__

