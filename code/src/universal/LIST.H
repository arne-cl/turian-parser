/*  $Id: LIST.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file LIST.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#ifndef __LIST_H__
#define  __LIST_H__

#include <vector>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

/// \brief A generic list type.
///
/// Make no assumptions about these objects.
/// vector<T> is usually faster than list<T>.
/// \todo Profile, use deque<T> or vector<T>?
#define LIST	vector
//template<class T> typedef vector<T> LIST<T>;
//template<class T> class LIST<T> : public vector<T, __single_client_alloc> {
/*
class LIST : public vector {
	template<class T> friend ostream& operator<<(ostream &o, const LIST<T>& l) {
		return o;
	}

	template<class T> friend debugstream& operator<<(debugstream &o, const LIST<T>& l) {
		return o;
	}
}; */

#endif /* __LIST_H__ */
