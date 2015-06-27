/*  $Id: Tiebreak.H 1443 2006-01-26 04:16:14Z turian $
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Tiebreak.H
 *  $LastChangedDate: 2006-01-25 23:16:14 -0500 (Wed, 25 Jan 2006) $
 *  $Revision: 1443 $
 */
/*!
 *  \class Tiebreak
 *  \brief Define a canonical tiebreaking.
 *
 *  Given an object with operator< and operator== defined, assign it
 *  random value in the range [0, 1) which is used for tiebreaking.
 *
 */

#ifndef __Tiebreak_H__
#define  __Tiebreak_H__

//#include "universal/Random.H"

template <typename T>
class Tiebreak {
public:
//	Tiebreak(const T& t) : _t(t), _v(Random::value()) {}
	Tiebreak(const T& t) : _t(t), _v(drand48()) {}

//	void set(T t) { _t = t; }
	void set(const T& t) { _t = t; }

	const T& operator()() const { return _t; }

	bool operator<(const Tiebreak<T>& t) const {
		assert(*this != t);
		if (_t < t._t) return true;
		else if (_t > t._t) return false;
		assert(_t == t._t);
		return _v < t._v;
	}

	bool operator>(const Tiebreak<T>& t) const { return t < *this; }

//	Tiebreak<T>& operator=(const Tiebreak<T>& t) { _t = t._t; _v = t._v; return *this; }

	bool operator==(const Tiebreak<T>& t) const { return _t == t._t && _v == t._v; }
	bool operator!=(const Tiebreak<T>& t) const { return !(*this == t); }

private:
	T _t;
	double _v;
};

#endif /* __Tiebreak_H__ */
