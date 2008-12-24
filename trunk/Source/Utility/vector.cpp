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
#include <Utility/vector.h>
#include <Utility/matrix.h>
#include <Utility/serializeable_object.h>
#include <Utility/assert_exception.h>


/*** Platform Included Header Files ***/
#ifdef __APPLE__
#include <vecLib/vBLAS.h>
#include <vecLib/vDSP.h>
//Make sure to use fast path
#define __GOT_BLAS__
#endif

#ifdef __WIN32__
//...
//Undefine this for now
#undef __GOT_BLAS__
#endif


/***********************************************~***************************************************/


WCVector4::WCVector4() {
	//Set all elements to zero
	memset(this->_data.d, 0, 4 * sizeof(WPFloat));
}


WCVector4::WCVector4(const WPFloat &i, const WPFloat &j, const WPFloat &k, const WPFloat &l) {
	this->_data.d[0] = i;
	this->_data.d[1] = j;
	this->_data.d[2] = k;
	this->_data.d[3] = l;
}


WCVector4::WCVector4(const WPFloat &value) {
	this->_data.d[0] = value;
	this->_data.d[1] = value;
	this->_data.d[2] = value;
	this->_data.d[3] = value;
}


WCVector4::WCVector4(const WPFloat *data) {
	//Check to make sure data exists
	ASSERT(data);
	//Copy the data
	memcpy(this->_data.d, data, 4*sizeof(WPFloat));
}


WCVector4::WCVector4(const WPUInt &size, const WPFloat *data) {
	//Couple of quick checks
	ASSERT(data);
	ASSERT(size <= 4);
	//Zero all values
	memset(this->_data.d, 0, 4 * sizeof(WPFloat));
	//Copy in values
	memcpy(this->_data.d, data, size * sizeof(WPFloat));
}


WCVector4::WCVector4(const WCVector4 &vector) {
	//Copy the data from the other vector
	memcpy(this->_data.d, vector._data.d, 4 * sizeof(WPFloat));
}


WCVector WCVector4::ToVector(void) {
	//Return a new WCVector object
	return WCVector(4, this->_data.d);
}

	
WCMatrix WCVector4::ToMatrix(void) {
	//Return a new CMatrix object
	return WCMatrix(1, 4, this->_data.d);
}


xercesc::DOMElement* WCVector4::ToElement(xercesc::DOMNode *parent, const std::string &name) {
	//Make sure there is a parent
	ASSERT(parent);
	//Name the node
	XMLCh* xmlString = xmlString = xercesc::XMLString::transcode(name.c_str());
	//Create the node in the document
	xercesc::DOMElement* child = parent->getOwnerDocument()->createElement(xmlString);
	//Make sure to release the name
	xercesc::XMLString::release(&xmlString);

	//Add i element
	WCSerializeableObject::AddFloatAttrib(child, "i", this->_data.d[0]);
	//Add j element
	WCSerializeableObject::AddFloatAttrib(child, "j", this->_data.d[1]);
	//Add k element
	WCSerializeableObject::AddFloatAttrib(child, "k", this->_data.d[2]);
	//Add l element
	WCSerializeableObject::AddFloatAttrib(child, "l", this->_data.d[3]);

	//Append child to parent
	parent->appendChild(child);
	//Return the new child
	return child;
}


void WCVector4::FromElement(xercesc::DOMElement *element) {
	//Make sure there is an element
	ASSERT(element);
	//Get i element
	this->_data.d[0] = WCSerializeableObject::GetFloatAttrib(element, "i");
	//Get j element
	this->_data.d[1] = WCSerializeableObject::GetFloatAttrib(element, "j");
	//Get k element
	this->_data.d[2] = WCSerializeableObject::GetFloatAttrib(element, "k");
	//Get l element
	this->_data.d[3] = WCSerializeableObject::GetFloatAttrib(element, "l");
}


WCVector4 WCVector4::operator+(const WCVector4 &vector) const {
	WCVector4 result;
//Apple specific optimization
#if defined(__APPLE__) && defined(__SSE2__)
	result._data.v[0] = this->_data.v[0] + vector._data.v[0];
	result._data.v[1] = this->_data.v[1] + vector._data.v[1];
//Default path
#else
	result._data.d[0] = this->_data.d[0] + vector._data.d[0];
	result._data.d[1] = this->_data.d[1] + vector._data.d[1];
	result._data.d[2] = this->_data.d[2] + vector._data.d[2];
	result._data.d[3] = this->_data.d[3] + vector._data.d[3];
#endif
	return result;
}


