/*  $Id: Cost.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Cost.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Cost
 *  \brief The cumulative cost of the confidences of Action%s.
 *
 *  \todo Rename to Score to emphasize the greater is better.
 *  (Currently, greater is better, even though this class is called Cost.)
 *
 *  Different costs:
 *	- Add loss.
 *	- Multiply loss.
 *	- Sort losses.		<=== This is our implementation.
 *  \todo Describe tiebreaking.
 *
 */

#ifndef __Cost_H__
#define  __Cost_H__

#include "universal/Debug.H"
#include "universal/SortedVector.H"

#include <string>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class Cost {
public:
	Cost() : _final(false), _loss_sum(0) {}

	/// Is this is cost for a final state?
	bool is_final() const { return _final; }
	void finalize() { assert(!_final); _final = true; }

	Cost operator+(double conf) const { assert(!_final); return this->plus(conf); }

	Cost& operator+=(double conf) { (*this) = (*this) + conf; return *this; }

	/// Is this the same cost as c, regardless of finality?
	bool operator==(const Cost& c) const;

	/// Is this more costly than c?
	bool operator>(const Cost& c) const;

	/// Is this less costly than c?
	bool operator<(const Cost& c) const;

	bool operator>=(const Cost& c) const { return (*this > c) || (*this == c); }
	bool operator<=(const Cost& c) const { return (*this < c) || (*this == c); }

	/// Will this always be more costly than c?
	bool always_more_costly_than(const Cost& c) const {
		// If c isn't final, its cost could increase.
		if (!c.is_final()) return false;

		if (*this <= c) return false;

		return this->_always_more_costly_than(c);
	}

	/// Will this always be less costly than c?
	bool always_less_costly_than(const Cost& c) const {
		return c.always_more_costly_than(*this);
	}

	string to_string() const;

private:
	Cost plus(double conf) const;

	bool _always_more_costly_than(const Cost& c) const;

	/// Is this Cost for a final State?
	/// If so, no more confidence can be added to it.
	bool _final;

	SortedVector<double> _confs;
	double _loss_sum;
};

#endif /* __Cost_H__ */
