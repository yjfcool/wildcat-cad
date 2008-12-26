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
#include <Kernel/wildcat_kernel.h>
#include <boost/program_options.hpp>
#include <Kernel/OSX/document_controller.h>


/***********************************************~***************************************************/


@interface WCAppDelegate : NSObject
{
}
- (void)applicationWillFinishLaunching:(NSNotification *)aNotification;
- (void)applicationWillTerminate:(NSNotification *)aNotification;
- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename;
@end


@implementation WCAppDelegate


- (BOOL)applicationOpenUntitledFile:(NSApplication *)app
{
	[[NSDocumentController sharedDocumentController] newDocument:self];
	return YES;
}


- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
	std::cout << "application:openFile -- " << [filename cStringUsingEncoding:NSUTF8StringEncoding];
	return YES;
}

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{
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
	//Shutdown the Wildcat Kernel
	WCWildcatKernel::Terminate();
}

@end


/***********************************************~***************************************************/


void _ParseCommandLine(int argc, char* argv[]) {
	//Command line arguments
	bool logToFile = true;
	std::string loggerLevel = "ERROR";
	std::string loggerFile = "wildcat.log";
	std::vector<std::string> modules;
	bool headless = false;
	//Parse the command line options
	boost::program_options::options_description optionsDescription("Allowed options");
	optionsDescription.add_options()
	("help", "produce help message")
	("logtofile", boost::program_options::value<bool>(&logToFile)->default_value(true), "log output to a file")
	("logging-level,l", boost::program_options::value<std::string>(&loggerLevel)->default_value("ERROR"), "set logging level")
	("logging-file,f", boost::program_options::value<std::string>(&loggerFile)->default_value("wildcat.log"), "set logging file")
	("modules,m", boost::program_options::value< std::vector<std::string> >(&modules), "load module(s)")
	("headless,h", boost::program_options::value<bool>(&headless)->default_value(false), "run in headless mode");

	//Get out the map!
	boost::program_options::variables_map variablesMap;
	//Try to parse the command line options
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, optionsDescription), variablesMap);
		boost::program_options::notify(variablesMap);
	} catch (boost::program_options::error &ex) {
		//Must have been quite some problem
		std::cerr << ex.what();
		exit(-1);
	}
	//Convert logging level
	WCLoggerLevel logLevel = WCLoggerLevel::Error();
	std::transform(loggerLevel.begin(), loggerLevel.end(), loggerLevel.begin(), tolower);
	if (variablesMap.count("logging-level")) {
		if (loggerLevel == "fatal") logLevel = WCLoggerLevel::Fatal();
		if (loggerLevel == "error") logLevel = WCLoggerLevel::Error();
		if (loggerLevel == "warn") logLevel = WCLoggerLevel::Warn();
		if (loggerLevel == "info") logLevel = WCLoggerLevel::Info();
		if (loggerLevel == "debug") logLevel = WCLoggerLevel::Debug();
	}

	//Initialize the Wildcat Kernel
	WCWildcatKernel::Initialize(logToFile, logLevel, loggerFile, headless);

	//Open the modules
	//...
}


int main(int argc, char *argv[]) {
	//Set up an auto-release pool
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	//Handle the command line
	_ParseCommandLine(argc, argv);

	//Begin setting up the App delegate
	WCAppDelegate *delegate = [[WCAppDelegate alloc] autorelease];

	//Create the main application instance (NSApp is global instance)
	NSApplication *app = [NSApplication sharedApplication];
	[app setDelegate:delegate];
	//Load the initial NIB
	[NSBundle loadNibNamed:@"MainMenu" owner:NSApp];
	//Run the main loop
	[NSApp run];

	//Clean up the autoreleaes pool
	[pool release];
	//Exit the application
	return 1;
}


/***********************************************~***************************************************/

