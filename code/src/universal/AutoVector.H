/*  $Id: AutoVector.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file AutoVector.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class AutoVector
 *  \brief An automatically resizing vector.
 *
 *  If we insert an element past the current end, a sufficient number
 *  of entries will be inserted (with a default value) to allow
 *  the insertion of the given element.
 *  An AutoVector can be locked, disabling future insertions.
 *
 */

#ifndef __AutoVector_H__
#define  __AutoVector_H__

#include "universal/ID.H"

#include <cassert>
#include <vector>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

/// No index can be larger than MAX_INDEX.
const unsigned MAX_INDEX = 100*1024*1024;	// 100M
//const unsigned MAX_INDEX = 262144;
//const unsigned MAX_INDEX = 65535;

template <class T>
class AutoVector {
public:
	/// Construct an AutoVector with an unspecified default value.
	AutoVector();

	/// Construct an AutoVector with an explicit default value.
	/// \param defaultT The default value for a T, assigned to new
	/// items created when resizing the vector.
	AutoVector(const T& defaultT);

	/// Disable lock assertions.
	void disable_locking();

	/// Clear the AutoVector, and unlock it.
	void clear() {
		_vector.clear();
		_locked = false;
	}

	/// Blithely retrieve an item.
	/// \param idx The index of the desired item. If the vector is
	/// not long enough to contain this index, then the vector will be
	/// automatically extended to this length. New entries in the
	/// vector will get the default value AutoVector<T>::_defaultT.
	/// \return The (writeable) item at the desired index. The
	/// default value AutoVector<T>::_defaultT if the vector was
	/// resized.
	/// \warning This method will throw an assertion if the
	/// AutoVector is locked.
	T& operator[] (unsigned idx);
	template<class T2> T& operator[] (const ID<T2>& idx) {
		return this->operator[](idx());
	}

	/// Carefully retrieve an item.
	/// \note If the AutoVector is not locked yet, this operation
	/// will automatically lock it.
	/// \param idx The index of the desired item.
	/// \warning This method will throw an assertion if the index
	/// is out-of-range.
	/// \return The (const) item at the desired index.
	/// \warning This method will throw an assertion if the value
	/// to be returned is the default value.
	const T& at(unsigned idx) const;
	template<class T2> const T& at(const ID<T2>& idx) const {
		return this->at(idx());
	}

	/// Return the size of the AutoVector.
	/// \note If the AutoVector is not locked yet, this operation
	/// will automatically lock it.
	unsigned size() const;

	/// True iff the AutoVector is empty.
	/// \note If the AutoVector is not locked yet, this operation
	/// will automatically lock it.
	bool empty() const;

	/// Swap the contents of two AutoVectors.
	void swap(AutoVector<T>& v);

	/// Lock this vector against resizing or writing.
	/// \warning This method will fail if the AutoVector is already locked.
	void lock() const;

	/// Unlock this vector against resizing or writing.
	void unlock() const;
private:
	vector<T> _vector;
	mutable bool _locked;
	T _defaultT;

	bool _use_locking;
};

/// Construct an AutoVector with an unspecified default value.
template<class T> AutoVector<T>::AutoVector() :
	_locked(false), _use_locking(true)
{
	assert(_vector.empty());
}

/// Construct an AutoVector with an explicit default value.
/// \param defaultT The default value for a T, assigned to new
/// items created when resizing the vector.
template<class T> AutoVector<T>::AutoVector(const T& defaultT) :
	_locked(false), _defaultT(defaultT), _use_locking(true)
{
	assert(_vector.empty());
}

/// Disable lock assertions.
template<class T> void AutoVector<T>::disable_locking() {
	// One final assertion, just for old time's sake...
	assert(_use_locking);

	_use_locking = false;
}

/// Blithely retrieve an item.
/// \param idx The index of the desired item. If the vector is
/// not long enough to contain this index, then the vector will be
/// automatically extended to this length. New entries in the
/// vector will get the default value AutoVector<T>::_defaultT.
/// \return The (writeable) item at the desired index. The
/// default value AutoVector<T>::_defaultT if the vector was
/// resized.
/// \warning This method will throw an assertion if the
/// AutoVector is locked.
template<class T> T& AutoVector<T>::operator[](unsigned idx) {
	assert(!_locked || !_use_locking);
	assert(idx <= MAX_INDEX);
	while (_vector.size() <= idx)
		_vector.push_back(_defaultT);
	return _vector.at(idx);
}

/// Carefully retrieve an item.
/// \note If the AutoVector is not locked yet, this operation
/// will automatically lock it.
/// \param idx The index of the desired item.
/// \warning This method will throw an assertion if the index
/// is out-of-range.
/// \return The (const) item at the desired index.
/// \warning This method will throw an assertion if the value
/// to be returned is the default value.
template<class T> const T& AutoVector<T>::at(unsigned idx) const {
	if (!_locked) _locked = true;
	assert(idx <= MAX_INDEX);
	assert(_vector.size() > idx);
//	assert(_vector.at(idx) != _defaultT);
	return _vector.at(idx);
}

/// Return the size of the AutoVector.
/// \note If the AutoVector is not locked yet, this operation
/// will automatically lock it.
template<class T> unsigned AutoVector<T>::size() const {
	if (!_locked) _locked = true;

	return _vector.size();
}

/// True iff the AutoVector is empty.
/// \note If the AutoVector is not locked yet, this operation
/// will automatically lock it.
template<class T> bool AutoVector<T>::empty() const {
	if (!_locked) _locked = true;

	return _vector.empty();
}

/// Swap the contents of two AutoVectors.
template<class T> void AutoVector<T>::swap(AutoVector<T>& v) {
	_vector.swap(v._vector);

	bool tmp_locked = _locked;
	_locked = v._locked;
	v._locked = tmp_locked;

	bool tmp_use_locking = _use_locking;
	_use_locking = v._use_locking;
	v._use_locking = tmp_use_locking;

	T tmp_defaultT = _defaultT;
	_defaultT = v._defaultT;
	v._defaultT = tmp_defaultT;
}

/// Lock this vector against resizing or writing.
/// \warning This method will fail if the AutoVector is already locked.
template<class T> void AutoVector<T>::lock() const {
	assert(!_locked || !_use_locking);
	_locked = true;
}

/// Unlock this vector against resizing or writing.
template<class T> void AutoVector<T>::unlock() const {
	_locked = false;
}

#endif /* __AutoVector_H__ */
