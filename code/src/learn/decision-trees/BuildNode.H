/*  $Id: BuildNode.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file BuildNode.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class BuildNode
 *  \brief A Node in a BuildTree.
 *
 *  A BuildNode inherits from Node, and is a Node in a BuildTree (a Tree that is still being built).
 *  If this BuildNode is an "open" leaf, it is in the "frontier" of its BuildTree
 *  and is still accumulating Example Weights in Feature expansion vector,
 * i.e.\ we are still trying to split it.
 *
 *  \todo Subtype BuildNode to FrontierNode. FrontierNodes contain hashes
 *  and are nodes we are trying to split. BuildNodes only contain a weight sum
 *  and can be used for weighting, pruning, etc.
 *  Alternately, have a FrontierNode type and a plain Node type, the latter
 * which would store Weights for just that node, not for Feature vector expansions.
 *
 */

#ifndef __BuildNode_H__
#define  __BuildNode_H__

#include "learn/decision-trees/Node.H"

#include "learn/features/Features.H"

#include "learn/Weights.H"

#include "parse/Sentence.H"

#include "universal/Double.H"

#include <ostream>
#include <sstream>

class BuildTree;

class BuildNode : public Node {
//	friend class BuildTree;
public:
	BuildNode();

	/// Create a BuildNode with a parent.
	/// \param newid The NodeID to assign to this new node.
	/// \param parent The NodeID of the parent of the node about to be constructed.
	/// \param depth The depth of the node about to be constructed.
	/// NULL iff this is a root node.
	BuildNode(const NodeID newid, const NodeID parent, const unsigned depth);

	/// Clear this BuildNode.
//	void clear();

	bool is_root() const { return (m_parent == NO_NODE); }

	bool is_closed() const { return m_is_closed; }

	/// Treat one training example.
	/// Determine if an example is weighty enough to expand its feature
	/// vector and be accumulated in the split feature weights.
	/// Otherwise, we skip it.
	template<typename EXAMPLE> void treat(const EXAMPLE& e);

	/// Close against future splits.
	void close() {
		assert(!m_is_closed);
		m_is_closed = true;

		this->free_memory();
	}

	/// Find the best splitting feature for this node.
	/// Any non-degenerate splitting feature is subject to the following constraints:
	///	- The positive child must have examples from at least
	///	parameter::min_sentences_per_feature() different sentences.
	///	NOTE: We do *not* do this test for the negative leaf!
	///	- Some child of the split must have gain at least parameter::l1_penalty_factor()
	ID<Feature> best_split();

	string to_string(const string s="");

	/// \todo Instead of overloading, give this another name
	void perform_split(const ID<Feature>& f, const NodeID pos, const NodeID neg) {
		this->Node::perform_split(f, pos, neg);
		this->free_memory();
	}

private:
	const vector<Weights>& weight() const { return m_weight; }
	const Weights& total_weight() const { return m_total_weight; }

	/// Free the memory used by this object.
	void free_memory();

	template<typename EXAMPLE> void add_features(const EXAMPLE& e);
	template<typename FS> void add_weight(const FS& fs, bool correct, const Double& w, const ID<Sentence>& s);

	bool m_is_closed;

	/// Weight totals (non-discarded)
	Weights m_total_weight;

	/// Weight values for the features of examples that fall in this node.
	vector<Weights> m_weight;

	/// Different sentences into which each feature falls.
	vector<set<ID<Sentence> > > m_sentences;
};

#endif /* __BuildNode_H__ */