WCVector4 WCVector4::operator-(const WCVector4 &vector) const {
	WCVector4 result;
//Apple specific optimization
#if defined(__APPLE__) && defined(__SSE2__)
	result._data.v[0] = this->_data.v[0] - vector._data.v[0];
	result._data.v[1] = this->_data.v[1] - vector._data.v[1];
//Default path
#else
	result._data.d[0] = this->_data.d[0] - vector._data.d[0];
	result._data.d[1] = this->_data.d[1] - vector._data.d[1];
	result._data.d[2] = this->_data.d[2] - vector._data.d[2];
	result._data.d[3] = this->_data.d[3] - vector._data.d[3];
#endif
	return result;
}


WCVector4 WCVector4::operator*(const WPFloat &scalar) const {
	WCVector4 result;
//Apple specific optimization
#if defined(__APPLE__) && defined(__SSE2__)
	vDouble S = { scalar, scalar };
	result._data.v[0] = this->_data.v[0] * S;
	result._data.v[1] = this->_data.v[1] * S;
	return result;
//Default path
#else
	result._data.d[0] = this->_data.d[0] * scalar;
	result._data.d[1] = this->_data.d[1] * scalar;
	result._data.d[2] = this->_data.d[2] * scalar;
	result._data.d[3] = this->_data.d[3] * scalar;
#endif
	return result;
}


WCVector4 WCVector4::operator/(const WPFloat &scalar) const {
	WCVector4 result;
	WPFloat recip = 1.0 / scalar;
//Apple specific optimization
#if defined(__APPLE__) && defined(__SSE2__)
	vDouble R = { recip, recip };
	result._data.v[0] = this->_data.v[0] * R;
	result._data.v[1] = this->_data.v[1] * R;
	return result;
//Default path
#else
	result._data.d[0] = this->_data.d[0] * recip;
	result._data.d[1] = this->_data.d[1] * recip;
	result._data.d[2] = this->_data.d[2] * recip;
	result._data.d[3] = this->_data.d[3] * recip;
#endif
	return result;
}


WCMatrix4 WCVector4::operator*(const WCVector4 &vector) const {
	WCMatrix4 retMat(this->_data.d[0] * vector._data.d[0], this->_data.d[0] * vector._data.d[1],
					this->_data.d[0] * vector._data.d[2], this->_data.d[0] * vector._data.d[3],
					this->_data.d[1] * vector._data.d[0], this->_data.d[1] * vector._data.d[1],
					this->_data.d[1] * vector._data.d[2], this->_data.d[1] * vector._data.d[3],
					this->_data.d[2] * vector._data.d[0], this->_data.d[2] * vector._data.d[1],
					this->_data.d[2] * vector._data.d[2], this->_data.d[2] * vector._data.d[3],
					this->_data.d[3] * vector._data.d[0], this->_data.d[3] * vector._data.d[1],
					this->_data.d[3] * vector._data.d[2], this->_data.d[3] * vector._data.d[3]);

	return retMat;
}


WCVector4& WCVector4::operator+=(const WPFloat &scalar) {
//Apple specific optimization
#if defined(__APPLE__) && defined(__SSE2__)
	vDouble S = { scalar, scalar };
	this->_data.v[0] += S;
	this->_data.v[1] += S;
//Default path
#else
	this->_data.d[0] += scalar;
	this->_data.d[1] += scalar;
	this->_data.d[2] += scalar;
	this->_data.d[3] += scalar;
#endif
	return *this;
}


WCVector4& WCVector4::operator+=(const WCVector4 &vector) {
//Apple specific optimization
#if defined(__APPLE__) && defined(__SSE2__)
	this->_data.v[0] += vector._data.v[0];
	this->_data.v[1] += vector._data.v[1];
//Default path
#else
	this->_data.d[0] += vector._data.d[0];
	this->_data.d[1] += vector._data.d[1];
	this->_data.d[2] += vector._data.d[2];
	this->_data.d[3] += vector._data.d[3];
#endif
	return *this;
}


