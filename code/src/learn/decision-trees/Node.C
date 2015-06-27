/*  $Id: Node.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Node.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/decision-trees/Node.H"

#include "learn/examples/Example.H"
#include "learn/examples/FullExample.H"
#include "learn/examples/Examples.H"

#include "learn/loss.H"

/// Create a node with no parent.
Node::Node() :
	m_id(NO_NODE),
	m_pos_child(NO_NODE),
	m_neg_child(NO_NODE),
	m_parent(NO_NODE),
	m_split_feature(NULL),
	m_depth(0),
	m_have_confidence_and_initial_weights(false)
{ }

/// Constructor for an internal node.
Node::Node(const NodeID id, const NodeID pos, const NodeID neg, const ID<Feature>& f, const unsigned depth) :
	m_id(id),
	m_pos_child(pos),
	m_neg_child(neg),
	m_parent(NO_NODE),
	m_split_feature(&(Features::instance()->from_id(f))),
	m_depth(depth),
	m_have_confidence_and_initial_weights(false)
{ }

/// Constructor for a blank leaf node.
Node::Node(const NodeID newid, const NodeID parent, const unsigned depth) :
	m_id(newid),
	m_pos_child(NO_NODE),
	m_neg_child(NO_NODE),
	m_parent(parent),
	m_split_feature(NULL),
	m_depth(depth),
	m_have_confidence_and_initial_weights(false)
{ }

/// Constructor for a leaf node.
Node::Node(const NodeID newid, double conf, const Weights& initial_weights, const unsigned depth) :
	m_id(newid),
	m_pos_child(NO_NODE),
	m_neg_child(NO_NODE),
	m_parent(NO_NODE),
	m_split_feature(NULL),
	m_depth(depth),
	m_have_confidence_and_initial_weights(true),
	m_confidence(conf),
	m_initial_weights(initial_weights)
{ }


/// Compute the l1 penalty associated with this leaf.
/// \pre this->is_leaf().
double Node::penalty() const {
	assert (this->is_leaf());
	assert(m_have_confidence_and_initial_weights);

	// We only do normalization during decision tree building,
	// so the parameter value is irrelevant here.
	//assert(!parameter::normalize_feature_counts());

	return fabs(this->confidence()) * parameter::l1_penalty_factor();
}

/// Choose the confidence over exmpls that minimizes penalized loss,
/// and then set the initial_weights.
template<typename EXAMPLE> void Node::set_confidence_and_initial_weights(const ExamplePtrs<EXAMPLE>& exmpls) {
	assert(!m_have_confidence_and_initial_weights);
	m_confidence = minimize_loss(exmpls);
	m_initial_weights = exmpls.initial_weight();
	m_have_confidence_and_initial_weights = true;
}

string Node::to_string(const string s) const {
	ostringstream o;
	o.precision(12);
	o << s;
	if (!this->is_leaf()) {
		o << "Node #" << m_id << ": ";
//		o << "(parent=" << m_parent << ") ";
		o << "+child=" << m_pos_child << " ";
		o << "-child=" << m_neg_child << " ";

		string fstr = m_split_feature->to_string();
		// Make sure the feature string contains no whitespace
		assert(fstr.find(" ") == fstr.npos);
		assert(fstr.find("\t") == fstr.npos);
		assert(fstr.find("\r") == fstr.npos);
		assert(fstr.find("\n") == fstr.npos);

		o << "split on (" << fstr << ")";
	} else {
		assert(m_have_confidence_and_initial_weights);
		o << "Leaf #" << m_id << ": ";
		o << "confidence=" << m_confidence;
		o << " initial_weights=" << m_initial_weights;
	}
	o << " depth=" << m_depth;
	o << "\n";
	return o.str();
}

// TEMPLATE INSTANTIATIONS
template void Node::set_confidence_and_initial_weights<Example>(ExamplePtrs<Example> const&);
template void Node::set_confidence_and_initial_weights<SparseFullExample>(ExamplePtrs<SparseFullExample> const&);
