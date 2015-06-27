/*  $Id: StateCost.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file StateCost.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class StateCost
 *  \brief A ParseState and its associated Cost.
 *
 */

#ifndef __StateCost_H__
#define  __StateCost_H__

#include "learn/Cost.H"
#include "parse/ParseState.H"

class StateCost {
public:
	StateCost() : _state(), _cost(), _empty(true) { }
	StateCost(const ParseState& state, const Cost& cost) :
			_state(state), _cost(cost), _empty(false) {
		this->sanity_check();
	}

	bool empty() const { return _empty; }

	bool is_final() const { assert(!_empty); return _cost.is_final(); }
	void finalize() { assert(!_empty); _cost.finalize(); }

	bool operator<(const StateCost& x) const { assert(!_empty); return _cost > x._cost; }
	bool operator>(const StateCost& x) const { assert(!_empty); return _cost < x._cost; }
	bool operator==(const StateCost& x) const { assert(!_empty); return _cost == x._cost; }
	bool operator<(const Cost& cost) const { assert(!_empty); return _cost > cost; }
	bool operator>(const Cost& cost) const { assert(!_empty); return _cost < cost; }

	bool always_more_costly_than(const StateCost& x) const { assert(!_empty); return _cost.always_more_costly_than(x._cost); }
	bool always_less_costly_than(const StateCost& x) const { assert(!_empty); return _cost.always_less_costly_than(x._cost); }
	bool always_more_costly_than(const Cost& cost) const { assert(!_empty); return _cost.always_more_costly_than(cost); }
	bool always_less_costly_than(const Cost& cost) const { assert(!_empty); return _cost.always_less_costly_than(cost); }

	bool object_equal_to(const StateCost& x) const { assert(!_empty); return _state == x._state; }

	const Cost& cost() const { assert(!_empty); return _cost; }
	const ParseState& state() const { assert(!_empty); return _state; }

	friend ostream& operator<<(ostream &o, const StateCost& s) {
		assert(!s._empty); 
		//o << s.cost();
		o << s.cost().to_string() << " cost of " << s.state().to_string() << "\n";
		//o << s.cost() << " cost (" << s.state().items().size() << " items)";
		return o;
	}

        friend debugstream& operator<<(debugstream &o, const StateCost& s) {
		assert(!s._empty); 
		//o << s.cost();
		o << s.cost().to_string() << " cost of " << s.state().to_string() << "\n";
		//o << s.cost() << " cost (" << s.state().items().size() << " items)";
		return o;
	}

private:
	void sanity_check() const {
		assert(_state.is_final() == _cost.is_final());
	}

	ParseState _state;
	Cost _cost;
	bool _empty;
};


#endif /* __StateCost_H__ */
