#ifndef __BACKGROUND_LAYER_H__
#define __BACKGROUND_LAYER_H__


/*** Included Headers ***/
#include "object.h"


/*** Local Defines ***/
#define WILDCATAPP_DEFAULT_LOGGER	WCLoggerLevel::Debug()


/*** Class Predefines ***/
class WCScene;


/***********************************************~***************************************************/


class WCWildcatApp : public WCObject {
protected:
	WCScene										*_scene;

private:
	//Deny Access
	WCWildcatApp();																					//!< Deny acces to default constructor

public:
	//Constructors and Destructors
	WCWildcatApp(int argc, char **argv);															//!< Primary constructor
	~WCWildcatApp();																				//!< Default destructor
	
	void Run(void);																					//!< Take over
	WCScene* Scene(void)						{ return this->_scene; }							//!< Yes
};



/***********************************************~***************************************************/


#endif //__USER_INTERFACE_LAYER_H__

