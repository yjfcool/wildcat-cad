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


//Imported Header Files
#import <Kernel/OSX/dialog_osx.h>


/***********************************************~***************************************************/


template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}


/***********************************************~***************************************************/


@implementation WCDialog_OSX


- (id)initWithDialog:(WCDialog*)dialog
{
	//Set the dialog
	_dialog = dialog;
	//Get full path of dialog
	std::string str = "/" + _dialog->Name() + ".html";
	NSString *cstr = [NSString stringWithCString:str.c_str() encoding:NSUTF8StringEncoding];
	NSString *path = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:cstr];
	location = location = [NSURL URLWithString:path];

	//Do the cocoa thing (need to modify this to handle borderless)
    self = [super initWithWindowNibName:@"ModalDialog"];

	//Set the windowsize
	NSRect rect = [[self window] frame];
	rect.size.width = _dialog->Width();
	rect.size.height = _dialog->Height();
	[[self window] setFrame:rect display:YES];
	//Set self as delegate for both window and webView
	[[self window] setDelegate:self];
	[webView setFrameLoadDelegate:self];
	[webView setUIDelegate:self];

	//Return the object
	return self;	
}


- (void)dealloc
{
    // Do the regular Cocoa thing.
    [super dealloc];
}


- (void)windowDidLoad
{
	//Try loading location into webview
	WebFrame *frame = [webView mainFrame];
	[frame loadRequest:[NSURLRequest requestWithURL:location]];
}


- (WebView*)WebView
{
	return webView;
}


- (void)showWindow
{
}


- (void)hideWindow
{
}


- (void)windowWillClose:(NSNotification *)notification
{
	//Let dialog know that window is closing
	_dialog->CloseWindow(true);
	//If in modal, stop it
	if (_dialog->IsModal())
		[NSApp stopModal];
}


- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
	//If window is cached, we are done
	if (_dialog->Mode() != WCDialogMode::Cached()) {
		//Page has fully loaded, now order front
		[[sender window] orderFront:self];
	}
	//Run modal if appropriate
	if (_dialog->IsModal()) [NSApp runModalForWindow:[self window]];
}


- (void)webView:(WebView *)sender didReceiveTitle:(NSString *)title forFrame:(WebFrame *)frame
{
	// Report feedback only for the main frame.
	if (frame == [sender mainFrame]){
		//Set the title of the window to the title of the main frame
		[[sender window] setTitle:title];
	}
}


- (void)webView:(WebView *)webView windowScriptObjectAvailable:(WebScriptObject *)windowScriptObject {
	//Once scripting is available, link to console object
    [windowScriptObject setValue:self forKey:@"console"];
}


- (void)alertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo {
	//Nothing to do here (just have to have method for showing alert sheets)
}


	/* sent to the WebView's ui delegate when alert() is called in JavaScript.
	If you call alert() in your JavaScript methods, it will call this
	method and display the alert message in the log.  In Safari, this method
	displays an alert that presents the message to the user.
	*/
- (void)webView:(WebView *)sender runJavaScriptAlertPanelWithMessage:(NSString *)message {
	//Create a basic informative alert
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert addButtonWithTitle:@"OK"];
	[alert setMessageText:message];
	[alert setAlertStyle:NSInformationalAlertStyle];
	//Show the alert
	[alert beginSheetModalForWindow:[self window] modalDelegate:self didEndSelector:@selector(alertDidEnd:returnCode:contextInfo:) contextInfo:nil];
}

	/* This method is called by the WebView when it is deciding what
	methods on this object can be called by JavaScript.  The method
	should return NO the methods we would like to be able to call from
	JavaScript, and YES for all of the methods that cannot be called
	from JavaScript.
	*/
+ (BOOL)isSelectorExcludedFromWebScript:(SEL)selector {
//	NSLog(@"%@ received %@ for '%@'", self, NSStringFromSelector(_cmd), NSStringFromSelector(selector));
	//Only selector allowed is sendMessage
    if (selector == @selector(sendMessage:)) return NO;
	//Otherwise, return YES to exclude
    return YES;
}


	/* This method is called by the WebView to decide what instance
	variables should be shared with JavaScript.  The method should
	return NO for all of the instance variables that should be shared
	between JavaScript and Objective-C, and YES for all others.
	*/
+ (BOOL)isKeyExcludedFromWebScript:(const char *)property {
//	NSLog(@"%@ received %@ for '%s'", self, NSStringFromSelector(_cmd), property);
//	if (strcmp(property, "sharedValue") == 0) {
//		return NO;
//	}
	return YES;
}

	/* This method converts a selector value into the name we'll be using
	to refer to it in JavaScript.  here, we are providing the following
	Objective-C to JavaScript name mappings:
		'doOutputToLog:' => 'log'
		'changeJavaScriptText:' => 'setscript'
	With these mappings in place, a JavaScript call to 'console.log' will
	call through to the doOutputToLog: Objective-C method, and a JavaScript call
	to console.setscript will call through to the changeJavaScriptText:
	Objective-C method.  
	
	Comments for the webScriptNameForSelector: method in WebScriptObject.h talk more
	about the default name conversions performed from Objective-C to JavaScript names.
	You can overrride those defaults by providing your own translations in your
	webScriptNameForSelector: method.
	*/
