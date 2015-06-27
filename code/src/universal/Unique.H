/*  $Id: Unique.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Unique.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Unique
 *  \brief Maintain object uniqueness.
 *
 *  A factory class for storing objects uniquely.
 *  Inheriting from UniqueObject will turn the class into a type (compare: token).
 *  This class is based, in spirit, upon the Singleton design pattern (GoF, pp. 127-134).
 *  The difference is that our implementation does eager object
 *  construction (GoF's is lazy).
 *
 *  \todo Rewrite using terms "object type" and "canonical address"
 *  \todo Use Unique class in libparse/ too?
 *  \todo Allow duplicate inserts without crashing?
 *  \todo More detailed description.
 *  \todo Use const? virtual?
 *
 */

#ifndef __Unique_H__
#define  __Unique_H__

#include "universal/LIST.H"

#include <ext/hash_map>
//#include <typeinfo>
#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
#endif /* DOXYGEN */

template <class T>
class Unique {
public:
	/// Adds an object type.
	/// \param t The object which we are inserting.
	/// \return The new canonical address of this object type.
	/// \warning Fails if Unique<T> is locked against insertions.
	/// (Unique<T> is locked after the first call to ptr())
	/// \warning An assertion is thrown if the object is already present.
	static const T* insert(const T& t);

	/// Find an object's canonical address.
	/// \param t The object which address we desire.
	/// \return A canonical pointer to the object.
	/// \note Calling this method write locks Unique<T> against future inserts.
	static const T* ptr(const T& t);

	/// Return all canonical addresses that we keep track of.
	static const LIST<const T*>& all() {
		_write_locked = true;
		return _all;
	}

private:
	/// This constructor is private. "This ensures that only one
	/// instance can ever get created." ---GoF, pg.129
	Unique();

	/// Unique<T> is write-locked (against inserts) after the first
	/// read (call to ptr()).
	static bool _write_locked;

	static hash_map<T, const T*, hash<T>, equal_to<T> > object_map;
	static LIST<const T*> _all;
};


// Initialize and set default values for member variables
template<class T> bool Unique<T>::_write_locked = false;
template<class T> hash_map<T, const T*, hash<T>, equal_to<T> > Unique<T>::object_map = hash_map<T, const T*, hash<T>, equal_to<T> >();
template<class T> LIST<const T*> Unique<T>::_all = LIST<const T*>();

/// Adds an object type.
/// \param t The object which we are inserting.
/// \return The new canonical address of this object type.
/// \warning Fails if Unique<T> is locked against insertions.
/// (Unique<T> is locked after the first call to ptr())
/// \warning An assertion is thrown if the object is already present.
template<class T> const T* Unique<T>::insert(const T& t) {
	assert(!Unique<T>::_write_locked);
	assert(object_map.find(t) == object_map.end());

	object_map.insert(make_pair(t, &t));
	assert(object_map.find(t)->first == t);
	object_map.find(t)->second = &(object_map.find(t)->first);
	assert(object_map.find(t)->second == &(object_map.find(t)->first));

	_all.push_back(object_map.find(t)->second);
	assert (_all.size() == object_map.size());

	return object_map.find(t)->second;
}

/// Find an object's canonical address.
/// \param t The object which address we desire.
/// \return A canonical pointer to the object.
/// \note Calling this method write locks Unique<T> against future inserts.
template<class T> const T* Unique<T>::ptr(const T& t) {
	Unique<T>::_write_locked = true;

	typename hash_map<T, const T*, hash<T>, equal_to<T> >::iterator p = object_map.find(t);

	assert (p != object_map.end());
	assert (p->second != NULL);

#ifdef SANITY_CHECKS
	assert (*p->second == t);
#endif /* SANITY_CHECKS */

	return p->second;
}

#endif /* __Unique_H__ */
