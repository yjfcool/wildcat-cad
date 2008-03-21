/*** Included Header Files ***/
#include "vector.h"
#include "matrix.h"
#include <vecLib/vBLAS.h>


/*** Class Order ***/
//---WCVector4


/***********************************************~***************************************************/


WCVector4::WCVector4() {
	this->_data[0] = 0.0;
	this->_data[1] = 0.0;
	this->_data[2] = 0.0;
	this->_data[3] = 0.0;
}


WCVector4::WCVector4(const WPFloat i, const WPFloat j, const WPFloat k, const WPFloat l) {
	this->_data[0] = i;
	this->_data[1] = j;
	this->_data[2] = k;
	this->_data[3] = l;
}


WCVector4::WCVector4(const WPUInt size, const WPFloat* data) {
	//Check to make sure non-NULL data passed
	if (data == NULL) {
		std::cout << "WCVector4::WCVector4 Error - NULL data passed.\n";
		//throw error
		return;
	}
	//Check to make sure size is not too large
	if (size > 4) {
		std::cout << "WCVector4::WCVector4 Error - Size is too large\n";
		//throw error
		return;
	}
	//Copy in values
	unsigned int i;
	for (i=0; i<size; i++) 
		this->_data[i] = data[i];
	//Zero remaining values
	for (i=size; i<4; i++)
		this->_data[i] = 0.0;
}


WCVector4::WCVector4(const WCVector4& vector) {
	this->_data[0] = vector._data[0];
	this->_data[1] = vector._data[1];
	this->_data[2] = vector._data[2];
	this->_data[3] = vector._data[3];
}


WCVector WCVector4::ToVector(void) {
	//Return a new WCVector object
	return WCVector(4, this->_data);
}

	
WCMatrix WCVector4::ToMatrix(void) {
	//Return a new CMatrix object
	return WCMatrix(1, 4, this->_data);
}


void WCVector4::ToElement(DOMElement *element) {
	std::ostringstream os;
	os.precision(8);
	//Convert i, j, k, l values to strings
	os << this->_data[0];
	std::string iStr = os.str();
	os.seekp(0); os << this->_data[1];
	std::string jStr = os.str();
	os.seekp(0); os << this->_data[2];
	std::string kStr = os.str();
	os.seekp(0); os << this->_data[3];
	std::string lStr = os.str();
	//Add four attributes to element
	element->setAttribute(XMLString::transcode("i"), XMLString::transcode(iStr.c_str()));
	element->setAttribute(XMLString::transcode("j"), XMLString::transcode(jStr.c_str()));
	element->setAttribute(XMLString::transcode("k"), XMLString::transcode(kStr.c_str()));
	element->setAttribute(XMLString::transcode("l"), XMLString::transcode(lStr.c_str()));
}


WCVector4 WCVector4::operator+(const WCVector4 &vector) const {
	WCVector4 retVec(this->_data[0] + vector._data[0], this->_data[1] + vector._data[1],
					this->_data[2] + vector._data[2], this->_data[3] + vector._data[3]);
	return retVec;
//	return cblas_daxpy(4, 1.0, this->_data, 1, vector._data, 1);	//results put into vector._data
}


WCVector4 WCVector4::operator-(const WCVector4 &vector) const {
	WCVector4 retVec(this->_data[0] - vector._data[0], this->_data[1] - vector._data[1],
					this->_data[2] - vector._data[2], this->_data[3] - vector._data[3]);
	return retVec;
//	return cblas_daxpy(4, -1.0, vector._data, 1, this->_data, 1);	//results put into this->_data
}


WCVector4 WCVector4::operator*(const WPFloat scalar) const {
	WCVector4 retVec(this->_data[0] * scalar, this->_data[1] * scalar,
					this->_data[2] * scalar, this->_data[3] * scalar);
	return retVec;
//	return cblas_dscal(4, scalar, this->_data, 1);					//results put into this->_data
}


WCVector4 WCVector4::operator/(const WPFloat scalar) const {
	WCVector4 retVec(this->_data[0] / scalar, this->_data[1] / scalar,
					this->_data[2] / scalar, this->_data[3] / scalar);
	return retVec;
//	return cblas_dscal(4, 1.0/scalar, this->_data, 1);				//results put into this->_data
}

