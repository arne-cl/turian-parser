/*  $Id: Cost.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Cost.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/Cost.H"
#include "learn/loss.H"
#include "universal/parameter.H"

/// Is this the same cost as c, regardless of finality?
bool Cost::operator==(const Cost& c) const {
	return _loss_sum == c._loss_sum;
}

/// Is this more costly than c?
/// \todo Describe tiebreaking.
bool Cost::operator>(const Cost& c) const {
	return _loss_sum > c._loss_sum;
}

/// Is this less costly than c?
bool Cost::operator<(const Cost& c) const {
	return !((*this == c) || (*this > c));
}

Cost Cost::plus(double conf) const {
	Cost newcost(*this);
	newcost._loss_sum += loss_of(conf);
	newcost._confs.insert(conf);
	return newcost;
}

bool Cost::_always_more_costly_than(const Cost& c) const {
	if (c.is_final()) {
		if (*this > c) return true;
	}
	return false;
}

string Cost::to_string() const {
	ostringstream o;
	o << _loss_sum << " " << _confs.to_string();
	return o.str();
}
