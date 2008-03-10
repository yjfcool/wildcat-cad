#ifndef __ICON_H__
#define __ICON_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCIcon {
protected:
	std::string									_name;												//!< Icon file name
	GLuint										_texture;											//!< Icon texture ID
	WPUInt										_width, _height;									//!< Icon width and height
	
	WCIcon();
public:
	//Constructor and Destructor
	WCIcon(std::string name);
	~WCIcon();
	
	//Member Access Methods
	inline GLuint Texture(void) const			{ return this->_texture; }							//!< Get the icon texture
	inline WPUInt Width(void) const				{ return this->_width; }							//!< Get the icon width
	inline WPUInt Height(void) const			{ return this->_height; }							//!< Get the icon height
};


/***********************************************~***************************************************/


#endif //__ICON_H__

