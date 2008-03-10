#include "curve_profile.h"
#include "vector.h"
#include "ray.h"
#include "bounding_object.h"


/***********************************************~***************************************************/



void WCCurveProfile::ValidateLinearity(void) {
	this->_isLinear = false;
}


void WCCurveProfile::ValidateClosure(void) {
	//Find the beginning of the curve
	WCVector4 start(this->Evaluate(0.0));
	WCVector4 end(this->Evaluate(1.0));
	//Check to see if start and end are the same
	if (start.Distance(end) < CURVEPROFILE_CONNECTIVITY_TOLERANCE) this->_isClosed = true;
	else this->_isClosed = false;
}


/***********************************************~***************************************************/



WCCurveProfile::WCCurveProfile() : ::WCGeometricCurve(), _isLinear(true) {
	//Nothing else to do for now
	this->_bounds = new WCAlignedBoundingBox();
}


WCCurveProfile::WCCurveProfile(const WCCurveProfile &profile) : ::WCGeometricCurve(), 
	_curves(profile._curves), _parametricDirection(profile._parametricDirection), _isLinear(profile._isLinear) {
	//Retain all new curves
	for (int i=0; i<this->_curves.Size(); ++i)
		this->_curves.At(i)->Retain(*this);
}


WCCurveProfile::~WCCurveProfile() {
	//Releaes all sub-curves
	for (int i=0; i<this->_curves.Size(); ++i)
		this->_curves.At(i)->Release(*this);
	//Clear the collections
	this->_curves.Clear();
	this->_parametricDirection.Clear();
}


WCAlignedBoundingBox WCCurveProfile::BoundingBox(void) {
	//Check visibility
	if (!this->_isVisible) return WCAlignedBoundingBox();
	//Add contributions for all geometric objects
	for (int i=0; i<this->_curves.Size(); ++i)
		*(this->_bounds) += this->_curves.At(i)->BoundingBox();
	//Return the box
	return *this->_bounds;
}


bool WCCurveProfile::Intersect(const WCGeometricPoint &point, const WPFloat tolerance) {
	//Check to see if any sub-curves intersect the point
	for (int i=0; i<this->_curves.Size(); ++i)
//		if (this->_curves.At(i)->Intersect(point, tolerance)) return true;
	return false;
}


bool WCCurveProfile::Intersect(const WCGeometricCurve &curve, const WPFloat tolerance) {
	return false;
}


WPFloat WCCurveProfile::Length(WPFloat tolerance) {
	WPFloat length = 0.0;
	//Get length for all sub-curves
	for (int i=0; i<this->_curves.Size(); ++i)
		length += this->_curves.At(i)->Length();
	return length;
}


WCVector4 WCCurveProfile::Evaluate(WPFloat u) {
	//Make sure there are sub-curves
	if (this->_curves.Size() == 0) return WCVector4();
	//Clamp u from 0 -> 1
	u = std::min(std::max(u, 0.0), 1.0);
	//Figure out which curve u lies on
	WPFloat spot = u * this->_curves.Size();
	WPUInt id;
	if (u == 1.0) id = this->_curves.Size() - 1;
	else id = floor(spot);
	//Based on curve direction - evaluate it
	if (this->_parametricDirection.At(id))
		return this->_curves.At(id)->Evaluate(spot - id);
	else
		return this->_curves.At(id)->Evaluate(1.0 - spot + id);
}


WCVector4 WCCurveProfile::Derivative(WPFloat u, const WPUInt der) {
	//Make sure there are sub-curves
	if (this->_curves.Size() == 0) return WCVector4();
	//Clamp u from 0 -> 1
	u = std::min(std::max(u, 0.0), 1.0);
	//Figure out which curve u lies on
	WPFloat spot = u * this->_curves.Size();
	WPUInt id = floor(spot);
	//Based on curve direction - evaluate it
	if (this->_parametricDirection.At(id))
		return this->_curves.At(id)->Derivative(spot - id, der);
	else
		return this->_curves.At(id)->Derivative(1.0 - spot + id, der);
}


WCRay WCCurveProfile::Tangent(WPFloat u) {
	//Make sure there are sub-curves
	if (this->_curves.Size() == 0) return WCRay(WCVector4(), WCVector4());
	//Clamp u from 0 -> 1
	u = std::min(std::max(u, 0.0), 1.0);
	//Figure out which curve u lies on
	WPFloat spot = u * this->_curves.Size();
	WPUInt id = floor(spot);
	//Based on curve direction - evaluate it
	if (this->_parametricDirection.At(id))
		return this->_curves.At(id)->Tangent(spot - id);
	else
		return this->_curves.At(id)->Tangent(1.0 - spot + id);
}


