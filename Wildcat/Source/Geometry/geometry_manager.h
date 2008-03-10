#ifndef __WGEOL_GEOMETRY_MANAGER_H__
#define __WGEOL_GEOMETRY_MANAGER_H__


/*** Included Headers ***/
#include "Geometry/wgeol.h"


/*** Locally Defined Values ***/
//None


/*** Class PreDefines ***/
//None


/***********************************************~***************************************************/


class WCGeometryManager {
private:
	static int									_refCount;											//!< Reference count for startup-terminate
private:
	WCGeometryManager();																			//!< Deny access to default constructor
	~WCGeometryManager();																			//!< Deny access to default destructor
	WCGeometryManager(const WCGeometryManager&);													//!< Deny access to copy constructor
	WCGeometryManager& operator=(const WCGeometryManager&);											//!< Deny access to equals operator
public:
	//Startup - Terminate Functions
	static bool Started(void)					{ return WCGeometryManager::_refCount; }			//!< Check to see if the manager has been started
	static bool Initialize(void);																	//!< Initialize manager
	static bool Terminate(void);																	//!< Terminate the manager
};


/***********************************************~***************************************************/


#endif //__WGEOL_GEOMETRY_MANAGER_H__

