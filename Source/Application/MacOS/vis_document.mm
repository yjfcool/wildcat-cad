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


/*** Imported Header Files ***/
#import "Application/MacOS/vis_document.h"
#import "Application/MacOS/render_window.h"


/*** Included Header Files ***/
#include "RTVisualization/visualization.h"
#include "RTVisualization/vis_feature.h"
#include "Application/keymap.h"


/***********************************************~***************************************************/


@implementation WCVisDocument


- (id)init
{
	//Do the cocoa thing
    self = [super init];
	_visualization = NULL;
	_filePath = NULL;
	//Return reference to self
    return self;
}


- (id)initWithContentsOfURL:(NSURL*)absoluteURL ofType:(NSString*)typeName error:(NSError**)outError
{
	//Do the cocoa thing
    self = [super init];
	//Get the specified path for the file
	NSString* path = [absoluteURL path];
	//Set filePath as a flag for loading (see onInitOpenGL)
	_filePath = new std::string( [path cStringUsingEncoding:NSUTF8StringEncoding] );
	//Return reference to self
    return self;
}


- (void)close
{
	//Call to the super
	[super close];
	//Delete the visualization
	delete _visualization;
}


- (void)makeWindowControllers {
	//Get the render window up and running
	[super makeWindowControllers];
	//Call onInitOpenGL
	[self onInitOpenGL];
	//Show the window (and make key)
	[[_renderWindow window] makeKeyAndOrderFront:_renderWindow];
}


- (BOOL)writeToURL:(NSURL*)absoluteURL ofType:(NSString*)typeName error:(NSError**)outError
{
	//Get the specified path for saving the file to
	NSString* path = [absoluteURL path];
	std::string fullPath( [path cStringUsingEncoding:NSUTF8StringEncoding] );
	//Call to the root document to save
	_visualization->ActiveWorkbench()->SaveAs(fullPath);
	//Assume is successful
    return YES;
}


- (BOOL)isDocumentEdited
{
	//Need to actually check _part to see
	return NO;
}


- (void)onInitOpenGL
{
	//Call to the parent document
	[super onInitOpenGL];
	//See what _filePath is
	if (_filePath == NULL) {
		//Create the visualization
		_visualization = new WCVisualization("", "");
		//Set the super document's document pointer
		[super document:_visualization];
//		[super setFileName:@""];
//		[[_renderWindow window] setTitle:@"Test me"];
		//Set up default surface renderer
//		WCPartFeature::DefaultSurfaceRenderer = _part->Scene()->ShaderManager()->ProgramFromName("scn_basiclight");
		//Try resize
		[super onResize:[[_renderWindow renderView] frame].size];
	}
	//Or need to restore from file
	else {
		//Create xml parser
		xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
		//Set validation scheme
		parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);    
		xercesc::ErrorHandler* errHandler = (xercesc::ErrorHandler*) new xercesc::HandlerBase();
		parser->setErrorHandler(errHandler);

		xercesc::DOMElement *rootElement;
		xercesc::DOMNodeList *elementList;
		XMLCh* xmlString;

		//Try to parse
		try {
			//Parse the manifest
			parser->parse( _filePath->c_str() );
			//Get the root document node (Wildcat)
			rootElement = parser->getDocument()->getDocumentElement();
			//Get list of Part nodes
			xmlString = xercesc::XMLString::transcode("Visualization");
			elementList = rootElement->getElementsByTagName(xmlString);
			xercesc::XMLString::release(&xmlString);

			//Create SerialDictionary
			WCSerialDictionary *dictionary = new WCSerialDictionary();
			//Restore part from first Visualization element, and pass dictionary
			_visualization = new WCVisualization((xercesc::DOMElement*)(elementList->item(0)), dictionary);
		}
		//Error checking
		catch (const xercesc::XMLException& toCatch) {
			char* message = xercesc::XMLString::transcode(toCatch.getMessage());
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisDocument::onInitOpenGL - Exception message is: \n" << message);
			xercesc::XMLString::release(&message);
		}
		catch (const xercesc::DOMException& toCatch) {
			char* message = xercesc::XMLString::transcode(toCatch.msg);
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisDocument::onInitOpenGL - Exception message is: \n" << message);
			xercesc::XMLString::release(&message);
		}
		catch (...) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisDocument::onInitOpenGL - Unexpected Exception");
		}
		//Clean up
		delete parser;
		delete errHandler;

		//Set the super document's document pointer
		[super document:_visualization];
		//Set up default surface renderer
//		WCPartFeature::DefaultSurfaceRenderer = _part->Scene()->ShaderManager()->ProgramFromName("scn_basiclight");
		//Try resize
		[super onResize:[[_renderWindow renderView] frame].size];
	}
}


- (bool)onKeyPress:(NSEvent*)theEvent
{	
	//Try to let super handle it
	if ([super onKeyPress:theEvent]) return true;

	//Otherwise, get event key & modifiers
	NSUInteger modifiers = [theEvent modifierFlags];
	bool control = modifiers & NSControlKeyMask;
	bool option = modifiers & NSAlternateKeyMask;
	bool alt = modifiers & NSAlternateKeyMask;
	bool command = modifiers & NSCommandKeyMask;
	bool esc = false;
	NSString *keys = [theEvent charactersIgnoringModifiers];

	//Are there any that we want to handle here
	//Nothing for now...

	//Create the key event
	WCKeyEvent event([keys characterAtIndex:0], control, option, alt, command, esc);
	WCUserMessage message = _visualization->ActiveWorkbench()->KeyMap()->MessageFromKeyEvent( event );
	//If no message return false
	if (message == "") return false;
	
	//Pass the message to the part
	_visualization->ActiveWorkbench()->OnUserMessage(message);
	//Check to see if is dirty
	if (_visualization->IsVisualDirty()) [self onDisplay];
	//Return true
	return true;
}


- (void)onWindowWillClose
{
}


- (NSString*)view:(NSView *)view stringForToolTip:(NSToolTipTag)tag point:(NSPoint)point userData:(void *)userData
{
	return @"";
}



- (WCVisualization*)visualization
{
	//Return the associated Wildcat Visualization
	return _visualization;
}


@end


/***********************************************~***************************************************/


