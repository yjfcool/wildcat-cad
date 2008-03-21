#ifndef __WUTIL_COLLECTION_H__
#define __WUTIL_COLLECTION_H__


/*** Included Header Files ***/
#include "wutil.h"
#include <vector>

/*** Locally Defined Values ***/
//None


/*** Class Order ***/
//---WCCollection<T>


/***********************************************~***************************************************/


template <class T>
class WCCollection {
protected:
	std::vector<T>								_data;												//!< Data vector for the collection
	typename std::vector<T>::iterator			_iter;												//!< Built in iterator
public:
	//Constructors and Destructors
	WCCollection()								{ }													//!< Default constructor
	WCCollection(const T& col) : _data(col._data) { }												//!< Alternate constructor
	~WCCollection()								{ this->_data.clear(); }							//!< Default destructor
	
	//General Access Methods
	void Clear(void)							{ this->_data.clear(); }							//!< Clear the collection
	void Add(const T item)						{ this->_data.push_back(item); }					//!< Add an item to the collection
	void Remove(WPUInt index)					{ this->_iter = this->_data.begin();				//!< Removes a specific element
												  //for (int i=0; i<index; i++) _iter++;
												  _iter += index;
												  this->_data.erase(_iter); }
	WPUInt Size(void) const						{ return this->_data.size(); }						//!< Get the size of the collection
	T At(const WPUInt index) const				{ return this->_data.at(index); }					//!< Index into a specific item
	WPInt Find(const T value)					{ for (int i=0; i<this->_data.size(); i++)			//!< Find first index with given value
													if (this->_data[i] == value) return i;
													return -1; }
	
	T& operator=(const T& col)					{ this->_data.clear(); this->_data = col._data; }	//!< Clear the existing vector, and copy the old one
	bool operator==(const T& col)				{ return ( this->_data == col._data ); }			//!< Check to see if both collectons are identical
	bool operator!=(const T& col)				{ return ( this->_data != col._data ); }			//!< Check to see if both collections are not identical
	
	static WCCollection<T>* CollectionFromArray(const WPUInt count, T* data) {						//!< Create a collection from an array of data
												if (data == NULL) {	
													std::cout << "WCCollection::CollectionFromArray Error - NULL value passed\n"; 
													return NULL; }
												WCCollection<T> *collection = new WCCollection<T>();
												for (int i=0; i<count; i++) collection->Add(data[i]);
												return collection; }
};


/***********************************************~***************************************************/


#endif //__WUTIL_COLLECTION_H__

