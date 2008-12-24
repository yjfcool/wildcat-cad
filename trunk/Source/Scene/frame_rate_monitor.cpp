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


/*** Included Header Files ***/
#include <Scene/frame_rate_monitor.h>
#include <Scene/user_interface_layer.h>
#include <Scene/text.h>


/*** Platform Included Header Files ***/
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif
#ifdef __WIN32__
//...
#endif


/*************************************************************/

WCFrameRateMonitor::WCFrameRateMonitor(WCUserInterfaceLayer *layer) : ::WCOverlay(layer, false),
	_frameCounter(0), _start(), _fps(0.0), _color(FRAMERATEMONITOR_DEFAULT_COLOR), _label(NULL) {
	//Set the size of the FRM
	this->WCWidget::SetSize(FRAMERATEMONITOR_WIDTH, FRAMERATEMONITOR_HEIGHT);
	//Register the FRM
	layer->RegisterWidget(this);
	//Install as scene frametick function
	this->_layer->Scene()->FrameTick(this);
	//Initialize label
	this->_label = new WCText(this->_layer->Scene(), "0.00 FPS", this->_color,
							  WCTextFont::Palatino(), WCTextStyle::Roman(), 20.0);	
}


WCFrameRateMonitor::~WCFrameRateMonitor() {
	//Unregister the widget
	this->_layer->UnregisterWidget(this);
	//Delete label if needed
	if (this->_label != NULL) delete this->_label;
}


void WCFrameRateMonitor::operator()(void) {
	//Start the timer if counter = 0
	if (this->_frameCounter == 0)
#ifdef __APPLE__
		//Use OSX frameworks to get time
		this->_start = UpTime();
#elif __WIN32__
		//Use MFC frameworks to get time
		this->_start = GetTickCount();
#endif
	//Calculate FPS every 100 frames
	if (this->_frameCounter == 100) {
#ifdef __APPLE__
		Nanoseconds elapsed = AbsoluteDeltaToNanoseconds(UpTime(), this->_start);
		this->_fps = 100.0 / UnsignedWideToUInt64(elapsed) * 1000000000;
#elif __WIN32__
		//Determine how much time has elapsed since start of 100 frame block
		WPFloat elapsed = (WPFloat)(GetTickCount() - this->_start);
		this->_fps = 100.0 / elapsed * 1000.0;
#endif
		if (this->_fps > 1000.0) this->_fps = -1.0f;
		//Reset the counter to 0
		this->_frameCounter = 0;
		//Make sure the layer/scene know to redraw
		this->MarkDirty();
	}
	//Otherwise increment the counter
	else this->_frameCounter++;
}



void WCFrameRateMonitor::OnReshape(void) {
	this->WCOverlay::OnReshape();
}


void WCFrameRateMonitor::Render(void) {
	//Generate the string
	char str[12];
	if (this->_fps >= 0.0)
		sprintf(str, "%4.2f fps", this->_fps);
	else
		sprintf(str, ">1000 fps");
		
	//Check to make sure the compass is visible
	if (this->_isVisible) {
		this->_label->Text(str);
		this->_label->DrawAtPoint((GLfloat)(this->_xMin + FRAMERATEMONITOR_BORDER),
								  (GLfloat)(this->_yMin + FRAMERATEMONITOR_BORDER));
	}
	else {
		if (this->_frameCounter == 99) std::cout << "FRM: " << str << std::endl;
	}
}


/*************************************************************/


