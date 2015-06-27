/*  $Id: GroupIterator.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file GroupIterator.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class GroupIterator
 *  \brief An iterator for a group of items in an example.
 *
 *  WRITEME
 *
 */

#ifndef __GroupIterator_H__
#define  __GroupIterator_H__

#include "learn/features/features-help.H"

class IntermediateExample;
class ItemTokens;

class GroupIterator {
public:
	GroupIterator(item_group_ty group, const IntermediateExample& e);

	bool end() const;

	void operator++(int);

	const ItemToken& operator*() const;

	unsigned size() const;

	
private:
	unsigned _i, _head;
	int max, _size;
	item_group_ty _group;
	const ItemTokens* _items;
};

#endif /* __GroupIterator_H__ */
