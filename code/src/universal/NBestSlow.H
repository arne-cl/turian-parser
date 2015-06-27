/*  $Id: NBestSlow.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file NBestSlow.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class NBestSlow
 *  \brief A naive n-best list implementation.
 *
 *  This implementation is based upon the SGI STL priority_queue code.
 *
 *  \warning SLOW! Unfortunately, push operations are worst-case linear in n
 *  (the size of the list). Reason being, we may have to walk the entire
 *  list to find if the element is already in the n-best list.  For this
 *  reason, one should choose a low n.
 *
 *  The _Sequence container (to which we adapt the NBestSlow) must, by
 *  default, define 'operator<' for priority comparison.
 *  _Sequence must also define 'operator<<' for ostreams and debugstreams.
 *  Iff SANITY_CHECKS is defined, _Sequence must also define 'operator=='
 *  for *priority* equality (not object equality).
 *  _Sequence must also define object_equal_to(), to check for *object*
 *  equality (ignoring priority).
 *  _Tp must define bool empty() to determine if the object has been initialized.
 *
 *  \bug There may be some tiebreaking issues when items in the n-best
 *  list have equal priority. For example, if several items have the same
 *  worst priority, then pushing a new item onto the n-best list should evict
 *  a random worst priority item. However, we evict the first lowest-priority
 *  item found.
 *
 *  \todo Move the implementation to a .C file. Or, at the very least,
 *  move the implementation to the bottom of this file.
 *
 */

#ifndef __NBestSlow_H__
#define  __NBestSlow_H__

/// Disallow N values higher than MAXN.
const unsigned MAXN = 31278;

#include "universal/Debug.H"

#include <queue>
#include <vector>
#include <cassert>
#ifndef DOXYGEN
using namespace std;
#endif

template <class _Tp, class _Sequence = vector<_Tp>, 
          class _Compare = less<typename _Sequence::value_type> >
class NBestSlow {
public:
  typedef typename _Sequence::value_type	value_type;
  typedef typename _Sequence::size_type		size_type;
  typedef          _Sequence			container_type;

  typedef typename _Sequence::reference		reference;
  typedef typename _Sequence::const_reference	const_reference;

  typedef typename _Sequence::iterator		iterator;
  typedef typename _Sequence::const_iterator	const_iterator;

protected:
	_Sequence c;
	_Compare comp;
	size_type n;
public:
	/// Create a n-best list.
	/// \param n The size of the n-best list.
	NBestSlow(size_type _n) : c(), n(_n), _worst(), _popcnt(0), _pushcnt(0), _oeqcnt(0) {
		assert(0 <= _n && _n <= MAXN);
	}

	void clear() { c.clear(); }

	bool empty() const { return c.empty(); }

	bool full() const { return c.size() == n; }

	size_type size() const { return c.size(); }

	const_reference top() const { return c.front(); }

	/// Push an element onto the list.
	///
	/// Iff there is already an element on the list with the same object
	/// (i.e. object equality, not necessarily priority equality):
	///	If the pushed element has a higher priority than the duplicate
	///	in the list, the priority of the duplicate is updated.
	///	Regardless, we don't insert the pushed element onto the list.
	///
	/// Otherwise:
	///	Iff the list isn't full, just insert the new element.
	///	Otherwise, the list is full:
	///		Iff the pushed element has lower priority than the
	///		worst element in the list, then don't insert the element.
	///		Otherwise, evict the worst element and insert the
	///		new element.
	///
	/// \note Unfortunately, push operations are worst-case linear in
	/// n (the size of the list). Reason being, we may have to walk the
	/// entire list to find if the element is already in the n-best list.
	/// For this reason, one should choose a low n.
	void push(const value_type& x) {
		_pushcnt++;

		// First, see if x is already in the n-best list.
		// FIXME: Use a const_iterator
		iterator i;
		for (i = c.begin(); i != c.end(); i++) {
			if (x.object_equal_to(*i)) {
				_oeqcnt++;
				break;
			}
		}
		if (i != c.end()) {
			// Yes, x is already in the n-best list.
			assert(x.object_equal_to(*i));
			reference x2 = *i;

			// Does x have a higher priority than its copy
			// in the list?
			if (x > x2) {
				// Yes, so update the priority.
				if (x2 == _worst) {
					// Also, update the worst element,
					// if it used to be x's copy.
					x2 = x;
					_worst = *(this->find_worst());
				} else {
					x2 = x;
				}
			}

			// Regardless, we don't need to insert a duplicate.
			// So, just return.
			return;
		}

		// Otherwise, x is not already in the n-best list.

		if (!this->full()) {
			// If the list is not full, just insert x.
			c.push_back(x); 
			push_heap(c.begin(), c.end(), comp);
			if (c.size() == 1 || x < _worst) _worst = x;
		} else {
			// If the n-best list is full, and our priority
			// is lower than the worst priority, then don't
			// insert x.
			if (!_worst.empty() && x < _worst) return;

			// Otherwise, erase the worst element and
			// insert x.
			c.erase(this->find_worst_var());
			c.push_back(x); 
			push_heap(c.begin(), c.end(), comp);

			// Update the _worst item.
			_worst = *(this->find_worst());
		}
	}

	void pop() {
		_popcnt++;

		assert(!c.empty());
		pop_heap(c.begin(), c.end(), comp);
		c.pop_back();
	}

	const_reference worst() const {
		assert(!c.empty());
//#ifdef SANITY_CHECKS
		assert(_worst == *(this->find_worst()));
//#endif /* SANITY_CHECKS */
		return _worst;
	}

	friend ostream& operator<<(ostream &o, const NBestSlow& q) {
		if (!q.empty()) {
//			o << q.n << "-best list (top=" << q.top() << ", worst=" << q.worst() << "): ";
			o << q.n << "-best list:\n";
			const_iterator i;
			for(i = q.c.begin(); i != q.c.end(); ) {
//				o << *i;
//				i++;
//				if (i != q.c.end()) o << ", ";
				o << "\t" << *i << "\n";
				i++;
			}
		} else {
			o << q.n << "-best list: [empty]";
		}
		return o;
	}

	friend debugstream& operator<<(debugstream &o, const NBestSlow &q) {
		if (!q.empty()) {
//			o << q.n << "-best list (top=" << q.top() << ", worst=" << q.worst() << "): ";
			o << q.n << "-best list:\n";
			const_iterator i;
			for(i = q.c.begin(); i != q.c.end(); ) {
//				o << *i;
//				i++;
//				if (i != q.c.end()) o << ", ";
				o << "\t" << *i << "\n";
				i++;
			}
		} else {
			o << q.n << "-best list: [empty]";
		}
		return o;
	}

	string stats() const {
		ostringstream o;
		o << n << "-best list stats: " << 1.*_pushcnt/_popcnt << " pushes/pop, " << _popcnt << " pops and " << _pushcnt << " pushes (" << _oeqcnt << " of equal objects)";
		return o.str();
	}

private:
	const_iterator find_worst() const {
		assert(!c.empty());
		const_iterator w = c.begin();
		const_iterator i;
		for(i = c.begin(); i != c.end(); i++) {
			if (*i < *w) w = i;
		}
		return w;
	}

	/// \todo REMOVEME. Adapt the above fn
	iterator find_worst_var() {
		assert(!c.empty());
		iterator w = c.begin();
		iterator i;
		for(i = c.begin(); i != c.end(); i++) {
			if (*i < *w) w = i;
		}
		return w;
	}

	value_type _worst;

	mutable unsigned _popcnt;
	mutable unsigned _pushcnt;
	mutable unsigned _oeqcnt;
};


#endif /* __NBestSlow_H__ */
