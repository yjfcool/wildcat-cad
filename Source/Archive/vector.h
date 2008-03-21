#ifndef __WUTIL_VECTOR_H__
#define __WUTIL_VECTOR_H__


/*** Included Header Files ***/
#include "wutil.h"


/*** Locally Defined Values ***/
//None

/*** Class Order ***/
//---WCVector4


/***********************************************~***************************************************/


class WCVector4 {
private:
	WPFloat										_data[4];											//!< Four consecutive WPFloat elements
	friend class WCVector;																			//!< Make sure that WCVector4 is a friend
	friend class WCMatrix4;																			//!< Make sure that WCMatrix4 is a friend
	friend class WCMatrix;																			//!< Make sure that WCMatrix is a friend
public:
	WCVector4();																					//!< Default constructor
	WCVector4(const WPFloat i, const WPFloat j, const WPFloat k, const WPFloat l=1.0);				//!< Standard constructor
	WCVector4(const WPFloat *data);																	//!< Double array constructor
	WCVector4(const WPUInt size, const WPFloat* data);												//!< Size and pointer constructor
	WCVector4(const WCVector4& vector);																//!< Copy constructor
	~WCVector4()								{ }													//!< Standard destructor	
	//General access methods
	inline WPFloat I(void) const				{ return this->_data[0]; }							//!< Return the I element
	inline WPFloat J(void) const				{ return this->_data[1]; }							//!< Return the J element
	inline WPFloat K(void) const				{ return this->_data[2]; }							//!< Return the K element
	inline WPFloat L(void) const				{ return this->_data[3]; }							//!< Return the L element
	inline void I(const WPFloat i)				{ this->_data[0] = i; }								//!< Set the I element
	inline void J(const WPFloat j)				{ this->_data[1] = j; }								//!< Set the J element
	inline void K(const WPFloat k)				{ this->_data[2] = k; }								//!< Set the K element
	inline void L(const WPFloat l)				{ this->_data[3] = l; }								//!< Set the L element
	inline WCVector4 &Set(const WPFloat i, const WPFloat j, const WPFloat k, const WPFloat l=1.0)	//!< Set all the elements
					{ this->I(i); this->J(j); this->K(k); this->L(l); return *this; }	
					
	//Conversion methods
	WCVector ToVector(void);																		//!< Convert to a WCVector object
	WCMatrix ToMatrix(void);																		//!< Convert to a WCMatrix object
	void ToElement(DOMElement *element);															//!< Serialize the object

	//Overloaded Operators
	WCVector4 operator+(const WCVector4 &vector) const;												//!< Vector4 addition
	WCVector4 operator-(const WCVector4 &vector) const;												//!< Vector4 subtraction
	WCVector4 operator*(const WPFloat scalar) const;												//!< Scalar multiplication
	WCVector4 operator/(const WPFloat scalar) const;												//!< Scalar division
	WCMatrix4 operator*(const WCVector4 &vector) const;												//!< Vector4 multiplication
	WCMatrix  operator*(const WCVector &vector) const;												//!< Vector multiplication
	WCVector4 operator*(const WCMatrix4 &matrix) const;												//!< Matrix4 multiplication

	WCVector4& operator+=(const WPFloat scalar);													//!< Scalar plus equals
	WCVector4& operator+=(const WCVector4 &vector);													//!< Vector plus equals
	WCVector4& operator-=(const WPFloat scalar);													//!< Scalar minus equals
	WCVector4& operator-=(const WCVector4 &vector);													//!< Vector minus equals
	WCVector4& operator*=(const WPFloat scalar);													//!< Scalar times equals	
	WCVector4& operator/=(const WPFloat scalar);													//!< Scalar div equals
	
	WCVector4& operator=(const WCVector4 &vector);													//!< Vector equation
	WCVector4& operator=(const WCVector &vector);													//!< Vector equation
	bool	 operator==(const WCVector4 &vector);													//!< Vector equivalence
	bool	 operator!=(const WCVector4 &vector);													//!< Vector equivalence
	
