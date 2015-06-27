/*  $Id: Tree.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Tree.C
 *  \brief Definition of main Tree functions.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/decision-trees/Tree.H"
#include "learn/decision-trees/BuildTree.H"

#include "learn/examples/Example.H"
#include "learn/examples/IntermediateExample.H"
#include "learn/examples/FullExample.H"
#include "learn/examples/Examples.H"

#include "universal/Debug.H"
#include "universal/stats.H"

/// Create an empty tree.
void Tree::initialize() {
	root_node = NO_NODE;
	nodes.clear();
}

Tree& Tree::operator= (const BuildTree& t) {
	assert(t.root_node == 0);		// FIXME
	this->root_node = t.root_node;

	this->nodes.clear();
	vector<BuildNode>::const_iterator n;
	for (n = t.nodevec().begin(); n != t.nodevec().end(); n++) {
		this->nodes.push_back(*n);	// FIXME: Don't use [] operator
		assert(n->id() == this->nodes.back().id());
		assert(this->nodes.at(n->id()).id() == this->nodes.back().id());
		assert(this->nodes.size() == this->nodes.back().id()+1);
	}

	return *this;
}

/// Find the confidence of some EXAMPLE.
/// \param e An EXAMPLE.
/// \return The confidence of that example.
template<typename EXAMPLE> double Tree::confidence(const EXAMPLE& e) const {
	return this->find_leaf(e)->confidence();
}
template<> double Tree::confidence(const Example& e) const {
	Debug::warning(__FILE__, __LINE__, "Why are you converting from an Example here?");
	return this->confidence(IntermediateExample(e));
}

/// Find the Node that some EXAMPLE falls into.
/// \todo Return a NodeID instead of a ptr?
/// \todo Make this member function of Node?
/// \todo Make this protected
/// \todo Make this virtual, don't rewrite for BuildTree
template<typename EXAMPLE> const Node* Tree::find_leaf(const EXAMPLE& e) const {
	const Node* n = &nodes.at(root_node);
	const Node* newn;

	while (!n->is_leaf()) {
		if (e.has_feature(n->split())) {
			if (parameter::trace_confidence_paths())
				Debug::log(0) << "\tEXAMPLE has feature " << n->split()->to_string() << "\n";
			newn = &nodes.at(n->pos_child());
		} else {
			if (parameter::trace_confidence_paths())
				Debug::log(0) << "\tEXAMPLE does not have feature " << n->split()->to_string() << "\n";
			newn = &nodes.at(n->neg_child());
		}
		assert(newn->parent() == n->id());
		n = newn;
	}
	return n;
}
template<> const Node* Tree::find_leaf(const Example& e) const {
	Debug::warning(__FILE__, __LINE__, "Why are you converting from an Example here?");
	return this->find_leaf(IntermediateExample(e));
}

/// Scale leaf confidences.
/// \param alpha Factor by which all leaf confidences are multiplied.
/// \todo Make this protected.
void Tree::multiply_by(const double alpha) {
	vector<Node>::iterator n;
	for (n = nodes.begin(); n != nodes.end(); n++)
		n->multiply_by(alpha);
}

/// Is this a degenerate Decision Tree?
/// \return True if the tree contains no splits, i.e. only a root Node,
/// or all leaves have confidence below parameter::confidence_epsilon().
bool Tree::is_degenerate() const {
	assert(nodes.size() > 0);

	/// - Check if no leaf has higher confidence than
	/// parameter::confidence_epsilon().
	bool allzero = true;
	vector<Node>::const_iterator n;
	for (n = nodes.begin(); n != nodes.end(); n++) {
		if (!n->is_leaf()) continue;
		if (fabs(n->confidence()) > fabs(parameter::confidence_epsilon())) {
			allzero = false;
			break;
		}
	}
	if (allzero)
		return true;

	return nodes.size() == 1;
}

double Tree::penalty() const {
	double p = 0;

	vector<Node>::const_iterator n;
	for (n = nodes.begin(); n != nodes.end(); n++) {
		if (!n->is_leaf()) continue;
		p += n->penalty();
	}
	return p;
}

/// Convert a Tree to a string.
/// Used for saving hypotheses in Checkpoint.
string Tree::to_string(const string s) const {
	vector<Node>::const_iterator n;
	string o;
	for (n = nodes.begin(); n != nodes.end(); n++)
		o += n->to_string(s);
	return o;
}

/// Create a new Tree, and weight it.
/// \param exmpls The source of Example%s used to build
/// and weight this hypothesis.
template<typename EXAMPLE> void Tree::create(const Examples<EXAMPLE>& exmpls) {
	Debug::log(1) << "Tree::create()...\n";

	// Build a tree.
	BuildTree buildtree(exmpls);

	// Save the newly created tree.
	*this = buildtree;

	if (parameter::l1_penalty_factor() < parameter::final_l1_penalty_factor()) return;

	Debug::log(1) << "...Tree::create()\n";
}

/// Find all leaf Node%s in this Tree.
LIST<const Node*> Tree::leaves() const {
	LIST<const Node*> leaves;

	vector<Node>::const_iterator n;
	for (n = nodes.begin(); n != nodes.end(); n++) {
		if (n->is_leaf())
			leaves.insert(leaves.end(), &(*n));
	}
	return leaves;
}


/// Weight the leaves of this tree, and update the Example weights.
/// \todo Backprune splits that don't reduce loss, and backprune leaves
/// that don't have enough weight/exmpls to meet the initial splitting
/// criteria
/// \todo Weight the internal nodes too, for debugging purposes?
template<typename EXAMPLE> void Tree::weight_leaves_and_update_examples(Examples<EXAMPLE>& exmpls) {
	Debug::log(1) << "\nTree::weight_leaves_and_update_examples(Examples<" << EXAMPLE::name() << ">)...\n";

	vector<Node>::iterator n;
	Double orig_total_weight;

	hash_map<NodeID, set<ID<Sentence> > > sentences;
	hash_map<NodeID, ExamplePtrs<EXAMPLE> > leaves;

	unsigned totcnt = 0;
	for(typename Examples<EXAMPLE>::iterator ex = exmpls.begin(); ex != exmpls.end(); ex++) {
		// Find the node that e falls into.
		const Node* n = this->find_leaf(*ex);
		assert(n->is_leaf());

		assert(n->id() != NO_NODE);
		sentences[n->id()].insert(ex->sentence());
		sentences[NO_NODE].insert(ex->sentence());

		leaves[n->id()].push_back(&(*ex));

		// FIXME: This won't work if there's noise
		orig_total_weight += ex->weight();

		totcnt++;
		if (totcnt % 100000 == 0)
			Debug::log(3) << "\tProcessed " << totcnt << " examples in Tree::weight_leaves()\n";
		if (totcnt % 10000 == 0)
			Debug::log(4) << "\tProcessed " << totcnt << " examples in Tree::weight_leaves()\n";
	}

	Debug::log(2) << "Done processing " << totcnt << " examples in Tree::weight_leaves()\n";

	// Compute the confidence for each leaf.
	unsigned leafcnt = 0;
	unsigned sentence_cnt = 0;
	unsigned example_cnt = 0;
	for (n = nodes.begin(); n != nodes.end(); n++) {
		if (n->is_leaf()) {
			assert(leaves.find(n->id()) != leaves.end());

			const ExamplePtrs<EXAMPLE>& leaf_examples = leaves.find(n->id())->second;
			double orig_unpenalized_loss = leaf_examples.unpenalized_loss();
			Weights initial_weight = leaf_examples.initial_weight();

			example_cnt += leaf_examples.size();
			n->set_confidence_and_initial_weights(leaf_examples);

			// Add the leaf confidence to the leaf Examples.
			leaves.find(n->id())->second.add_confidence(n->confidence());

			// Update the confidence of this leaf's Example%s.
			double unpenalized_loss = leaf_examples.unpenalized_loss();


			leafcnt++;
//			Debug::log(2) << "Weighted leaf:\n";
//			Debug::log(2) << n->to_string("\t");
			Debug::log(2) << n->to_string();
			assert(sentences.find(n->id()) != sentences.end());
			sentence_cnt += sentences.find(n->id())->second.size();
			Debug::log(2) << "\t" << leaf_examples.size() << " examples from ";
			Debug::log(2) << sentences.find(n->id())->second.size() << " different sentences\n";

			double penalty = n->penalty();
			Debug::log(2) << "\tloss = " << unpenalized_loss + penalty << " = " << unpenalized_loss << " (unpenalized loss) + " << penalty << " (penalty)";
			if (n->confidence() != 0) Debug::log(2) << "  (conf=0 loss was " << orig_unpenalized_loss << ")";
			Debug::log(2) << "\n";
		}
	}
	assert(example_cnt == exmpls.size());
	Debug::log(2) << "Examples from " << sentences[NO_NODE].size() << " different sentences.\n";

	Debug::log(2) << "Done weighting " << leafcnt << " leaves in Tree::weight_leaves()\n";
	Debug::log(2) << stats::resource_usage() << "\n";

	// WRITEME: Backprune splits that don't reduce loss

	Debug::log(1) << "...Tree::weight_leaves_and_update_examples(Examples<" << EXAMPLE::name() << ">)\n";
}

/// Sanity check that the initial Weights computed from exmpls
/// match those read from the hypothesis file in this Tree.
/// \todo It would be much more efficient if we move this to
/// Ensemble::set_confidences.
template<typename EXAMPLE> void Tree::verify_initial_weights(const Examples<EXAMPLE>& exmpls) const {
	hash_map<NodeID, Weights> initial_weights;
	for(typename Examples<EXAMPLE>::const_iterator e = exmpls.begin(); e != exmpls.end(); e++) {
		const Node& n = *this->find_leaf(*e);
		initial_weights[n.id()].add(e->initial_weight(), e->is_correct());
	}
	for (vector<Node>::const_iterator n = nodes.begin(); n != nodes.end(); n++) {
		if (!n->is_leaf()) continue;
//		assert(fabs(n->initial_weights().pos()() - initial_weights[n->id()].pos()()) < parameter::small_epsilon());
//		assert(fabs(n->initial_weights().neg()() - initial_weights[n->id()].neg()()) < parameter::small_epsilon());
		if (fabs(n->initial_weights().pos()() - initial_weights[n->id()].pos()()) > parameter::small_epsilon()) {
			ostringstream o;
			o << "|read initial posweight - calc initial posweight| = " << fabs(n->initial_weights().pos()() - initial_weights[n->id()].pos()());
			Debug::warning(__FILE__, __LINE__, o.str());
		}
		if (fabs(n->initial_weights().neg()() - initial_weights[n->id()].neg()()) > parameter::small_epsilon()) {
			ostringstream o;
			o << "|read initial negweight - calc initial negweight| = " << fabs(n->initial_weights().neg()() - initial_weights[n->id()].neg()());
			Debug::warning(__FILE__, __LINE__, o.str());
		}
	}
}

// TEMPLATE INSTANTIATIONS
template double Tree::confidence(const IntermediateExample& e) const;
template double Tree::confidence(const SparseFullExample &) const;
template double Tree::confidence(const DenseFullExample &) const;
template void Tree::create(Examples<Example> const&);
template void Tree::create(Examples<SparseFullExample> const&);
template void Tree::verify_initial_weights(Examples<Example> const&) const;
template void Tree::verify_initial_weights(Examples<SparseFullExample> const&) const;
template void Tree::weight_leaves_and_update_examples(Examples<Example>&);
template void Tree::weight_leaves_and_update_examples(Examples<SparseFullExample>&);