WCVector4 WCCurveProfile::PointInversion(const WCGeometricPoint &point) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCCurveProfile::PointInversion - Not yet implemented.");
	return WCVector4();
}


WCVisualObject* WCCurveProfile::HitTest(const WCRay &ray, const WPFloat tolerance) {
	WCVisualObject* ptr;
	//Check hit on all sub-curves
	for(int i=0; i<this->_curves.Size(); i++) {
		//Check to see if it is a hit
		ptr = this->_curves.At(i)->HitTest(ray, tolerance);
		if (ptr != NULL) return ptr;
	}
	return NULL;
}


void WCCurveProfile::ApplyTransform(const WCMatrix4 &transform) {
	//Apply transform to all sub-curves
	for (int i=0; i<this->_curves.Size(); i++)
		this->_curves.At(i)->ApplyTransform(transform);
	//Mark the profile as dirty
	this->_isVisualDirty = true;
	//Update dependents
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);	
}


void WCCurveProfile::ApplyTranslation(const WCVector4 &translation) {
	//Apply translation to all sub-curves
	for (int i=0; i<this->_curves.Size(); i++)
		this->_curves.At(i)->ApplyTranslation(translation);
	//Mark the profile as dirty
	this->_isVisualDirty = true;
	//Update dependents
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCCurveProfile::Render(const GLuint defaultProg, const WCColor color) {
	//Check for visibility flag
	if (!this->_isVisible) return;
	//Render all sub-curves
	for (int i=0; i<this->_curves.Size(); i++)
		this->_curves.At(i)->Render(defaultProg, color);
	//Mark as clean
	this->_isVisualDirty = false;
}


void WCCurveProfile::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark the profile as dirty
	this->_isVisualDirty = true;
	//Update dependents
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}
	

#ifdef __WILDCAT_SCENE_INCLUDED__
void WCCurveProfile::Layer(WCLayer *layer) {
	//Make sure layer is not NULL
	if (layer == NULL) { CLOGGER_WARN(WCLogManager::RootLogger(), "WCCurveProfile::Layer - Passed layer is null."); return; }
	//Set the layer locally and for all subcurves
	this->_layer = layer;
	for (int i=0; i<this->_curves.Size(); ++i) this->_curves.At(i)->Layer(layer);
}
#endif


bool WCCurveProfile::AddCurve(WCGeometricCurve *curve) {
	//Make sure the passed curve is non-null
	if (curve == NULL) { CLOGGER_ERROR(WCLogManager::RootLogger(), "WCCurveProfile::AddCurve - Passed curve is null."); return false; }
	//Check to make sure curve is not already in profile
	for (int i=0; i<this->_curves.Size(); i++) {
		if (curve == this->_curves.At(i)) {
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCCurveProfile::AddCurve - This curve already exists in the profile.");
			return false;
		}
	}
	//Ensure connectivity and direction
	switch (this->_curves.Size()) {
		//For now curve direction and profile direction align	
		case 0: this->_parametricDirection.Add(true); break;
		//Must check to make sure there is connectivity and that directions are set properly
		case 1: {
			WCVector4 firstZero( this->_curves.At(0)->Evaluate(0.0) );
			WCVector4 firstOne( this->_curves.At(0)->Evaluate(1.0) );
			WCVector4 secondZero( curve->Evaluate(0.0) );
			WCVector4 secondOne( curve->Evaluate(1.0) );
			//Check all cases
			if (firstZero.Distance(secondZero) < CURVEPROFILE_CONNECTIVITY_TOLERANCE) { 
				//Reverse the zero curve
				this->_parametricDirection.Remove(0); 
				this->_parametricDirection.Add(false);
				//Second curve is aligned
				this->_parametricDirection.Add(true);
			}
			else if (firstZero.Distance(secondOne) < CURVEPROFILE_CONNECTIVITY_TOLERANCE) {
				//Reverse the zero curve
				this->_parametricDirection.Remove(0); 
				this->_parametricDirection.Add(false);
				//Reverse the second curve
				this->_parametricDirection.Add(false);
			}
			else if (firstOne.Distance(secondZero) < CURVEPROFILE_CONNECTIVITY_TOLERANCE) {
				//Just add second curve aligned
				this->_parametricDirection.Add(true);
			}
			else if (firstOne.Distance(secondOne) < CURVEPROFILE_CONNECTIVITY_TOLERANCE) {
				//Just add second curve reversed
				this->_parametricDirection.Add(false);				
			}
			else {
				CLOGGER_WARN(WCLogManager::RootLogger(), "WCCurveProfile::AddCurve - The new curve is not connected to profile.");
				return false;
			}
			} break;
		//Must check new curve connectedness and direction to last curve
		default:
			//Get the very end of the existing curve profile
			WCVector4 lastEnd;
			if (this->_parametricDirection.At(this->_curves.Size()-1)) lastEnd = this->_curves.At(this->_curves.Size()-1)->Evaluate(1.0);
			else lastEnd = this->_curves.At(this->_curves.Size()-1)->Evaluate(0.0);
			//See if this matches either end of the new curve
			WCVector4 newZero( curve->Evaluate(0.0) );
			WCVector4 newOne( curve->Evaluate(1.0) );
			if (lastEnd.Distance(newZero) < CURVEPROFILE_CONNECTIVITY_TOLERANCE) {
				this->_parametricDirection.Add(true);
			}
			else if (lastEnd.Distance(newOne) < CURVEPROFILE_CONNECTIVITY_TOLERANCE) {
				this->_parametricDirection.Add(false);
			}
			else {
				CLOGGER_ERROR(WCLogManager::RootLogger(), "WCCurveProfile::AddCurve - The new curve is not connected to profile.");
				return false;			
			}
			break;
	}
#ifdef __WILDCAT_SCENE_INCLUDED__
	//Make sure layer has been set for the profile
	if (this->_layer == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCCurveProfile::AddCurve - Layer must be set for profile before adding curves.");
		this->_parametricDirection.Remove(this->_parametricDirection.Size()-1);
		return false; 
	}
	//Set the curve's layer
	curve->Layer(this->_layer);
#endif
	//Add the curve into the collection
	this->_curves.Add(curve);
	//Retain the curve
	curve->Retain(*this);
	//Perform validity checks
	this->ValidateLinearity();
	this->ValidateClosure();
	//Mark the profile as dirty
	this->_isVisualDirty = true;
	//Update dependents
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
	//Return successful
	return true;
}


