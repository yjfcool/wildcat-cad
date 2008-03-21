/*** Included Header Files ***/
#include "selection_layer.h"


/***********************************************~***************************************************/


WCSelectionLayer::WCSelectionLayer(WCScene *scene) : ::WCLayer(scene, "SelectionLayer"),
	_xMark(0.0), _yMark(0.0) {
	//Set some layer parameters
	this->_isShadowPass = false;
	this->_isSelectionPass = false;
	this->_isVisible = false;
}


WCSelectionLayer::~WCSelectionLayer() {
}


bool WCSelectionLayer::OnMouseMove(const WPFloat x, const WPFloat y) {
	if (this->_isVisible) {
		this->MarkDirty();
		//Build selection list here
		//...
		return true;
	}
	return false;
}


bool WCSelectionLayer::OnMouseDown(WCMouseButton button) {
	std::cout << "Selecting!!!\n";
	//Mark the layer as active
	this->_isVisible = true;
	//Gather current information
	this->_xMark = this->_scene->MouseX();
	this->_yMark = this->_scene->MouseY();
	//Set this layer as FirstResponder
	this->_scene->FirstResponder(this);
	return true;
}


bool WCSelectionLayer::OnMouseUp(WCMouseButton button) {
	//No longer marked or first responder
	this->_isVisible = false;
	this->MarkDirty();
	this->_scene->FirstResponder(NULL);
	//Return true
	return true;
}


/*
void WCSelectionLayer::Render(WCRenderState *state) {
	//Only render if visible
	if (!this->_isVisible) return;
	//Set the program to zero
	glUseProgram(0);
	//Calculate box corners
	GLfloat xMin = std::min(this->_xMark, this->_scene->MouseX());
	GLfloat xMax = std::max(this->_xMark, this->_scene->MouseX());
	GLfloat yMin = std::min(this->_yMark, this->_scene->MouseY());
	GLfloat yMax = std::max(this->_yMark, this->_scene->MouseY());
	//Setup billboarding
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	//Draw the selection box
	this->_color.Enable();
	//Enable blending
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(SELECTIONLAYER_FACTOR, SELECTIONLAYER_PATTERN);
	glBegin(GL_LINE_STRIP);
		//Lower left corner
		glVertex2f(xMin, yMin);
		//Upper left corner
		glVertex2f(xMin, yMax);
		//Upper right corner
		glVertex2f(xMax, yMax);
		//Lower right corner
		glVertex2f(xMax, yMin);
		//Lower left corner
		glVertex2f(xMin, yMin);		
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//Restore the modelview matrix
	glPopMatrix();
	//Restore the program
//	glUseProgram(state->Program());
	//Check for errors
	if (glGetError() != GL_NO_ERROR) std::cout << "WCSelectionLayer::Render Error - Unspecified Errors.\n";
}
*/

/***********************************************~***************************************************/


