/*  $Id: Node.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Node.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Node
 *  \brief A node in a decision Tree.
 *
 *  A leaf node has no children, and assigns a certain confidence to any
 *  Example that falls in that leaf.
 *  An internal node has a positive and negative child, determined by a splitting Feature.
 *
 *  \todo Convert IDs to be unique, universe-wide (not just unique tree-wide).
 *  \todo Change parent and children from ID to Node*.
 *
 */

#ifndef __Node_H__
#define  __Node_H__

#include "learn/features/Features.H"
#include "learn/Weights.H"

#include <ostream>
#include <sstream>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

template<typename EXAMPLE> class ExamplePtrs;

typedef unsigned NodeID;
//const NodeID NO_NODE = NO_ID;
/// \todo Cleaner way to define NO_NODE? Maybe start root node at NodeID 1?
const NodeID NO_NODE = (NodeID)-1;		// 0 is the root node

class Node {
public:
	/// Create a node with no parent.
	Node();

	/// Constructor for an internal node.
	Node(const NodeID id, const NodeID pos, const NodeID neg, const ID<Feature>& f, const unsigned depth);

	/// Constructor for a blank leaf node.
	Node(const NodeID newid, const NodeID parent, const unsigned depth);

	/// Constructor for a leaf node.
	Node(const NodeID newid, double conf, const Weights& initial_weights, const unsigned depth);

//	Node& operator= (const Node& n);

	NodeID id() const { assert(m_id != NO_NODE); return m_id; }

	/// \todo assert(m_pos_child != NO_NODE); ?
	NodeID pos_child() const { return m_pos_child; }

	/// \todo assert(m_neg_child != NO_NODE); ?
	NodeID neg_child() const { return m_neg_child; }

	const pair<NodeID, NodeID> children() const {
		assert(m_split_feature != NULL);
		assert(m_pos_child != NO_NODE);
		assert(m_neg_child != NO_NODE);
		return make_pair(m_neg_child, m_pos_child);
	}

	/// \todo assert(!m_split_feature.empty()); ?
	const Feature* split() const { assert(m_split_feature); return m_split_feature; }

	unsigned depth() const { return m_depth; }

	NodeID parent() const { return m_parent; }
	void set_parent(const NodeID p) { m_parent = p; }

	double confidence() const { assert(m_have_confidence_and_initial_weights); return m_confidence; }
	const Weights& initial_weights() const { return m_initial_weights; }

	/// Choose the confidence over exmpls that minimizes penalized loss,
	/// and then set the initial_weights.
	template<typename EXAMPLE> void set_confidence_and_initial_weights(const ExamplePtrs<EXAMPLE>& exmpls);

	/// Scale leaf confidence.
	/// \param alpha Factor by which the leaf confidence are multiplied.
	void multiply_by(const double alpha) { assert(m_have_confidence_and_initial_weights); m_confidence *= alpha; }

	bool is_leaf() const {
		assert(m_id != NO_NODE);
		assert((m_pos_child == NO_NODE && m_neg_child == NO_NODE) ||
				(m_pos_child != NO_NODE && m_neg_child != NO_NODE));
		return (m_pos_child == NO_NODE && m_neg_child == NO_NODE);
	}

	string to_string(const string s="") const;

	/// Compute the l1 penalty associated with this leaf.
	/// \pre this->is_leaf().
	double penalty() const;

protected:
	/// \todo Instead of overloading, give this another name
	void perform_split(const ID<Feature>& f, const NodeID pos, const NodeID neg) {
		m_split_feature = &(Features::instance()->from_id(f));
		m_pos_child = pos;
		m_neg_child = neg;
	}

	NodeID m_id;
	NodeID m_pos_child;
	NodeID m_neg_child;
	NodeID m_parent;

	/// The Feature that splits this node.
	const Feature* m_split_feature;

	/// Depth of this node.
	/// The root is depth 0.
	unsigned m_depth;

	bool m_have_confidence_and_initial_weights;

	/// The confidence predicted by this node.
	/// \todo Allow this variable only for leaf nodes?
	double m_confidence;

	/// Initial Weights (probabilities) associated with this feature.
	Weights m_initial_weights;
};

#endif /* __Node_H__ */
