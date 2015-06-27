/*  $Id: FullExample.H 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file FullExample.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class FullExample
 *  \brief An Example with a fully uncompressed Feature vector.
 *
 *  These instances are expensive to create, as uncompressing the Feature
 *  vector takes roughly 240x as long as perfoming a single Feature test.
 *  A FullExample also takes up a relatively large amount of memory,
 *  roughly 720 bytes.
 *  However, it provides very fast Feature operations, faster than an
 *  IntermediateExample.
 *
 */

#ifndef __FullExample_H__
#define  __FullExample_H__

#include "learn/examples/AbstractExample.H"

#include "learn/features/Features.H"

#include "parse/Sentence.H"

#include "universal/ID.H"


#include <google/sparse_hash_set>
#include <google/dense_hash_set>


class Example;
class IntermediateExample;

template<typename FEATURE_HASH>
class FullExample : public AbstractExample {
public:
	FullExample(const Example& e);
	FullExample(const IntermediateExample& e);

	template<typename FEATURE_HASH2> FullExample(const FullExample<FEATURE_HASH2>& e);

	bool has_feature(ID<Feature> f) const;
	bool has_feature(const Feature* f) const;

	const FEATURE_HASH& fset() const { assert(_fset); assert(!_fset->empty()); return *_fset; }

	static string name();

	/// From which sentence is this FullExample derived?
	const ID<Sentence>& sentence() const { assert(!_sentence.empty()); return _sentence; }

protected:
	void construct(const IntermediateExample& e);
	void construct_help();

	ID<Sentence> _sentence;

	boost::shared_ptr<FEATURE_HASH> _fset;
};

typedef FullExample<google::sparse_hash_set<ID<Feature> > > SparseFullExample;
typedef FullExample<google::dense_hash_set<ID<Feature> > > DenseFullExample;

#endif /* __FullExample_H__ */
