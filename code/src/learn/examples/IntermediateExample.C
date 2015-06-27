/*  $Id: IntermediateExample.C 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file IntermediateExample.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/examples/IntermediateExample.H"
#include "learn/examples/Example.H"
#include "learn/features/Features.H"

#include "parse/State.H"

#include "universal/parameter.H"

IntermediateExample::IntermediateExample(const Example& e) : Example(e) {
	assert(_state);

	int i;
	for (i = _start; i <= _end; i++)
		_span.push_back(_state->frontier().at(i));
	for (i = _start-1; i >= 0; i--)
		_lcontext.push_back(_state->frontier().at(i));
	for (i = _end+1; i < (int)_state->frontier().size(); i++)
		_rcontext.push_back(_state->frontier().at(i));

	assert(_span.size() > 0 && _span.size() <= parameter::max_span_length());

	assert(_head < _span.size());

	_head_terminal = &_span.at(_head)->head_terminal();

	this->sanity_check();
}

/// Convert the example to a string.
/// \todo Output vectors
string IntermediateExample::to_string(const string s, bool longform) const {
	ostringstream o;
	o << s;
	if (this->have_is_correct())
		if (this->is_correct())
			o << "+";
		else
			o << "-";
	o << "IntermediateExample:";
	if (longform) {
		o << "\n";
		o << s << "\t";
	} else {
		o << " ";
	}
/*
	o << "confidence = " << m_confidence << "\n";

	if (longform) {
		o << s << "lcontext (end..start):\t";
		//for (ItemTokens::const_iterator i = m_lcontext.begin(); i != m_lcontext.end(); i++) {
		for (ItemTokens::const_reverse_iterator i = m_lcontext.rbegin(); i != m_lcontext.rend(); i++) {
			if (i != m_lcontext.rbegin())
				o << " / ";
			o << "(" << (*i)->to_string(true);
			if (!(*i)->is_terminal())
				for (ItemTokens::const_iterator c = (*i)->children().begin(); c != (*i)->children().end(); c++)
					o << " " << (*c)->to_string_with_children();
			o << ")";
		}
		o << "\n";
		o << s << "span (start..end):\t";
		for (ItemTokens::const_iterator i = m_span.begin(); i != m_span.end(); i++) {
			if (i != m_span.begin())
				o << " / ";
			o << "(" << (*i)->to_string(true);
			if (!(*i)->is_terminal())
				for (ItemTokens::const_iterator c = (*i)->children().begin(); c != (*i)->children().end(); c++)
					o << " " << (*c)->to_string_with_children();
			o << ")";
		}
		o << "\n";
		o << s << "rcontext (start..end):\t";
		for (ItemTokens::const_iterator i = m_rcontext.begin(); i != m_rcontext.end(); i++) {
			if (i != m_rcontext.begin())
				o << " / ";
			o << "(" << (*i)->to_string(true);
			if (!(*i)->is_terminal())
				for (ItemTokens::const_iterator c = (*i)->children().begin(); c != (*i)->children().end(); c++)
					o << " " << (*c)->to_string_with_children();
			o << ")";
		}
		o << "\n";
	}
*/

/*
	o << s;
	for (ItemTokens::const_reverse_iterator l = m_lcontext.rbegin(); l != m_lcontext.rend(); l++) {
		o << " " << (*l)->to_string_with_children();
	}
	o << " | ";
	for (ItemTokens::const_iterator i = m_span.begin(); i != m_span.end(); i++) {
		o << " " << (*i)->to_string_with_children();
	}
	o << " | ";
	for (ItemTokens::const_iterator i = m_rcontext.begin(); i != m_rcontext.end(); i++) {
		o << " " << (*i)->to_string_with_children();
	}
 */

	return o.str();
}

const ItemTokens& IntermediateExample::span() const {
	return _span;
}

const ItemTokens& IntermediateExample::lcontext() const {
	return _lcontext;
}

const ItemTokens& IntermediateExample::rcontext() const {
	return _rcontext;
}

/// The terminal item that is the head.
const ItemToken& IntermediateExample::head_terminal() const {
	assert(_head_terminal);
	return *_head_terminal;
}

/// \todo Cache feature lookups, for speed?
bool IntermediateExample::has_feature(ID<Feature> f) const {
	return Features::instance()->from_id(f).apply(*this);
}
bool IntermediateExample::has_feature(const Feature* f) const {
	return this->has_feature(f->id());
}

/// Get all FeatureIDs over which this IntermediateExample holds.
FeatureIDs IntermediateExample::flist() const {
	return Features::instance()->get(*this);
}
