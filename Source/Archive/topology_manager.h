#ifndef __WTPKL_TOPOLOGY_MANAGER_H__
#define __WTPKL_TOPOLOGY_MANAGER_H__


/*** Included STL Header Files ***/
//None


/*** Included Wildcat Header Files ***/
#include "log_manager.h"
#include "memory_manager.h"
#include "thread_manager.h"
#include "wtpkl.h"


/*** Locally Defined Values ***/
#define TOPOLOGYMANAGER_LOGGER	"Wildcat::TopologySubsystem"
#define TOPOLOGYMANAGER_POOL	"Wildcat::TopologySubsystem"


/*** Class Template Order ***/
//---CTopologyManager


/*****************************************************************************/


class CTopologyManager {
private:
	static int						_refCount;								//!< Dependent manager reference count
	static CLogger					*_defaultLogger;						//!< Logger for the TopologyManager
	static CMemoryPool				*_defaultPool;							//!< MemoryPool for topology objects
	static std::vector<CModelUse*>	*_modelUses;							//!< Vector of all current models
private:
	CTopologyManager();														//!< Deny access to default constructor
	~CTopologyManager();													//!< Deny access to default destructor
	CTopologyManager(const CTopologyManager&);								//!< Deny access to copy constructor
	CTopologyManager& operator=(const CTopologyManager&);					//!< Deny access to equals operator
public:
	//Startup - Shutdown Functions
	static bool Started(void)		{ return CTopologyManager::_refCount; }	//!< Check to see if TopologyManager has been started
	static bool Initialize(CLogger *logger, CMemoryPool *pool);				//!< Initialize the TopologyManager
	static bool Shutdown(void);												//!< Shutdown the TopologyManager
	//General Access Functions
	static CLogger* Logger(void)	{ return CTopologyManager::_defaultLogger; }//!< Get the topology logger
	static CMemoryPool* Pool(void)	{ return CTopologyManager::_defaultPool; }//!< Get the topology memory pool
};

/*****************************************************************************/


#endif //__WTPKL_TOPOLOGY_MANAGER_H__