WCMatrix4 WCVector4::operator*(const WCVector4 &vector) const {
	WCMatrix4 retMat(this->_data[0] * vector._data[0], this->_data[0] * vector._data[1],
					this->_data[0] * vector._data[2], this->_data[0] * vector._data[3],
					this->_data[1] * vector._data[0], this->_data[1] * vector._data[1],
					this->_data[1] * vector._data[2], this->_data[1] * vector._data[3],
					this->_data[2] * vector._data[0], this->_data[2] * vector._data[1],
					this->_data[2] * vector._data[2], this->_data[2] * vector._data[3],
					this->_data[3] * vector._data[0], this->_data[3] * vector._data[1],
					this->_data[3] * vector._data[2], this->_data[3] * vector._data[3]);

	return retMat;
}


WCVector4& WCVector4::operator+=(const WPFloat scalar) {
	this->_data[0] += scalar;
	this->_data[1] += scalar;
	this->_data[2] += scalar;
	this->_data[3] += scalar;
	return *this;
}


WCVector4& WCVector4::operator+=(const WCVector4 &vector) {
	this->_data[0] += vector._data[0];
	this->_data[1] += vector._data[1];
	this->_data[2] += vector._data[2];
	this->_data[3] += vector._data[3];
	return *this;
}


WCVector4& WCVector4::operator-=(const WPFloat scalar) {
	this->_data[0] -= scalar;
	this->_data[1] -= scalar;
	this->_data[2] -= scalar;
	this->_data[3] -= scalar;
	return *this;
}


WCVector4& WCVector4::operator-=(const WCVector4 &vector) {
	this->_data[0] -= vector._data[0];
	this->_data[1] -= vector._data[1];
	this->_data[2] -= vector._data[2];
	this->_data[3] -= vector._data[3];
	return *this;
}


WCVector4& WCVector4::operator*=(const WPFloat scalar) {
	this->_data[0] *= scalar;
	this->_data[1] *= scalar;
	this->_data[2] *= scalar;
	this->_data[3] *= scalar;			
	return *this;
}


WCVector4& WCVector4::operator/=(const WPFloat scalar) {
	this->_data[0] /= scalar;
	this->_data[1] /= scalar;
	this->_data[2] /= scalar;
	this->_data[3] /= scalar;			
	return *this;
}


WCVector4& WCVector4::operator=(const WCVector4 &vector) {
	memcpy(this->_data, vector._data, 4*sizeof(WPFloat));
//	this->_data[0] = vector._data[0];
//	this->_data[1] = vector._data[1];
//	this->_data[2] = vector._data[2];
//	this->_data[3] = vector._data[3];
//	return cblas_dcopy (4, vector._data, 1, this->_data, 1 );		//results put into this->_data
	return *this;
}


WCVector4& WCVector4::operator=(const WCVector &vector) {
	if (vector._size != 4) std::cout << "WCVector4::operator= Warning - Missized cast from Vector to Vector4.\n";
	//Copy over the data
	memcpy(this->_data, vector._data, 4*sizeof(WPFloat));
//	this->_data[0] = vector._data[0];
//	this->_data[1] = vector._data[1];
//	this->_data[2] = vector._data[2];
//	this->_data[3] = vector._data[3];	
	return *this;
}


bool WCVector4::operator==(const WCVector4 &vector) {
	if ((this->_data[0] == vector._data[0]) && 
		(this->_data[1] == vector._data[1]) && 
		(this->_data[2] == vector._data[2]) && 
		(this->_data[3] == vector._data[3])) return true;
	return false;
}


bool WCVector4::operator!=(const WCVector4 &vector) {
	if ((this->_data[0] != vector._data[0]) || 
		(this->_data[1] != vector._data[1]) || 
		(this->_data[2] != vector._data[2]) || 
		(this->_data[3] != vector._data[3])) return true;
	return false;
}


WCVector4 WCVector4::operator*(const WCMatrix4 &m) const {
	WCVector4 retVec(this->_data[0]*m._data[0] + this->_data[1]*m._data[4] + this->_data[2]*m._data[8] + this->_data[3]*m._data[12], 
					 this->_data[0]*m._data[1] + this->_data[1]*m._data[5] + this->_data[2]*m._data[9] + this->_data[3]*m._data[13], 
					 this->_data[0]*m._data[2] + this->_data[1]*m._data[6] + this->_data[2]*m._data[10] + this->_data[3]*m._data[14], 
					 this->_data[0]*m._data[3] + this->_data[1]*m._data[7] + this->_data[2]*m._data[11] + this->_data[3]*m._data[15]);
	return retVec;
}


