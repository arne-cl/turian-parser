/*  $Id: UniqueObject.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file UniqueObject.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class UniqueObject
 *  \brief Maintain object uniqueness.
 *
 *  Inheriting from UniqueObjectObject will turn the class into a type (compare: token).
 *
 *  \note Please include the following methods in any class Foo that
 *  inherits from UniqueObject:
 *	const Foo* operator&() { return this->ptr(this); }
 *	void insert() { this->UniqueObject<Foo>::insert(this); }
 *
 *  \todo More detailed description.
 *  \todo Use UniqueObject class in libparse/ too?
 *  \todo Make sure that constructor UniqueObject<T>::UniqueObject<T> is called!
 *  \todo Use const?
 *  \todo Use virtual?
 *
 */

#ifndef __UniqueObject_H__
#define  __UniqueObject_H__

#include "universal/Unique.H"

#include <cassert>

template <class T>
class UniqueObject {
public:
	/// Retrieve the canonical address of this object type.
	/// \return The canonical pointer to this object type.
	const T* ptr(const T* p);

	/// Add this object to the set of canonical types.
	const T* insert(const T* p);

public:
//protected:
	/// \todo Make this protected?
	UniqueObject<T>() {
//		TRACE;
		_ptr = NULL;
	}

private:
	T* _ptr;
};

/// Retrieve the canonical address of this object type.
/// \return The canonical pointer to this object type.
template<class T> const T* UniqueObject<T>::ptr(const T* p) {
	if (_ptr == NULL) {
		_ptr = (T*)Unique<T>::ptr(*p);
	}
	assert(_ptr);
	return _ptr;
}

/// Add this object to the set of canonical types.
template<class T> const T* UniqueObject<T>::insert(const T* p) {
	_ptr = (T*)Unique<T>::insert(*p);
	assert(*_ptr == *p);
	return _ptr;
}

#endif /* __UniqueObject_H__ */