WCVector4& WCVector4::operator-=(const WPFloat &scalar) {
//Apple specific optimization
#if defined(__APPLE__) && defined(__SSE2__)
	vDouble S = { scalar, scalar };
	this->_data.v[0] -= S;
	this->_data.v[1] -= S;
//Default path
#else
	this->_data.d[0] -= scalar;
	this->_data.d[1] -= scalar;
	this->_data.d[2] -= scalar;
	this->_data.d[3] -= scalar;
#endif
	return *this;
}


WCVector4& WCVector4::operator-=(const WCVector4 &vector) {
//Apple specific optimization
#if defined(__APPLE__) && defined(__SSE2__)
	this->_data.v[0] -= vector._data.v[0];
	this->_data.v[1] -= vector._data.v[1];
//Default path
#else
	this->_data.d[0] -= vector._data.d[0];
	this->_data.d[1] -= vector._data.d[1];
	this->_data.d[2] -= vector._data.d[2];
	this->_data.d[3] -= vector._data.d[3];
#endif
	return *this;
}


WCVector4& WCVector4::operator*=(const WPFloat &scalar) {
//Apple specific optimization
#if defined(__APPLE__) && defined(__SSE2__)
	vDouble S = { scalar, scalar };
	this->_data.v[0] *= S;
	this->_data.v[1] *= S;
//Default path
#else
	this->_data.d[0] *= scalar;
	this->_data.d[1] *= scalar;
	this->_data.d[2] *= scalar;
	this->_data.d[3] *= scalar;
#endif
	return *this;
}


WCVector4& WCVector4::operator/=(const WPFloat &scalar) {
	this->_data.d[0] /= scalar;
	this->_data.d[1] /= scalar;
	this->_data.d[2] /= scalar;
	this->_data.d[3] /= scalar;			
	return *this;
}


WCVector4& WCVector4::operator=(const WCVector4 &vector) {
	memcpy(this->_data.d, vector._data.d, sizeof(WPFloat) * 4);
	return *this;
}


WCVector4& WCVector4::operator=(const WCVector &vector) {
	//Make sure vector size is always 4
	ASSERT(vector._size == 4);
	//Copy over the data
	memcpy(this->_data.d, vector._data, 4 * sizeof(WPFloat));
	return *this;
}


bool WCVector4::operator==(const WCVector4 &vector) {
	return (memcmp(this->_data.d, vector._data.d, sizeof(WPFloat) * 4) == 0);
}


bool WCVector4::operator!=(const WCVector4 &vector) {
	return (memcmp(this->_data.d, vector._data.d, sizeof(WPFloat) * 4) != 0);
}


WCVector4 WCVector4::operator*(const WCMatrix4 &m) const {
	WCVector4 retVec(this->_data.d[0]*m._data[0] + this->_data.d[1]*m._data[4] + this->_data.d[2]*m._data[8] + this->_data.d[3]*m._data[12], 
					 this->_data.d[0]*m._data[1] + this->_data.d[1]*m._data[5] + this->_data.d[2]*m._data[9] + this->_data.d[3]*m._data[13], 
					 this->_data.d[0]*m._data[2] + this->_data.d[1]*m._data[6] + this->_data.d[2]*m._data[10] + this->_data.d[3]*m._data[14], 
					 this->_data.d[0]*m._data[3] + this->_data.d[1]*m._data[7] + this->_data.d[2]*m._data[11] + this->_data.d[3]*m._data[15]);
	return retVec;
}


WPFloat WCVector4::Distance(const WCVector4 &vec) const {
	WPFloat distance = (this->_data.d[0] - vec._data.d[0]) * (this->_data.d[0] - vec._data.d[0]) +
					   (this->_data.d[1] - vec._data.d[1]) * (this->_data.d[1] - vec._data.d[1]) +
					   (this->_data.d[2] - vec._data.d[2]) * (this->_data.d[2] - vec._data.d[2]) +
					   (this->_data.d[3] - vec._data.d[3]) * (this->_data.d[3] - vec._data.d[3]);
	return sqrt(distance);
}