+ (NSString *) webScriptNameForSelector:(SEL)sel {
//	NSLog(@"%@ received %@ with sel='%@'", self, NSStringFromSelector(_cmd), NSStringFromSelector(sel));
    if (sel == @selector(sendMessage:)) return @"sendMessage";
	else return nil;
}


- (void)sendMessage:(NSString *)message {
	//Get the message
	const char* cString = [message cStringUsingEncoding:NSUTF8StringEncoding];
	std::string str(cString);
	//Dispatch the message back to the dialog class
	if (_dialog->Controller() != NULL) _dialog->Controller()->ReceiveMessage(str);
}


- (std::string)getStringFromScript:(NSString*)name
{
	//Get the script object
	id scriptObject = [webView windowScriptObject];
	NSString *value;
	@try {
		value = [scriptObject valueForKey:name];
	} @catch (NSException *e) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialog_OSX::getStringFromScript - Unknown variable: " << [name cStringUsingEncoding:NSUTF8StringEncoding]);
		value = [[NSString alloc] initWithString:@""];
	}

	//Convert value to string
	const char* cString = [value cStringUsingEncoding:NSUTF8StringEncoding];
	//Return the string value
	return std::string(cString);
}


- (WPFloat)getFloatFromScript:(NSString*)name
{
	//Get the script object
	id scriptObject = [webView windowScriptObject];
	NSString *value;
	@try {
		value = [scriptObject valueForKey:name];
	} @catch (NSException *e) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialog_OSX::getFloatFromScript - Unknown variable: " << [name cStringUsingEncoding:NSUTF8StringEncoding]);
		value = [[NSString alloc] initWithString:@"0.0"];
	}

	//Convert value to a float
	std::string strValue( [value cStringUsingEncoding:NSUTF8StringEncoding] );
	WPFloat output;
	bool retVal = from_string<WPFloat>(output, strValue, std::dec);
	//If conversion was valid, return output
	if (retVal) return output;
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialog_OSX::getFloatFromScript - Invalid float conversion.");	
	return 0.0;
}


- (WPInt)getIntFromScript:(NSString*)name
{
	//Get the script object
	id scriptObject = [webView windowScriptObject];
	NSString *value;
	@try {
		value = [scriptObject valueForKey:name];
	} @catch (NSException *e) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialog_OSX::getIntFromScript - Unknown variable: " << [name cStringUsingEncoding:NSUTF8StringEncoding]);
		value = [[NSString alloc] initWithString:@"0.0"];
	}

	//Convert newValue to an int
	std::string strValue( [value cStringUsingEncoding:NSUTF8StringEncoding] );
	WPInt output;
	bool retVal = from_string<WPInt>(output, strValue, std::dec);
	//If conversion was valid, return output
	if (retVal) return output;
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialog_OSX::getIntFromScript - Invalid float conversion.");	
	return 0;
}


- (WPUInt)getUnsignedIntFromScript:(NSString*)name
{
	//Get the script object
	id scriptObject = [webView windowScriptObject];
	NSString *value;
	@try {
		value = [scriptObject valueForKey:name];
	} @catch (NSException *e) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialog_OSX::getUnsignedIntFromScript - Unknown variable: " << [name cStringUsingEncoding:NSUTF8StringEncoding]);
		value = [[NSString alloc] initWithString:@"0.0"];
	}

	//Convert newValue to an unsigned int
	std::string strValue( [value cStringUsingEncoding:NSUTF8StringEncoding] );
	WPUInt output;
	bool retVal = from_string<WPUInt>(output, strValue, std::dec);
	//If conversion was valid, return output
	if (retVal) return output;
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialog_OSX::getUnsignedIntFromScript - Invalid float conversion.");	
	return 0;
}


- (bool)getBoolFromScript:(NSString*)name
{
	//Get the script object
	id scriptObject = [webView windowScriptObject];
	NSString *value;
	@try {
		value = [scriptObject valueForKey:name];
	} @catch (NSException *e) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialog_OSX::getUnsignedIntFromScript - Unknown variable: " << [name cStringUsingEncoding:NSUTF8StringEncoding]);
		value = [[NSString alloc] initWithString:@"0.0"];
	}

	//Convert newValue to an unsigned int
	std::string strValue( [value cStringUsingEncoding:NSUTF8StringEncoding] );
	if (strValue == "false") return false;
	else return true;
}


- (void)setStringFromScript:(std::string)var withValue:(std::string)value
{
}


- (void)setFloatFromScript:(std::string)var withValue:(WPFloat)value
{
}


- (void)setIntFromScript:(std::string)var withValue:(WPInt)value
{
}


- (void)setUnsignedIntFromScript:(std::string)var withValue:(WPUInt)value
{
}


- (void)setBoolFromScript:(std::string)var withValue:(bool)value
{
}


@end


/***********************************************~***************************************************/

