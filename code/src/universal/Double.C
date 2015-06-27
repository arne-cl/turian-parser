/*  $Id: Double.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Double.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "universal/Double.H"

#include "universal/globals.H"		// For USE_LOGS

#include <cmath>
#include <cassert>

#ifdef USE_LOGS
const double ZERO = -BIGVAL;
const double LOG_BIGVAL = log(BIGVAL);

/// Initialize to 0.
Double::Double() : _d(ZERO) { }

Double::Double(double d) {
	assert(d >= 0);
	if (d == 0) _d = ZERO;
	else _d = log(d);
}

Double::Double(bool x, double logd) : _d(logd) { assert(x); }

double Double::operator()() const {
	if (_d == ZERO) return 0;
	return exp(_d);
}

Double Double::operator+(const Double& d) const {
#ifdef SANITY_CHECKS
	assert(fabs(_d) < LOG_BIGVAL && fabs(d._d) < LOG_BIGVAL);
#endif /* SANITY_CHECKS */
	if (d._d - _d > LOG_BIGVAL) return d;
	else if (_d - d._d > LOG_BIGVAL) return *this;
	else if (d._d > _d) return Double(true, _d + log(exp(d._d - _d) + 1));
	else return Double(true, d._d + log(exp(_d - d._d) + 1));
}

Double Double::operator-(const Double& d) const {
	if (fabs(_d - d._d) < parameter::small_epsilon()) return Double();

	assert(_d >= d._d);
	if (_d - d._d > LOG_BIGVAL) return *this;
	else return Double(true, d._d + log(exp(_d - d._d) - 1));
}

Double& Double::operator+=(const Double& d) {
#ifdef SANITY_CHECKS
	assert(fabs(_d) < LOG_BIGVAL && fabs(d._d) < LOG_BIGVAL);
#endif /* SANITY_CHECKS */
	if (d._d - _d > LOG_BIGVAL) *this = d;
	else if (_d - d._d > LOG_BIGVAL) ;
	else if (d._d > _d) _d += log(exp(d._d - _d) + 1);
	else _d = d._d + log(exp(_d - d._d) + 1);

	return *this;
}

Double& Double::operator-=(const Double& d) {
	if (fabs(_d - d._d) < parameter::small_epsilon()) {
		_d = ZERO;
	} else {
		assert(_d >= d._d);
		if (_d - d._d > LOG_BIGVAL) ;
		else _d = d._d + log(exp(_d - d._d) - 1);
	}
	return *this;
}

Double Double::operator*(const Double& d) const {
	return Double(true, _d + d._d);
}

Double Double::operator/(const Double& d) const {
	return Double(true, _d - d._d);
}

#else /* !USE_LOGS */



const double ZERO = 0;

Double::Double() : _d(ZERO) { }

Double::Double(double d) : _d(d) { }

Double::Double(bool x, double logd) { assert(0); }

double Double::operator()() const {
	return _d;
}

Double Double::operator+(const Double& d) const {
	return _d + d._d;
}

Double Double::operator-(const Double& d) const {
	return _d - d._d;
}

Double& Double::operator+=(const Double& d) {
	_d += d._d;
	return *this;
}

Double& Double::operator-=(const Double& d) {
	_d -= d._d;
	return *this;
}

Double Double::operator*(const Double& d) const {
	return Double(_d*d._d);
}

Double Double::operator/(const Double& d) const {
	return Double(_d/d._d);
}
#endif /* USE_LOGS */



Double Double::operator*(double d) const {
	return *this * Double(d);
}

bool Double::operator<(const Double& d) const {
	return _d < d._d;
}

bool Double::operator>(const Double& d) const {
	return _d > d._d;
}

bool Double::operator==(const Double& d) const {
	return _d == d._d;
}

bool Double::operator!=(const Double& d) const {
	return _d != d._d;
}
