//Inluded Header Files
#import <Cocoa/Cocoa.h>
#include "Utility/log_manager.h"
#include "Utility/shader_manager.h"


/***********************************************~***************************************************/


void InitApplication(void) {
	//Initialize logger manager
	WCLogManager::Initialize();
	//Initialize xml manager
	xercesc::XMLPlatformUtils::Initialize();
}


void ShutdownApplication(void) {
	//Terminate the managers
	xercesc::XMLPlatformUtils::Terminate();
	WCLogManager::Terminate();
}


int main(int argc, char *argv[])
{
	//Init the app
	InitApplication();

	//Create the main application instance (NSApp is global instance)
	[NSApplication sharedApplication];
	//Load the initial NIB
	[NSBundle loadNibNamed:@"MainMenu" owner:NSApp];
	//Run the main loop
	[NSApp run];

	//Shutdown the app
	ShutdownApplication();

	//Exit the application
	return 1;
}


/***********************************************~***************************************************/

