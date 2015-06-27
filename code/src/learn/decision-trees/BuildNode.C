/*  $Id: BuildNode.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file BuildNode.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/decision-trees/BuildNode.H"
#include "learn/decision-trees/BuildTree.H"

#include "learn/examples/Example.H"
#include "learn/examples/FullExample.H"
#include "learn/examples/IntermediateExample.H"

#include "learn/Checkpoint.H"

#include "universal/globals.H"
#include "universal/Debug.H"
#include "universal/Tiebreak.H"

BuildNode::BuildNode() :
	Node(),
	m_is_closed(false),
	m_total_weight(Weights())
{ }

/// Create a BuildNode with a parent.
/// \param newid The NodeID to assign to this new node.
/// \param parent The NodeID of the parent of the node about to be constructed.
/// \param depth The depth of the node about to be constructed.
/// NULL iff this is a root node.
BuildNode::BuildNode(const NodeID newid, const NodeID parent, const unsigned depth) :
	Node(newid, parent, depth),
	m_is_closed(false),
	m_total_weight(Weights()),
	m_weight(vector<Weights>(Features::instance()->count(), Weights()))
{
	if (parameter::min_sentences_per_feature() > 1)
		m_sentences = vector<set<ID<Sentence> > >(Features::instance()->count(), set<ID<Sentence> >());
}

/// Free the memory used by this object.
void BuildNode::free_memory() {
	// Under the g++'s I've tested, this hack can be used to shrink the
	// capacity of a vector to 0.
	// cf. <1106167657.974039.305900@c13g2000cwb.googlegroups.com>
	// and subsequent discussion.

	// Postscript: I've learned the hard way that, at the very least,
	// gcc 4.0.1 on Solaris will leak memory unless you first clear
	// the vector.
	m_weight.clear();
	m_sentences.clear();

	vector<Weights>().swap(m_weight);
	vector<set<ID<Sentence> > >().swap(m_sentences);
}

/// \todo Explicitly compute loss from Wtot
string BuildNode::to_string(const string s) {
	ostringstream o;
	o << s << "BuildNode #" << m_id << " (";
	if (this->is_root()) o << "root, ";
	else o << "parent #" << m_parent << ", ";
	if (m_pos_child == NO_NODE) {
		assert(m_neg_child == NO_NODE);
		assert(m_split_feature == NULL);
		o << "leaf)\n";
	} else {
		assert(m_neg_child != NO_NODE);
		assert(m_split_feature != NULL);
		o << "+child #" << m_pos_child << ", ";
		o << "-child #" << m_neg_child << ")\n";
	}
	o << s << "depth = " << m_depth << "\n";

	if (this->is_leaf()) {
		if (m_have_confidence_and_initial_weights) {
			o << s << "confidence=" << this->confidence() << "\n";
			o << s << "initial_weight=" << m_initial_weights << "\n";
		}
	} else {
		assert(m_split_feature != NULL);
		o << s << "split_feature = " << m_split_feature->to_string() << "\n";
	}
	return o.str();
}

/// Treat one training example.
/// Determine if an example is weighty enough to expand its feature
/// vector and be accumulated in the split feature weights.
/// Otherwise, we skip it.
template<typename EXAMPLE> void BuildNode::treat(const EXAMPLE& e) {
	// If this node is no longer being split, just
	// return.
	if (this->is_closed())
		return;

	// Otherwise accumulate e in n
	this->add_features(e);
}

template<> void BuildNode::add_features(const IntermediateExample& e) {
	this->add_weight(e.flist(), e.is_correct(), e.weight(), e.sentence());
}
template<> void BuildNode::add_features(const SparseFullExample& e) {
	this->add_weight(e.fset(), e.is_correct(), e.weight(), e.sentence());
}
template<> void BuildNode::add_features(const Example& e) {
	Debug::warning(__FILE__, __LINE__, "Why are you converting the Example here?");
	this->add_features(IntermediateExample(e));
}

template<typename FS> void BuildNode::add_weight(const FS& fs, bool is_correct, const Double& w, const ID<Sentence>& s) {
	m_total_weight.add(w, is_correct);

	assert(!fs.empty());
	for (typename FS::const_iterator f = fs.begin(); f != fs.end(); ++f) {
		unsigned i = (*f)();
		m_weight.at(i).add(w, is_correct);
		if (parameter::min_sentences_per_feature() > 1 && m_sentences.at(i).size() < parameter::min_sentences_per_feature())
			m_sentences.at(i).insert(s);
	}
}

/// Find the best splitting feature for this node.
/// Any non-degenerate splitting feature is subject to the following constraints:
///	- The positive child must have examples from at least
///	parameter::min_sentences_per_feature() different sentences.
///	NOTE: We do *not* do this test for the negative leaf!
///	- One child of the split must have gain at least parameter::l1_penalty_factor()
ID<Feature> BuildNode::best_split() {
	Debug::log(4) << "BuildNode::best_split()...\n";
//	// FIXME: Call these assertions!
//	assert(log_add(m_total_weight) > LOG_ZERO);
//	assert(totcnt >= 2);

	/// \todo Write feature count normalization code.
	assert(!parameter::normalize_feature_counts());

	Tiebreak<Double> orig_gain(m_total_weight.penalized_gain());
	Tiebreak<Double> max_gain(orig_gain());
	Tiebreak<Double> min_gain(orig_gain() * parameter::min_gain_factor_for_split());

	if (parameter::min_sentences_per_feature() > 1)
		assert(m_weight.size() == m_sentences.size());

	ID<Feature> bestf;
	assert(bestf.empty());

	vector<Weights>::const_iterator w;
	vector<set<ID<Sentence> > >::const_iterator s;
	unsigned f;
	for (w = m_weight.begin(), s = m_sentences.begin(), f = 0;
			w != m_weight.end(); w++, s++, f++) {
		if (parameter::min_sentences_per_feature() > 1)
			assert(s != m_sentences.end());

		// If this feature is too rare, then skip it.
		if (parameter::min_sentences_per_feature() > 1 && s->size() < parameter::min_sentences_per_feature())
			continue;

		const Weights& pos_weight = *w;

		// The example weight that falls in the negative leaf child
		Weights neg_weight = m_total_weight - pos_weight;

		if (pos_weight.total() == 0) continue;
		if (neg_weight.total() == 0) continue;

		Tiebreak<Double> split_gain(pos_weight.penalized_gain()
				+ neg_weight.penalized_gain());

		// However, if there is no split gain and this root split, then the split gain is
		// the maximum gain of either leaf.
		if (split_gain() == 0 && this->is_root()) {
			// \todo In the case they are equal, then add them.
			split_gain.set(max(pos_weight.gain(), neg_weight.gain()));
			assert(split_gain() > 0);
		}

		// If this is a non-root split, then don't allow zero gain.
		if (split_gain() == 0) continue;

		// Keep this as the best feature if the split's gain beats
		// the previous best gain, as well as the minimum gain for
		// splitting this node (unless this node is the root node).
		if (split_gain > max_gain && (split_gain > min_gain || this->is_root())) {
			max_gain = split_gain;
			bestf = f;
		}
	}
	assert(w == m_weight.end());
	if (parameter::min_sentences_per_feature() > 1)
		assert(s == m_sentences.end());
	assert(f == m_weight.size());

	if (!bestf.empty()) {
		if (this->is_root() && max_gain()*parameter::l1_penalty_factor_overscale() < parameter::l1_penalty_factor()) {
			try {
				parameter::set_l1_penalty_factor((max_gain() * parameter::l1_penalty_factor_overscale())());
			} catch (string s) {
				cerr << "CAUGHT EXCEPTION: " << s << "\n";
				//cerr << "CHECKPOINTING AND RE-THROWING...\n";
				cerr << "CHECKPOINTING AND EXITING...\n";
				Checkpoint::instance()->save();
				exit(0);
				//throw s;
			}
			assert(parameter::l1_penalty_factor() >= parameter::final_l1_penalty_factor());
			Checkpoint::instance()->save();
		}

		const Weights& max_pos_weight = m_weight.at(bestf());
		const Weights max_neg_weight = m_total_weight - max_pos_weight;

		Debug::log(2) << "Node #" << m_id << ":\n";
		Debug::log(3) << "\taccumulated weight = " << m_total_weight << "\n";
		Debug::log(2) << "\tsplit feature: " << Features::instance()->from_id(bestf).to_string() << "\n";
		Debug::log(2) << "\tbest gain / unsplit gain = " << max_gain() << " / " << orig_gain() << " = " << max_gain()/orig_gain() << "\n";
		Debug::log(2) << "\t+child penalized gain " << max_pos_weight.penalized_gain() << " (unclipped=" << max_pos_weight.gain() << ") with weights " << max_pos_weight << "\n";
		Debug::log(2) << "\t-child penalized gain " << max_neg_weight.penalized_gain() << " (unclipped=" << max_neg_weight.gain() << ") with weights " << max_neg_weight << "\n";
		//Debug::log(2) << "\tbest gain = " << max_gain() << "\n";
		//Debug::log(2) << "\t+child gain " << max_pos_weight.gain() << " with weights " << max_pos_weight << "\n";
		//Debug::log(2) << "\t-child gain " << max_neg_weight.gain() << " with weights " << max_neg_weight << "\n";
		//Debug::log(2) << "\tvs. unsplit gain " << orig_gain() << " with weights " << m_total_weight << "\n";
	} else if (bestf.empty()) {
		Debug::log(3) << "No split can for Node #" << m_id << " can beat l1 penalty " << parameter::l1_penalty_factor() << "\n";
	}

	Debug::log(4) << "...BuildNode::best_split()\n";

	return bestf;
}

// TEMPLATE INSTANTIATIONS
//template void BuildNode::treat<Example>(Example const&);
template void BuildNode::treat<SparseFullExample>(SparseFullExample const&);
template void BuildNode::treat<IntermediateExample>(IntermediateExample const&);
