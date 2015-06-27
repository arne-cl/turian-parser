/*  $Id: FastMap.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file FastMap.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class FastMap
 *  \brief A fast map with unsigned keys.
 *
 *  A FastMap provides fast lookup by unsigned keys.
 *
 *  It provides a lock() method, after which inserts are disallowed but
 *  reads are very fast. Reads are disallowed before lock() is called.
 *  Since it converts its internal map to a (sparse) vector, the fast map requires
 *  O(max_key) entries, where max_key is the highest key value.
 *
 *  This structure should be used when we want a fast map for unsigned
 *  keys that will not be updated subsequent to initial inserts, and which
 *  will not have a large key range.
 *
 *  \todo Use FastMap elsewhere?
 *  \todo Make size = max key - min key.
 *  \todo FastSet?
 *  \todo Use const?
 *  \todo Use virtual?
 *  \todo Make this more like HashMap?
 *
 */

#ifndef __FastMap_H__
#define  __FastMap_H__

/// Disallow key values higher than MAXKEY.
const unsigned MAXKEY = 4096;

#include <cassert>
#include <vector>
#include <ext/hash_map>
#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
#endif

/// A fast map with unsigned keys.
///
/// A FastMap provides fast lookup by unsigned keys.
///
/// It provides a lock() method, after which inserts are disallowed but
/// reads are very fast. Reads are disallowed before lock() is called.
/// Since it converts its internal map to a (sparse) vector, the fast map requires
/// O(max_key) entries, where max_key is the highest key value.
///
/// This structure should be used when we want a fast map for unsigned
/// keys that will not be updated subsequent to initial inserts, and which
/// will not have a large key range.
/// 
/// \todo Use FastMap elsewhere?
/// \todo Make size = max key - min key.
/// \todo FastSet?
/// \todo Use const?
/// \todo Use virtual?
/// \todo Make this more like HashMap?
/// \todo Add size() method?
template <class T>
class FastMap {
public:
	/// Add <T> ?
	FastMap();
	~FastMap();

	/// Initialize this object, unlocking it.
	void clear();

	/// Insert a (key,value) pair.
	/// \warning This method will fail if lock() has been called.
	/// \warning This method will fail if this key is already present.
	void insert(const pair<unsigned, T> p);
	void insert(const unsigned i, const T t) { this->insert(make_pair((unsigned)i,(T)t)); }

	/// Write-lock this object.
	/// After write-locking, inserts are disallowed by reads are very fast.
	void lock();

	/// Retrieve the value for a particular key.
	/// \warning This method will fail if lock() hasn't been called yet.
	const T& operator[](unsigned i) const;

	/// Does this key exist?
	/// \warning This method will fail if lock() hasn't been called yet.
	bool exists(unsigned i) const;

	unsigned size() const;

private:
	void destroy();

	hash_map<unsigned, T, hash<unsigned>, equal_to<unsigned> >* _hsh;
	vector<pair<bool, T> > _vec;
	bool _locked;
	unsigned _max_key;
	unsigned _size;
};

template<class T> void FastMap<T>::destroy() {
	if (_locked) {
		assert(!_hsh);
		_vec.clear();
	} else {
		assert(_vec.empty());
		if (_hsh) {
			assert(_hsh);
			_hsh->clear();
			delete _hsh;
			_hsh = NULL;
		}
	}

	assert(!_hsh);
	assert(_vec.empty());
}

/// Initialize this object, unlocking it.
template<class T> void FastMap<T>::clear() {
	this->destroy();

	_locked = false;
	_hsh = new hash_map<unsigned, T, hash<unsigned>, equal_to<unsigned> >;
	assert(_hsh);
	_hsh->clear();
	_max_key = 0;
	_size = 0;
}

template<class T> unsigned FastMap<T>::size() const {
	if (_locked) return _size;
	else {
		assert(_hsh);
		return _hsh->size();
	}
}

template<class T> FastMap<T>::FastMap() :
		_hsh(NULL), _locked(true), _max_key(0), _size(0) {
	this->clear();
}

template<class T> FastMap<T>::~FastMap() {
	this->destroy();
}

/// Insert a (key,value) pair.
/// \warning This method will fail if lock() has been called.
/// \warning This method will fail if this key is already present.
template<class T> void FastMap<T>::insert(const pair<unsigned, T> p) {
	assert(!_locked);
	assert(_hsh);
	assert(p.first <= MAXKEY);
	assert(_hsh->find(p.first) == _hsh->end());
	_hsh->insert(p);
	if (p.first > _max_key) _max_key = p.first;
	assert(_hsh->find(p.first) != _hsh->end());
	assert(_hsh->find(p.first)->first == p.first);
//	assert(_hsh->find(p.first)->second == p.second);
}

/// Write-lock this object.
/// After write-locking, inserts are disallowed by reads are very fast.
template<class T> void FastMap<T>::lock() {
	assert(!_locked);
	assert(_hsh);

	assert(_max_key <= MAXKEY);
	// Create a vector with _max_key+1 elements, all empty (i.e. false)
	_vec = vector<pair<bool, T> >(_max_key+1, make_pair(false, T()));
	assert(_vec.size() == _max_key+1);

	// Insert all hash_map entries into the vector.
	// WTF does this give an Error when typename is not present?
	typename hash_map<unsigned, T, hash<unsigned>, equal_to<unsigned> >::iterator i;
	for (i = _hsh->begin(); i != _hsh->end(); i++)
		_vec.at(i->first) = make_pair(true, i->second);

	_size = _hsh->size();

	// Free the hash_map
	assert(_hsh);
	_hsh->clear();
	delete _hsh;
	_hsh = NULL;
	_locked = true;
}

/// Retrieve the value for a particular key.
/// \warning This method will fail if lock() hasn't been called yet.
template<class T> const T& FastMap<T>::operator[](unsigned i) const {
	assert(_locked);
	assert(_vec.at(i).first);
	return _vec.at(i).second;
}

/// Does this key exist?
/// \warning This method will fail if lock() hasn't been called yet.
template<class T> bool FastMap<T>::exists(unsigned i) const {
	assert(_locked);
	if (i >= _vec.size()) return false;
	return _vec.at(i).first;
}

#endif /* __FastMap_H__ */
