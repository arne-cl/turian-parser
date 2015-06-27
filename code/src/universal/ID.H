/*  $Id: ID.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ID.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class ID
 *  \brief Unique identifiers.
 *
 *  These identifiers are templated by class, and are unique globally.
 *
 *  \todo Allow non-singleton, i.e. non-global IDs?
 *  \todo Disallow copying, and make an explicit ID::copy(const ID& src, ID& dst) method.
 *
 */

#ifndef __ID_H__
#define  __ID_H__

#include "universal/Archive.H"
#include "universal/globals.H"	// For NO_VALUE

#include <cassert>

//typedef long unsigned id_ty;
typedef unsigned id_ty;

//const id_ty NO_ID = NO_VALUE;
const id_ty NO_ID = ((id_ty)-1);

template <class T>
class ID {
public:
	ID() { _id = NO_ID; }

	/// Reset the IDs, and start again from the initial value.
	static void reset() {
		last_id = NO_ID;
	}

	/// Create a fresh (new) ID.
	id_ty create() {
		assert(_id == NO_ID);
		_id = ++last_id;
		assert(_id != NO_ID);
		return _id;
	}

	ID<T>& operator=(const id_ty& id) {
		assert(id != NO_ID);
		assert(id <= last_id);
		_id = id;
		return *this;
	}

	/// Retrieve this ID.
	id_ty operator()(void) const {
		assert(_id <= last_id);
		assert(_id != NO_ID);
		return _id;
	}

	/// Is this the empty (null) ID?
	bool empty(void) const {
		assert(_id <= last_id || _id == NO_ID);
		return (_id == NO_ID);
	}

	bool operator==(const ID<T>& i) const {
		assert(_id <= last_id);
		assert(i._id <= last_id);

		assert(_id != NO_ID);
		assert(i._id != NO_ID);
		return (_id == i._id);
	}

	bool operator<(const ID<T>& i) const {
		assert(!this->empty());
		assert(!i.empty());
		return _id < i._id;
	}

private:
	id_ty _id;

	static id_ty last_id;

	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & _id;
		ar & last_id;
	}
};

// Initialize and set default values for member variables
template<class T> id_ty ID<T>::last_id = NO_ID;

#include <ext/hash_map>
#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
namespace __gnu_cxx {
	template<class T> class hash<ID<T> > {
	//template<> template<class T> class hash<ID<T> > {
		public: size_t operator() (ID<T> id) const{ return id(); }
	};
}
#endif /* DOXYGEN */

#endif /* __ID_H__ */