	//Other Methods
	WPFloat Distance(const WCVector4 &vec) const;													//!< Calculate distance between two vector values
	WPFloat Magnitude(void) const;																	//!< Calculate the magnitude of the vector
	WCVector4 &Normalize(const bool wIgnore=false);													//!< Normalize the vector
	WCVector4 &Homogenize(void);																	//!< Homogenize the vector
	WPFloat NormL2(void) const;																		//!< L2 Norm
	WPFloat NormInf(void) const;																	//!< Infinity Norm
	WPFloat DotProduct(const WCVector4 &vector) const;												//!< Return the dot product
	WCVector4 CrossProduct(const WCVector4 &vector) const;											//!< Return the cross product
	
	//Friend classes
	friend std::ostream& operator<<(std::ostream& out, const WCVector4 &vec);						//!< Overloaded output operator
};


/***********************************************~***************************************************/


class WCVector {
private:
	long										_size;												//!< Size of the vector
	WPFloat*									_data;												//!< Four consecutive WPFloat elements
	friend class WCVector4;																			//!< Make sure that WCVector4 is a friend
	friend class WCMatrix4;																			//!< Make sure that WCMatrix4 is a friend
	friend class WCMatrix;																			//!< Make sure that WCMatrix is a friend
public:
	//Constructors and Destructors
	WCVector(long size);																			//!< Default constructor
	WCVector(long size, WPFloat value);																//!< Set all values constructor
	WCVector(long size, WPFloat* data);																//!< Alternative constructor
	WCVector(const WCVector& vector);																//!< Copy constructor
	~WCVector();																					//!< Standard destructor
																							
	//General Access Methods
	inline long Size(void) const				{ return this->_size; }								//!< Get the vector size
	inline void Set(int index,WPFloat value)	{ this->_data[index] = value; }						//!< Standard Set operation
	inline WPFloat Get(int index) const			{ return this->_data[index]; }						//!< Standard Get operation
	inline void SetEmpty(void)					{ memset(this->_data, 0, this->_size *sizeof(WPFloat)); }	//!< Set the vector to all zeros
	inline WPFloat* GetData(void)				{ return this->_data; }								//!< Return the data array
	
	//Conversion Methods
	WCVector4 ToVector4(void);																		//!< Convert to a WCVector4 object
	WCMatrix ToMatrix(void);																		//!< Convert to a WCMatrix object
	
	//Operators
	WCVector operator+(const WCVector &vector);														//!< Vector addition
	WCVector operator-(const WCVector &vector);														//!< Vector subtraction
	WCVector operator*(const WPFloat scalar);														//!< Scalar multiplication
//	WCVector operator/(const WPFloat scalar);														//!< Scalar division
	WCMatrix operator*(const WCVector &vector);														//!< Vector multiplication
	WCVector operator*(const WCMatrix &matrix);														//!< Matrix multiplication
	WCVector& operator=(const WCVector4 &vector);													//!< Vector equation
	WCVector& operator=(const WCVector &vector);													//!< Vector equation
	bool	 operator==(const WCVector &vector);													//!< Vector equivalence

	//Other Methods
//	WCVector &Normalize(void);																		//!< Normalize the vector
//	WCVector &Homogenize(void);																		//!< Homogenize the vector
	WPFloat NormL2(void);																			//!< L2 Norm
	WPFloat NormInf(void);																			//!< Infinity Norm
	WPFloat DotProduct(const WCVector &vector);														//!< Return the dot product

	//Print Method
	void Print(int indent=0);																		//!< Print the contents of the vector
	friend std::ostream& operator<<(std::ostream& out, const WCVector &vec);						//!< Overloaded output operator
};


/***********************************************~***************************************************/


#endif //__WUTIL_VECTOR_H__

