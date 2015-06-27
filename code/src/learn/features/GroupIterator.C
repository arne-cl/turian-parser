/*  $Id: GroupIterator.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file GroupIterator.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/features/GroupIterator.H"

#include "learn/examples/IntermediateExample.H"

#include "parse/ItemToken.H"

GroupIterator::GroupIterator(item_group_ty group, const IntermediateExample& e) {
	_group = group;
	_head = e.head();
	_size = 0;

	switch (_group) {
		case ALL_ITEMS:
			_items = &e.frontier();
			_i = 0;
			max = e.frontier().size();
			break;

		case ALL_LCONTEXT_ITEMS:
			_items = &e.lcontext();
			_i = 0;
			max = e.lcontext().size();
			break;

		case ALL_RCONTEXT_ITEMS:
			_items = &e.rcontext();
			_i = 0;
			max = e.rcontext().size();
			break;

		case ALL_SPAN_ITEMS:
			_items = &e.span();
			_i = 0;
			max = e.span().size();
			break;

		case NONLEFTMOST_SPAN_ITEMS:
			_items = &e.span();
			_i = 1;
			max = e.span().size();
			break;

		case NONRIGHTMOST_SPAN_ITEMS:
			_items = &e.span();
			_i = 0;
			max = e.span().size() - 1;
			break;

		case NONHEAD_SPAN_ITEMS:
			_items = &e.span();
			_i = 0;
			max = e.span().size();
			break;

		case LEFT_OF_HEAD_SPAN_ITEMS:
			_items = &e.span();
			_i = 0;
			max = e.head();
			break;

		case RIGHT_OF_HEAD_SPAN_ITEMS:
			_items = &e.span();
			_i = e.head() + 1;
			max = e.span().size();
			break;

		case HEAD_OR_LEFT_OF_HEAD_SPAN_ITEMS:
			_items = &e.span();
			_i = 0;
			max = e.head() + 1;
			break;

		case HEAD_OR_RIGHT_OF_HEAD_SPAN_ITEMS:
			_items = &e.span();
			_i = e.head();
			max = e.span().size();
			break;

		default: assert(0);
	}

	_size = max - _i;

	// NONHEAD_SPAN_ITEMS is the only discontinuous group.
	if (_group == NONHEAD_SPAN_ITEMS && _i == _head) {
		_i++;
		_size--;
	}

	if (_size < 0) _size = 0;
}

unsigned GroupIterator::size() const {
	if (_group == NONHEAD_SPAN_ITEMS && _i <= _head) return max - _i - 1;
	return max - _i;
}

bool GroupIterator::end() const {
	return (int)_i >= max;
}

void GroupIterator::operator++(int) {
	assert ((int)_i < max);

	_i++;

	// NONHEAD_SPAN_ITEMS is the only discontinuous group.
	if (_group == NONHEAD_SPAN_ITEMS && _i == _head) _i++;
}

const ItemToken& GroupIterator::operator*() const {
	assert ((int)_i < max);
	return *_items->at(_i);
}