WPFloat WCVector4::Distance(const WCVector4 &vec) const {
	WPFloat distance = (this->_data[0] - vec._data[0]) * (this->_data[0] - vec._data[0]) +
					   (this->_data[1] - vec._data[1]) * (this->_data[1] - vec._data[1]) +
					   (this->_data[2] - vec._data[2]) * (this->_data[2] - vec._data[2]) +
					   (this->_data[3] - vec._data[3]) * (this->_data[3] - vec._data[3]);
	return sqrt(distance);
}


WPFloat WCVector4::Magnitude(void) const {
	WPFloat retVal = this->_data[0] * this->_data[0] + this->_data[1] * this->_data[1] +
					 this->_data[2] * this->_data[2] + this->_data[3] * this->_data[3];
	return sqrt(retVal);
}

	
WCVector4& WCVector4::Normalize(const bool wIgnore) {
	WPFloat mag;
	if(wIgnore) {
		mag = sqrt(this->_data[0] * this->_data[0] + this->_data[1] * this->_data[1] + this->_data[2] * this->_data[2]);
		this->_data[0] = this->_data[0] / mag;
		this->_data[1] = this->_data[1] / mag;
		this->_data[2] = this->_data[2] / mag;
		this->_data[3] = 0.0;		
	} else {
		mag = sqrt(this->DotProduct(*this));
		this->_data[0] = this->_data[0] / mag;
		this->_data[1] = this->_data[1] / mag;
		this->_data[2] = this->_data[2] / mag;
		this->_data[3] = this->_data[3] / mag;
	}
	return *this;
}


WCVector4& WCVector4::Homogenize(void) {
	if (this->_data[3] == 1.0) return *this;
	this->_data[0] /= this->_data[3];
	this->_data[1] /= this->_data[3];
	this->_data[2] /= this->_data[3];
	this->_data[3] = 1.0;
	return *this;
}


WPFloat WCVector4::NormL2(void) const {
	WPFloat l2 = this->_data[0] * this->_data[0] + this->_data[1] * this->_data[1] + 
				 this->_data[2] * this->_data[2] + this->_data[3] * this->_data[3];
	return sqrt(l2);
//	return cblas_dnrm2(4, this->_data, 1);
}


WPFloat WCVector4::NormInf(void) const {
	WPFloat max = this->_data[0];
	if (max < this->_data[1]) max = this->_data[1];
	if (max < this->_data[2]) max = this->_data[2];
	if (max < this->_data[3]) max = this->_data[3];
	return max;
}


WPFloat WCVector4::DotProduct(const WCVector4 &vector) const {
	WPFloat retVal = this->_data[0] * vector._data[0] + this->_data[1] * vector._data[1] +
					 this->_data[2] * vector._data[2] + this->_data[3] * vector._data[3];
	return retVal;
//	return cblas_ddot(4, this->_data, 1, vector._data, 1);
}


WCVector4 WCVector4::CrossProduct(const WCVector4 &vector) const {
	WCVector4 retVec(this->_data[1] * vector._data[2] - this->_data[2] * vector._data[1], 
					this->_data[2] * vector._data[0] - this->_data[0] * vector._data[2],
					this->_data[0] * vector._data[1] - this->_data[1] * vector._data[0], 0.0);
	return retVec;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCVector4 &vec) {
	//Print out basic surface info
	out << "Vector4(" << &vec << ") ";	
	for(int i=0; i < 4; i++) std::cout << vec._data[i] << "\t";
	//Return the output stream
	return out;
}


/***********************************************~***************************************************/


WCVector::WCVector(long size) : _size(size), _data(NULL) {
	//Allocate space for the new vector
	this->_data = new WPFloat[size];
	//Copy in the contents of the vector
	memset(this->_data, 0, size * sizeof(WPFloat));
}


WCVector::WCVector(long size, WPFloat value) : _size(size), _data(NULL) {
	//Allocate space for the new vector
	this->_data = new WPFloat[size];
	//Copy in the value into the vector
	for(int i=0; i < size; i++) this->_data[i] = value;
}