WPFloat WCVector4::Magnitude(void) const {
	WPFloat retVal = this->_data.d[0] * this->_data.d[0] + this->_data.d[1] * this->_data.d[1] +
					 this->_data.d[2] * this->_data.d[2] + this->_data.d[3] * this->_data.d[3];
	return sqrt(retVal);
}

	
WCVector4& WCVector4::Normalize(const bool &wIgnore) {
	WPFloat mag;
	if(wIgnore) {
		mag = sqrt(this->_data.d[0] * this->_data.d[0] + this->_data.d[1] * this->_data.d[1] + this->_data.d[2] * this->_data.d[2]);
		//Make sure not to div by zero
		if (mag != 0.0) {
			this->_data.d[0] = this->_data.d[0] / mag;
			this->_data.d[1] = this->_data.d[1] / mag;
			this->_data.d[2] = this->_data.d[2] / mag;
		}
		this->_data.d[3] = 0.0;		
	} else {
		mag = sqrt(this->DotProduct(*this));
		//Make sure not to div by zero
		if (mag == 0.0) return *this;
		this->_data.d[0] = this->_data.d[0] / mag;
		this->_data.d[1] = this->_data.d[1] / mag;
		this->_data.d[2] = this->_data.d[2] / mag;
		this->_data.d[3] = this->_data.d[3] / mag;
	}
	return *this;
}


WCVector4& WCVector4::Homogenize(void) {
	if (this->_data.d[3] == 1.0) return *this;
	this->_data.d[0] /= this->_data.d[3];
	this->_data.d[1] /= this->_data.d[3];
	this->_data.d[2] /= this->_data.d[3];
	this->_data.d[3] = 1.0;
	return *this;
}


WPFloat WCVector4::NormL2(void) const {
	WPFloat l2 = this->_data.d[0] * this->_data.d[0] + this->_data.d[1] * this->_data.d[1] + 
				 this->_data.d[2] * this->_data.d[2] + this->_data.d[3] * this->_data.d[3];
	return sqrt(l2);
}


WPFloat WCVector4::NormInf(void) const {
	WPFloat max = this->_data.d[0];
	if (max < this->_data.d[1]) max = this->_data.d[1];
	if (max < this->_data.d[2]) max = this->_data.d[2];
	if (max < this->_data.d[3]) max = this->_data.d[3];
	return max;
}


WPFloat WCVector4::DotProduct(const WCVector4 &vector) const {
	WPFloat retVal( this->_data.d[0] * vector._data.d[0] + this->_data.d[1] * vector._data.d[1] +
					this->_data.d[2] * vector._data.d[2] + this->_data.d[3] * vector._data.d[3] );
	return retVal;
}


WCVector4 WCVector4::CrossProduct(const WCVector4 &vector) const {
	WCVector4 retVec( this->_data.d[1] * vector._data.d[2] - this->_data.d[2] * vector._data.d[1], 
					this->_data.d[2] * vector._data.d[0] - this->_data.d[0] * vector._data.d[2],
					this->_data.d[0] * vector._data.d[1] - this->_data.d[1] * vector._data.d[0], 0.0);
	return retVec;
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCVector4 &vec) {
	//Print out basic surface info
	out << "Vector4(" << &vec << ") ";	
	for(int i=0; i < 4; i++) std::cout << vec._data.d[i] << "\t";
	//Return the output stream
	return out;
}


/***********************************************~***************************************************/


WCVector::WCVector(const WPUInt &size) : _size(size), _data(NULL) {
	//Allocate space for the new vector
	this->_data = new WPFloat[size];
	//Copy in the contents of the vector
	memset(this->_data, 0, size * sizeof(WPFloat));
}


WCVector::WCVector(const WPUInt &size, const WPFloat &value) : _size(size), _data(NULL) {
	//Allocate space for the new vector
	this->_data = new WPFloat[size];
	//Copy in the value into the vector
	for(WPUInt i=0; i < size; i++) this->_data[i] = value;
}


WCVector::WCVector(const WPUInt &size, WPFloat *data) : _size(size), _data(NULL) {
	//Allocate space for the new vector
	this->_data = new WPFloat[size];
	//Copy in the contents of the vector
	memcpy(this->_data, data, size * sizeof(WPFloat));
}


WCVector::WCVector(const WCVector& vector) : _size(vector._size), _data(NULL) {
	//Allocate space for the new vector
	this->_data = new WPFloat[this->_size];
	//Copy in the contents of the vector
	memcpy(this->_data, vector._data, this->_size * sizeof(WPFloat));
}


