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


/*** Inluded Header Files ***/
#import <Cocoa/Cocoa.h>
#include "Utility/log_manager.h"
#include "Application/dialog_manager.h"
#include "Application/MacOS/document_controller.h"


/***********************************************~***************************************************/


void InitApplication(void) {
	//Initialize logger manager
	WCLogManager::Initialize();
	//Initialize xml manager
	xercesc::XMLPlatformUtils::Initialize();
}


void ShutdownApplication(void) {
	//Terminate the managers
	WCDialogManager::Terminate();
	xercesc::XMLPlatformUtils::Terminate();
	WCLogManager::Terminate();
}


/***********************************************~***************************************************/


@interface WCAppDelegate : NSObject
{
}
- (void)applicationWillFinishLaunching:(NSNotification *)aNotification;
- (void)applicationWillTerminate:(NSNotification *)aNotification;
@end


@implementation WCAppDelegate


- (BOOL)applicationOpenUntitledFile:(NSApplication *)app
{
	[[NSDocumentController sharedDocumentController] newDocument:self];
	return YES;
}


- (void)applicationWillFinishLaunching:(NSNotification *)aNotification {
	//Initialize the dialog manager (must be here because needs Bundle to be active for resource directory path)
	WCDialogManager::Initialize("dialog_manifest.xml", false);
	//Create the default document controller
	WCDocumentController *controller = [[WCDocumentController alloc] init];
	//Make sure a valid controller was created
	if (!controller) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCAppDelegate::applicationWillFinishLaunching - Not able to create Document Controller.");
		//throw error
		exit(-1);
	}
}


- (void)applicationWillTerminate:(NSNotification *)aNotification
{
	//Do nothing for now...
}

@end


/***********************************************~***************************************************/


int main(int argc, char *argv[])
{
	//Init the app
	InitApplication();

	//Begin setting up the App delegate
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	WCAppDelegate *delegate = [[WCAppDelegate alloc] autorelease];

	//Create the main application instance (NSApp is global instance)
	NSApplication *app = [NSApplication sharedApplication];
	[app setDelegate:delegate];
	//Load the initial NIB
	[NSBundle loadNibNamed:@"MainMenu" owner:NSApp];
	//Run the main loop
	[NSApp run];

	//Shutdown the application
	ShutdownApplication();
	//Clean up the autoreleaes pool
	[pool release];

	//Exit the application
	return 1;
}


/***********************************************~***************************************************/