WCVector::WCVector(long size, WPFloat* data) : _size(size), _data(NULL) {
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
		std::cout << "WCVector::operator+ Error - Incorrect vector size.\n";
		return WCVector(this->_size);
	}
	//Copy input vector into a new vector
	WCVector retVec(vector);
	//Call to the BLAS daxpy function (results are copied into retVec)
	cblas_daxpy (this->_size, 1.0, this->_data, 1, retVec._data, 1 );
	//Return retVec
	return retVec;
}


WCVector WCVector::operator-(const WCVector &vector) {
	//Check to make sure the sizes are equal
	if (this->_size != vector._size) {
		std::cout << "WCVector::operator- Error - Incorrect vector size.\n";
		return WCVector(this->_size);
	}
	//Copy input vector into a new vector
	WCVector retVec(*this);
	//Call to the BLAS daxpy function (results are copied into retVec)
	cblas_daxpy (this->_size, -1.0, vector._data, 1, retVec._data, 1 );
	//Return retVec
	return retVec;
}


WCVector WCVector::operator*(const WPFloat scalar) {
	//Copy this into the new vector
	WCVector retVec(*this);
	//Multiply the vector by the scalar using BLAS function
	cblas_dscal(this->_size, scalar, retVec._data, 1);
	//Return the result
	return retVec;
}


//WCVector WCVector::operator/(const WPFloat scalar) {
//}


WCMatrix WCVector::operator*(const WCVector &vector) {
	return WCMatrix(1, 1);
}


WCVector WCVector::operator*(const WCMatrix &matrix) {
	//Create the new output matrix
	WCVector c(matrix._numCol);
	//Check to make sure the matrix diminsions agree
	if (this->_size != matrix._numRow) {
		std::cout << "WCVector::operator* Error - Matrix size does not match\n";
		return c;
	}
/*** Debug ***/
	WPFloat value;
	for (int i=0; i<matrix._numCol; i++) {
		value = 0.0;
		for (int j=0; j<this->_size; j++)
			value += (this->_data[j] * matrix.Get(j, i));
		c.Set(i, value);
	}
/*** Debug ***/	
	return c;
}


WCVector& WCVector::operator=(const WCVector4 &vector) {
	//Reset the parameters for this vector
	this->_size = 4;
	delete this->_data;
	//Allocate new space for the vector data
	this->_data = new WPFloat[4];
	//Copy each element of matrix into this
	for(int i=0; i < this->_size; i++) this->_data[i] = vector._data[i];
	//Return the value of the matrix
	return *this;
}


WCVector& WCVector::operator=(const WCVector &vector) {
	//Reset the parameters for this vector
	this->_size = vector._size;
	delete this->_data;
	//Allocate new space for the vector data
	this->_data = new WPFloat[this->_size];
	//Copy each element of vector into this using BLAS function
    cblas_dcopy(this->_size, vector._data, 1, this->_data, 1 );
	//Return the value of the matrix
	return *this;
}


bool WCVector::operator==(const WCVector &vector) {
	//Check to make sure the vector diminsions agree
	if (this->_size != vector._size) {
		return false;
	}	
	//Check each element of matrix vs this, return false on the first non-match
	for(int i=0; i < this->_size; i++) if(this->_data[i] != vector._data[i]) return false;
	//Otherwise return true
	return true;
}


//WCVector& WCVector::Normalize(void) {
//	return *this;
//}


//WCVector& WCVector::Homogenize(void) {
//	return *this;
//}


WPFloat WCVector::NormL2(void) {
	//Return the L2 norm of the vector using BLAS function
	return cblas_dnrm2(this->_size, this->_data, 1);
}


WPFloat WCVector::NormInf(void) {
	//Get the index of the largest element of the vector using BLAS function
	int index = cblas_idamax(this->_size, this->_data, 1);
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
	return cblas_ddot( this->_size, this->_data, 1, vector._data, 1 );
}


void WCVector::Print(int indent) {
	for(int i=0; i < this->_size; i++)
		std::cout << this->_data[i] << "\t";
	std::cout << std::endl;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCVector &vec) {
	void* addr = (void*)&vec;
	
	//Print out basic surface info
	out << "Vector(" << addr << ") ";	
	for(int i=0; i < vec._size; i++)
		std::cout << vec._data[i] << "\t";
	//Return the output stream
	return out;
}


/***********************************************~***************************************************/

