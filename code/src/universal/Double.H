/*  $Id: Double.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Double.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Double
 *  \brief A generic floating-point class.
 *
 *  Allows us to transparently doing floating-point operations in the log domain.
 *
 *  \todo Inline some of these functions?
 *
 */

#ifndef __Double_H__
#define  __Double_H__

#include "universal/Debug.H"

#include <ostream>

// Change to 10e16 or 10e8?
const double BIGVAL = 10e30;

class Double {
public:
	/// Initialize to 0.
	Double();

	Double(double d);
	Double(bool x, double logd);

	double operator()() const;

	Double operator+(const Double& d) const;
	Double operator-(const Double& d) const;
	Double& operator+=(const Double& d);
	Double& operator-=(const Double& d);

	Double operator*(const Double& d) const;
	Double operator/(const Double& d) const;
	Double operator*(double d) const;

	bool operator<(const Double& d) const;
	bool operator>(const Double& d) const;
	bool operator==(const Double& d) const;
	bool operator!=(const Double& d) const;

	friend std::ostream& operator<<(std::ostream &o, const Double &d) {
		o << d();
		return o;
	}
	friend debugstream& operator<<(debugstream &o, const Double &d) {
		o << d();
		return o;
	}

private:
	double _d;
};

#endif /* __Double_H__ */
