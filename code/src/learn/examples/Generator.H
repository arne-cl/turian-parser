/*  $Id: Generator.H 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Generator.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Generator
 *  \brief A class for generating training Example%s.
 *
 *  \todo Don't store Treebank explicitly. (Need to convert
 *  State%s/Action%s/ItemToken%s to use smart pointers.)
 *
 */

#ifndef __Generator_H__
#define  __Generator_H__

#include "learn/examples/Example.H"
#include "learn/examples/Examples.H"

#include "parse/Treebank.H"

#include "universal/globals.H"

#include <deque>

class Ensemble;

class Generator : public Examples<Example> {
public:
	/// Construct a set of Example%s.
	/// \param filename The name of the file from which to read the Treebank and set of Example%s.
	/// based upon their computed margin.
	/// \param target_label Label of desired Example%s.
	Generator(string filename, const Label target_label);

/*
	/// Choose new parse Path%s.
	/// i.e. Draw a new sample set from the Treebank.
	/// \param seed Seed for the random number generator
	void choose_new_parse_paths(unsigned long seed);
	*/

private:
	/// Read in a set of Example%s.
	/// \param filename The name of the file from which to read the
	/// Treebank and set of Example%s.
	/// \param target_label Label of desired Example%s.
	/// \param target_label Label of desired Example%s.
	void load(string filename, const Label target_label);

	/// \todo REMOVEME. Don't store Treebank explicitly.
	/// (Need to convert State%s/Action%s/ItemToken%s
	/// to use smart pointers.)
	Treebank _treebank;
///////////////////////////////////////////
};

#endif /* __Generator_H__ */
