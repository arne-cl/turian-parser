/*  $Id: SortedVector.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file SortedVector.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class SortedVector
 *  \brief A vector that is always kept sorted (ascending order).
 *
 *  \todo This implementation is very slow. Use a multiset instead.
 *
 */

#ifndef __SortedVector_H__
#define  __SortedVector_H__

#include <cassert>
#include <vector>
#include <ostream>
#include <sstream>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

template <class T>
class SortedVector {
public:
	/// Construct an SortedVector with an unspecified default value.
	SortedVector() : _v(vector<T>()) {}

	const T& at(unsigned idx) const { return _v.at(idx); }

	string to_string() const {
		ostringstream o;
		o << "[";
		for (typename vector<T>::const_iterator i = _v.begin();
				i != _v.end(); i++) {
			o << *i;
			if (i+1 != _v.end()) o << ", ";
		}
		o << "]";
		return o.str();
	}

	unsigned size() const { return _v.size(); }

	void insert(const T& t) {
		_v.push_back(t);
		sort(_v.begin(), _v.end());
	}

private:
	vector<T> _v;
};

#endif /* __SortedVector_H__ */