WCVector::~WCVector() {
	//Make sure to erase the allocated space
	delete this->_data;
}


WCVector4 WCVector::ToVector4(void) {
	//Check to make sure the size is 4
	if (this->_size !=4) {
		std::cout << "WCVector::ToVector4 Error - Incorrect vector size.\n";
		return WCVector4();
	}
	//Otherwise, return a new WCVector4 object;
	return WCVector4(4, this->_data);
}


WCMatrix WCVector::ToMatrix(void) {
	//Create a new matrix from this vector (remember, it will be vertically oriented)
	return WCMatrix(this->_size, 1, this->_data);
}



WCVector WCVector::operator+(const WCVector &vector) {
	//Check to make sure the sizes are equal
	if (this->_size != vector._size) {
		std::cout << "WCVector::operator+ with Vector Error - Incorrect vector size.\n";
		return WCVector(this->_size);
	}
	//Copy input vector into a new vector
	WCVector retVec(vector);
//BLAS specific optimization
#ifdef __GOT_BLAS__
	//Call to the BLAS daxpy function (results are copied into retVec)
	cblas_daxpy (this->_size, 1.0, this->_data, 1, retVec._data, 1 );
//Non-BLAS path
#else
	//Loop through elements and add
	for (WPUInt i=0; i < this->_size; i++)
		retVec.Set(i, this->_data[i] + vector._data[i]);
#endif
	//Return retVec
	return retVec;
}


WCVector WCVector::operator-(const WCVector &vector) {
	//Check to make sure the sizes are equal
	if (this->_size != vector._size) {
		std::cout << "WCVector::operator- with Vector Error - Incorrect vector size.\n";
		return WCVector(this->_size);
	}
	//Copy input vector into a new vector
	WCVector retVec(*this);
//BLAS specific optimization
#ifdef __GOT_BLAS__
	//Call to the BLAS daxpy function (results are copied into retVec)
	cblas_daxpy (this->_size, -1.0, vector._data, 1, retVec._data, 1 );
//Non-BLAS path
#else
	//Loop through elements and subtract
	for (WPUInt i=0; i < this->_size; i++)
		retVec.Set(i, this->_data[i] - vector._data[i]);
#endif
	//Return retVec
	return retVec;
}


WCVector WCVector::operator*(const WPFloat &scalar) {
	//Copy this into the new vector
	WCVector retVec(*this);
//BLAS specific optimization
#ifdef __GOT_BLAS__
	//Multiply the vector by the scalar using BLAS function
	cblas_dscal(this->_size, scalar, retVec._data, 1);
//Non-BLAS path
#else
	//Loop through elements and multiply
	for (WPUInt i=0; i < this->_size; i++)
		retVec.Set(i, this->_data[i] * scalar);
#endif
	//Return the result
	return retVec;
}


WCVector WCVector::operator/(const WPFloat &scalar) {
	//Copy this into the new vector
	WCVector retVec(*this);
	WPFloat recip = 1.0 / scalar;
//BLAS specific optimization
#ifdef __GOT_BLAS__
	//Multiply the vector by the recip using BLAS function
	cblas_dscal(this->_size, recip, retVec._data, 1);
//Non-BLAS path
#else
	//Loop through elements and multiply by inverse
	for (WPUInt i=0; i < this->_size; i++)
		retVec.Set(i, this->_data[i] * recip);
#endif
	//Return the result
	return retVec;
}


WCMatrix WCVector::operator*(const WCVector &vector) {
	//Check to make sure the sizes are equal
	if (this->_size != vector._size) {
		std::cout << "WCVector::operator* with Vector Error - Incorrect vector size.\n";
		return WCMatrix(1, 1);
	}
	//Setup output matrix
	WCMatrix retMat(this->_size, this->_size);
	//Double loop to calculate values
	for (WPUInt i=0; i<this->_size; i++)
		for (WPUInt j=0; j<this->_size; j++)
			retMat.Set(i, j, this->_data[i] * vector._data[j]);
	//Return the loaded matrix
	return retMat;
}


