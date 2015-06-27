/*  $Id: Path.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Path.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Path
 *  \brief A sequence of parse Production%s.
 *
 */

#ifndef __Path_H__
#define  __Path_H__

#include "parse/Production.H"

#include <list>
#include <ostream>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class Path {
public:
	Path() { this->clear(); }
	void clear() { productions.clear(); }

	/// Add a step to the path.
	/// \param p The production to be added to the path.
	void add(const Production& p) {
		productions.push_back(p);
	}

	/// Output productions in a form that can be
	/// read by Treebank::lex_path
	string to_string() const {
		string s;
		list<Production>::const_iterator pr;
		for (pr = productions.begin(); pr != productions.end(); ) {
			s += pr->to_string();
			pr++;
			if (pr != productions.end())
				s += " ";
		}
		return "(" + s + ")";
	}

	/// Pop the first production from the path.
	/// \return The production popped.
	/// \todo Slow to return Production and not reference?
	const Production pop() {
		assert(!productions.empty());
		unsigned s = productions.size();
		Production p = productions.front();
		productions.pop_front();
		assert(s-1 == productions.size());
		return p;
	}

	/// Is this parse path empty?
	/// \return True iff the parse path is in fact empty.
	bool empty() const {
		return productions.empty();
	}

	unsigned size() const {
		return productions.size();
	}

	const Production& back() const {
		return productions.back();
	}

private:
	/// A list of the productions in this path
	list<Production> productions;
};

#endif /* __Path_H__ */
