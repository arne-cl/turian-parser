/*  $Id: NBest.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file NBest.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class NBest
 *  \brief An n-best list implementation.
 *
 *  This implementation is based a sorted container, the multiset.
 *  \warning Duplicates are *not* checked for.
 *
 *  The type T of elements in the NBest list must
 *  define 'operator>' for priority comparison.
 *
 *  \bug There may be some tiebreaking issues when items in the
 *  n-best list have equal priority.
 *  \todo Make 'priority' class which chooses a random value sub-priority
 *  for tiebreaks.
 *
 */

#ifndef __NBest_H__
#define  __NBest_H__

#include "universal/Debug.H"

#include <set>
#include <cassert>
#ifndef DOXYGEN
using namespace std;
#endif

template <typename T>
class NBest {
protected:
	/// Maximum size of the priority queue.
	size_t n;
public:
	/// Create a n-best list.
	/// \param n The size of the n-best list.
	NBest(size_t _n) : n(_n) { this->sanity_check(); }

	/// Clear the n-best list.
	void clear();

	/// Is the n-best list empty?
	bool empty() const;

	/// Is the n-best list full?
	bool full() const;

	/// How many elements are there?
	size_t size() const;

	/// Retrieve the highest priority element.
	const T& top() const;

	/// Retrieve the lowest priority element.
	const T& bottom() const;

	/// Push an element onto the list.
	/// Iff the list isn't full, just insert the new element.
	/// Otherwise, the list is full:
	///	Iff the pushed element has lower priority than the
	///	worst element in the list, then don't insert the element.
	///	Otherwise, evict the worst element and insert the
	///	new element.
	///
	/// \warning We do not detect duplicates: Iff there is already an
	/// element on the list with the same object (i.e. object equality,
	/// not necessarily priority equality), we won't detect that.
	///
	/// \bug There may be some tiebreaking issues when items in the
	/// n-best list have equal priority. Specifically, we don't do
	/// proper tiebreaking if two or more items (including or excluding
	/// the pushed item) have the same worst priority.
	void push(const T& t);

	/// Remove the highest priority item.
	void pop();

//	friend ostream& operator<<(ostream &o, const NBest& q);
//	friend debugstream& operator<<(ostream &o, const NBest& q);

private:
	void sanity_check();

	multiset<T, greater<T> >	_items;		///< Storage of the elements
};

/// \todo WRITEME: More vigorous checks. Check all pointers.
template <typename T> void NBest<T>::sanity_check() {
	assert(n > 0);
	assert(_items.size() <= n);
}

template <typename T> void NBest<T>::push(const T& t) {
	this->sanity_check();

	if (!this->full()) {
		// If the list is not full, just insert t.
		_items.insert(t);
	} else {
		// If the n-best list is full, and our priority
		// is lower than the worst priority, then don't
		// insert t.
		if (t < this->bottom()) return;

		// Otherwise, erase the worst element and
		// insert t.
		typename multiset<T>::iterator last = _items.end();
		_items.erase(--last);
		_items.insert(t);
		assert(this->full());
	}

	this->sanity_check();
}

/// Clear the n-best list.
template <typename T> void NBest<T>::clear() {
	this->sanity_check();
	_items.clear();
	this->sanity_check();
}

/// Is the n-best list empty?
template <typename T> bool NBest<T>::empty() const {
	return _items.empty();
}

/// Is the n-best list full?
template <typename T> bool NBest<T>::full() const {
	return _items.size() == n;
}

/// How many elements are there?
template <typename T> size_t NBest<T>::size() const {
	return _items.size();
}

/// Retrieve the highest priority element.
template <typename T> const T& NBest<T>::top() const {
	return *(_items.begin());
}

/// Retrieve the lowest priority element.
template <typename T> const T& NBest<T>::bottom() const {
	typename multiset<T>::iterator last = _items.end();
	return *(--last);
}

/// Remove the highest priority item.
template <typename T> void NBest<T>::pop() {
	this->sanity_check();
	_items.erase(_items.begin());
	this->sanity_check();
}

#endif /* __NBest_H__ */