WCVector WCVector::operator*(const WCMatrix &matrix) {
	//Create the new output matrix
	WCVector c(matrix._numCol);
	//Check to make sure the matrix diminsions agree
	if (this->_size != matrix._numRow) {
		std::cout << "WCVector::operator* Error - Matrix size does not match\n";
		return c;
	}
	//Loop and calculate
	WPFloat value;
	for (WPUInt i=0; i<matrix._numCol; i++) {
		value = 0.0;
		for (WPUInt j=0; j<this->_size; j++)
			value += (this->_data[j] * matrix.Get(j, i));
		c.Set(i, value);
	}
	//Return the vector
	return c;
}


WCVector& WCVector::operator=(const WCVector4 &vector) {
	//Reset the parameters for this vector (if needed)
	if (this->_size != 4) {
		this->_size = 4;
		delete this->_data;
		//Allocate new space for the vector data
		this->_data = new WPFloat[4];
	}
	//Copy each element of vector into this
	memcpy(this->_data, vector._data.d, 4 * sizeof(WPFloat));
	//Return the value of the matrix
	return *this;
}


WCVector& WCVector::operator=(const WCVector &vector) {
	//Reset the parameters for this vector (if needed)
	if (this->_size != vector._size) {
		this->_size = vector._size;
		delete this->_data;
		//Allocate new space for the vector data
		this->_data = new WPFloat[this->_size];
	}
	//Copy the memory contents
	memcpy(this->_data, vector._data, this->_size * sizeof(WPFloat));
	//Return the value of the matrix
	return *this;
}


bool WCVector::operator==(const WCVector &vector) {
	//Check to make sure the vector diminsions agree
	if (this->_size != vector._size) {
		return false;
	}	
	//Check each element of matrix vs this, return false on the first non-match
	for(WPUInt i=0; i < this->_size; i++) if(this->_data[i] != vector._data[i]) return false;
	//Otherwise return true
	return true;
}


WCVector& WCVector::Normalize(void) {
	//Get the magnitude
	WPFloat mag = 1.0 / this->NormL2();
	//Multiply by magnitude inverse
	for (WPUInt i=0; i < this->_size; i++)
		this->_data[i] = this->_data[i] * mag;
	//Return ref to this
	return *this;
}


WPFloat WCVector::NormL2(void) {
//BLAS specific optimization
#ifdef __GOT_BLAS__
	//Return the L2 norm of the vector using BLAS function
	return cblas_dnrm2(this->_size, this->_data, 1);
//Non-BLAS path
#else
	WPFloat retVal = 0.0;
	//Loop and calculate
	for (WPUInt i=0; i<this->_size; i++)
		retVal += (this->_data[i] * this->_data[i]);
	//Take the square root
	retVal = sqrt(retVal);
	//Return the value
	return retVal;
#endif
}


WPFloat WCVector::NormInf(void) {
//BLAS specific optimization
#ifdef __GOT_BLAS__
	//Get the index of the largest element of the vector using BLAS function
	int index = cblas_idamax(this->_size, this->_data, 1);
//Non-BLAS path
#else
//	WPFloat 
	std::cout << "WCVector::NormInf - need to get blas alternative\n";
	int index = 0;
#endif
	//Return the absolute value of it
	if (this->_data[index] > 0.0) return this->_data[index];
	else return -(this->_data[index]);
}


WPFloat WCVector::DotProduct(const WCVector &vector) {
	//Check to make sure the vector diminsions agree
	if (this->_size != vector._size) {
		std::cout << "WCVector::DotProduct Error - Invalid vector size.\n";
		return 0.0;
	}
//BLAS specific optimization
#ifdef __GOT_BLAS__
	return cblas_ddot( this->_size, this->_data, 1, vector._data, 1 );
//Non-BLAS path
#else
	WPFloat retVal = 0.0;
	//Loop through and calculate
	for (WPUInt i=0; i<this->_size; i++)
		retVal += this->_data[i] * vector._data[i];
	//Return the total
	return retVal;
#endif
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCVector &vec) {
	void* addr = (void*)&vec;
	
	//Print out basic surface info
	out << "Vector(" << addr << ") ";	
	for(WPUInt i=0; i < vec._size; i++)
		std::cout << vec._data[i] << "\t";
	//Return the output stream
	return out;
}


/***********************************************~***************************************************/