bool WCCurveProfile::RemoveCurve(WCGeometricCurve *curve) {
	//Make sure the passed curve is non-null
	if (curve == NULL) { CLOGGER_ERROR(WCLogManager::RootLogger(), "WCCurveProfile::RemoveCurve - Passed curve is null."); return false; }
	//Only remove the first or last curves
	if ((curve != this->_curves.At(0)) && (curve != this->_curves.At(this->_curves.Size()-1))) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCCurveProfile::RemoveCurve - Only able to remove first or last curve in profile.");
		return false; 		
	}
	//Remove the curve
	if (curve == this->_curves.At(0)) {
		this->_curves.Remove(0);
		this->_parametricDirection.Remove(0);
	}
	else {
		this->_curves.Remove(this->_curves.Size()-1);
		this->_parametricDirection.Remove(this->_curves.Size()-1);
	}
	//Release the curve
	curve->Release(*this);
	//Perform validity checks
	this->ValidateLinearity();
	this->ValidateClosure();	
	//Mark the profile as dirty
	this->_isVisualDirty = true;
	//Update dependents
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
	//Return success
	return true;
}
	

WCCurveProfile& WCCurveProfile::operator=(const WCCurveProfile &profile) {
	int i;
	//Release all current curves
	for (i=0; i<this->_curves.Size(); i++)
		this->_curves.At(i)->Release(*this);
	//Clear the existing collection a
	this->_curves.Clear();
	//Now add all of the other profile's curves
	this->_curves = profile._curves;
	//Retain all new curves
	for (i=0; i<this->_curves.Size(); i++)
		this->_curves.At(i)->Retain(*this);
	
	//Return this object
	return *this;
}


bool WCCurveProfile::operator==(const WCCurveProfile &profile) {
	return false;
}


DOMElement* WCCurveProfile::Serialize(DOMDocument *document) {
	//Create the base element for the object
	DOMElement*  profileElem = document->createElement(XMLString::transcode("CurveProfile"));
	return profileElem;
}


WCCurveProfile* WCCurveProfile::Deserialize(DOMElement* obj) {
	return NULL;
}


bool WCCurveProfile::Validate(DOMElement* obj) {
	return false;
}



/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCCurveProfile &profile) {
	//Print out some info
	out << "CurveProfile(" << &profile << ")\n";
	out << "\t Closeure: " << profile._isClosed << std::endl;
	out << "\t Linearity: " << profile._isLinear << std::endl;	
	//Loop through all curves and print information
	for (int i=0; i<profile._curves.Size(); i++) {
		out << "\t(";
		if (profile._parametricDirection.At(i)) out << "F) ";
		else out << "R) ";
		out << profile._curves.At(i) << std::endl;
	}
	//Pass out along
	return out;
}


/***********************************************~***************************************************/


