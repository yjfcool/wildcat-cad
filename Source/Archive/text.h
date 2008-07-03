#ifndef __TEXT_H__
#define __TEXT_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCText_OSX;


/***********************************************~***************************************************/


//Pre-Defined Font Types
#define TEXTSTYLE_ROMAN							1
#define TEXTSTYLE_BOLD							2
#define TEXTSTYLE_ITALIC						3

class WCTextStyle {
private:
	unsigned int								_style;												//!< Text style
	//Deny Access
	WCTextStyle();																					//!< Deny access to default constructor
	WCTextStyle(unsigned int style) : _style(style) { }												//!< Primary hidden constructor
	friend class WCText;
public:
	~WCTextStyle()								{ }													//!< Default destructor
	static WCTextStyle Roman(void)				{ return WCTextStyle(TEXTSTYLE_ROMAN); }			//!< Roman constructor
	static WCTextStyle Bold(void)				{ return WCTextStyle(TEXTSTYLE_BOLD); }				//!< Bold constructor
	static WCTextStyle Italic(void)				{ return WCTextStyle(TEXTSTYLE_ITALIC); }			//!< Italic constructor
};


/***********************************************~***************************************************/


//Pre-Defined Font Types
#define TEXTFONT_TIMES							1
#define TEXTFONT_PALATINO						2
#define TEXTFONT_HELVETICA						3
#define TEXTFONT_TAHOMA							4

class WCTextFont {
private:
	unsigned int								_type;												//!< Font type
	//Deny Access
	WCTextFont();																					//!< Deny access to default constructor
	WCTextFont(unsigned int type) : _type(type)	{ }													//!< Primary hidden constructor
	friend class WCText;
public:
	~WCTextFont()								{ }													//!< Default destructor
	static WCTextFont Helvetica(void)			{ return WCTextFont(TEXTFONT_HELVETICA); }			//!< Helvetica constructor
	static WCTextFont Palatino(void)			{ return WCTextFont(TEXTFONT_PALATINO); }			//!< Palatino constructor
	static WCTextFont Tahoma(void)				{ return WCTextFont(TEXTFONT_TAHOMA); }				//!< Helvetica constructor
	static WCTextFont Times(void)				{ return WCTextFont(TEXTFONT_TIMES); }				//!< Times constructor

};


/***********************************************~***************************************************/


class WCText {
private:
	WCText_OSX									*_baseText;											//!< Base text (platform specific) object
	
	//Deny Access
	WCText();																						//!< Deny access to default constructor
public:
	//Constructors and Destructors
	WCText(WCScene *scene, const std::string &text, const WCColor &color, const WCTextFont &font,	//!< Primary constructor
												const WCTextStyle &style, const float fontSize);
	~WCText();																						//!< Default destructor

	//General Access Methods
	GLfloat Width(void);																			//!< Get the width of the text
	GLfloat Height(void);																			//!< Get the height of the text
	std::string Text(void);																			//!< Get the text string
	void Text(const std::string &text);																//!< Set the text string
	WCColor Color(void);																			//!< Get the text color
	void Color(const WCColor &color);																//!< Set the text color
	float FontSize(void);																			//!< Get the text font size
	void FontSize(const float size);																//!< Set the text font size
	bool IsUnderlined(void);																		//!< Get the text underlined state
	void IsUnderlined(const bool &state);															//!< Set the text underlined state

	//Render Methods
	void DrawAtPoint(const GLfloat x, const GLfloat y);												//!< Draw the text at a specific 2D point
	void DrawAtPoint(const WCVector4 &pt, const WCVector4 &uUnit=WCVector4(1.0, 0.0, 0.0, 0.0),		//!< Draw the text at a specific 3D point
										  const WCVector4 &vUnit=WCVector4(0.0, 1.0, 0.0, 0.0));
};



/***********************************************~***************************************************/


#endif //__TEXT_H__

