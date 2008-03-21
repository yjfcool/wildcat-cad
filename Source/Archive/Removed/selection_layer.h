#ifndef __SELECTION_LAYER_H__
#define __SELECTION_LAYER_H__


/*** Included Header Files ***/
#include "wscnl.h"
#include "layer.h"


/*** Local Defines ***/
#define SELECTIONLAYER_EPSILON					0.01
#define SELECTIONLAYER_FACTOR					2
#define SELECTIONLAYER_PATTERN					0x6666


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCSelectionLayer : public WCLayer {
protected:
	WPFloat										_xMark, _yMark;										//!< Location of mouse down event

private:
	WCSelectionLayer();																				//!< Deny access to default constructor
	WCSelectionLayer(const WCSelectionLayer &layer);												//!< Deny access to copy constructor
	WCSelectionLayer& operator=(const WCSelectionLayer &layer);										//!< Deny access to equals operator
	
public:
	//Constructors and Destructors
	WCSelectionLayer(WCScene *scene);																//!< Primary constructor
	~WCSelectionLayer();																			//!< Default destructor

	//Inherited Methods
	bool OnMouseMove(const WPFloat x, const WPFloat y);												//!< Track the mouse x and y position
	bool OnMouseDown(WCMouseButton button);															//!< Mouse button down event
	bool OnMouseUp(WCMouseButton button);															//!< Mouse button up event
	
	//Required Methods
	void Render(WCRenderState *state)			{ }													//!< No selection layer rendering
};


/***********************************************~***************************************************/


#endif //__SELECTION_LAYER_H__

