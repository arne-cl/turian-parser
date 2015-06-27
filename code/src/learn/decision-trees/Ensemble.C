/*  $Id: Ensemble.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Ensemble.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/decision-trees/Ensemble.H"
#include "learn/decision-trees/EnsembleTokenizer.H"
#include "learn/decision-trees/BuildNode.H"

#include "learn/examples/FullExample.H"
#include "learn/examples/IntermediateExample.H"
#include "learn/examples/Examples.H"

#include "learn/Checkpoint.H"
#include "learn/examples/Example.H"
#include "learn/loss.H"

#include "universal/Debug.H"
#include "universal/stats.H"

/// Create an empty ensemble.
Ensemble::Ensemble() {
	this->clear();
}

void Ensemble::clear() {
	_trees.clear();
	_use_full_examples.clear();
}

const string Ensemble::to_string(const string s) const {
	string o;

	o += "use_full_examples = ";
	if (_use_full_examples.empty())
		o += "empty";
	else if (_use_full_examples())
		o += "true";
	else
		o += "false";
	o += "\n";

	LIST<Tree>::const_iterator t;
	for (t = _trees.begin(); t != _trees.end(); t++) {
		o += s + "Tree\n";
		o += s + t->to_string("\t");
	}
	return o;
}

/// Load an ensemble from a file, and set Example confidences.
/// \param filename The file from which to load the ensemble.
void Ensemble::load(const string filename) {
	Examples<Example> exmpls;
	this->load(filename, exmpls);
}

/// Load an ensemble from a file.
/// \param filename The file from which to load the ensemble.
/// \param exmpls The Example%s whose confidences should be updated,
/// and from whom the initial weights should be computed.
template<typename EXAMPLE> void Ensemble::load(const string filename, Examples<EXAMPLE>& exmpls) {
	this->clear();

	EnsembleTokenizer lexer;
	lexer.open((char*)filename.c_str());

	Debug::log(2) << stats::resource_usage() << "\n";

	bool more_trees = true;
	bool got_use_full = false;

	// We'll use this to set ::_use_full_examples *after* all the trees are added.
	Optional<bool> use_full_examples;

	token tok = lexer.get();
	switch(tok) {
		case USE_FULL_EXAMPLES_TOK:
			got_use_full = true;
			break;
		case TREE_TOK:
			break;
		case END_TOK:
			more_trees = false;
			break;
		default: assert(0);
	}

	if (got_use_full) {
		lexer.want(EQ_TOK);
		switch (lexer.get()) {
			case TRUE_TOK:
				use_full_examples = true;
				Debug::log(2) << "Ensemble tuned to use DenseFullExamples\n";
				break;
			case FALSE_TOK:
				use_full_examples = false;
				Debug::log(2) << "Ensemble tuned to use IntermediateExamples\n";
				break;
			case EMPTY_TOK:
				use_full_examples.clear();
				Debug::warning(__FILE__, __LINE__, "Ensemble not tuned.");
				break;
			default: assert(0);
		}
		more_trees = lexer.want(TREE_TOK);
	} else {
		Debug::warning(__FILE__, __LINE__, "Ensemble does not contain 'use_full_examples' value.");
		use_full_examples.clear();
	}

	while (more_trees) {
		Tree t;
		more_trees = t.lex(lexer, exmpls);
		this->add(t);
	}

	lexer.close();

	_use_full_examples = use_full_examples;
	Debug::log(1) << "Read " << _trees.size() << " trees from: " << filename << "\n\n";

	this->set_confidences(exmpls);
}

/// Add a Tree to the ensemble.
/// \param t The Tree to be added.
/// \post _use_full_examples.empty()
void Ensemble::add(const Tree& t) {
	_trees.push_back(t);
	_use_full_examples.clear();
}

/// Compute the penalty over all Tree%s in this Ensemble.
double Ensemble::penalty() const {
	double penalty = 0;
	for(LIST<Tree>::const_iterator t = _trees.begin(); t != _trees.end(); t++)
		penalty += t->penalty();
	return penalty;
}

/// Determine statistics of this Ensemble and a set of Examples,
/// and return them in a string.
template<typename EXAMPLE> string Ensemble::stats(const Examples<EXAMPLE>& exmpls) const {
	ostringstream o;
	double unpenalized_loss = exmpls.unpenalized_loss();
	double penalty = this->penalty();
	o << "Total loss = " << unpenalized_loss + penalty << " = " << unpenalized_loss << " (unpenalized loss) + " << penalty << " (penalty), current weight = " << exmpls.current_weight() << ", over " << exmpls.size() << " examples";
	return o.str();
}

/// Sanity check that the initial Weights computed from exmpls
/// match those read from the hypothesis file in this Ensemble.
/// \todo It would be much more efficient if we move this to
/// Ensemble::set_confidences.
template<typename EXAMPLE> void Ensemble::verify_initial_weights(const Examples<EXAMPLE>& exmpls) const {
	Debug::log(2) << "Verifying initial weights in ensemble...\n";

	// Only verify all trees if sanity checks are enabled.
	unsigned verify_cnt = 0;
#ifdef SANITY_CHECKS
	verify_cnt = _trees.size();
#else
	verify_cnt = TREES_TO_VERIFY_INITIAL_WEIGHT;
#endif

	unsigned i = 0;
	for(LIST<Tree>::const_iterator t = _trees.begin();
			t != _trees.end() && i < verify_cnt; t++) {
		t->verify_initial_weights(exmpls);
		i++;
	}

	Debug::log(2) << "...done verifying initial weights over " << i << " trees in the ensemble\n";
}

/// Add the tree to the ensemble, choose confidences for
/// all the tree's leaves using the example set, and update
/// the example weights.
/// \param t The Tree to be added.
/// \param exmpls The Example%s whose confidences should be updated.
/// \post _use_full_examples.empty()
template<typename EXAMPLE> void Ensemble::add_and_update(Tree& tree, Examples<EXAMPLE>& exmpls) {
	_use_full_examples.clear();

        tree.weight_leaves_and_update_examples(exmpls);

	// If this is a degenerate hypothesis, then decrease the l1 penalty.
	if (tree.is_degenerate()) {
		assert(parameter::vary_l1_penalty());
		assert(parameter::l1_penalty_factor_overscale() < 1);
		parameter::set_l1_penalty_factor(parameter::l1_penalty_factor()*parameter::l1_penalty_factor_overscale());
		Debug::log(1) << "Last hypothesis was degenerate hypothesis. l1 penalty -> " << parameter::l1_penalty_factor() << "\n";
		return;
	}

	this->add(tree);

#ifdef SANITY_CHECKS
	for(typename Examples<EXAMPLE>::iterator e = exmpls.begin(); e != exmpls.end(); e++) {
		// Doublecheck that the cached confidence is the same as the computed confidence.
		assert(fabs(e->confidence() - this->confidence_compute(*e)) < parameter::small_epsilon());
	}
#endif /* SANITY_CHECKS */

	Debug::log(2) << "Added a tree to the ensemble. " << this->stats(exmpls) << "\n";
}

/// Find all leaf Node%s in this Ensemble.
LIST<const Node*> Ensemble::leaves() const {
	LIST<const Node*> leaves;

	LIST<Tree>::const_iterator t;
	for (t = _trees.begin(); t != _trees.end(); t++) {
		LIST<const Node*> l = t->leaves();
		leaves.insert(leaves.end(), l.begin(), l.end());
	}
	return leaves;
}

/// Set Example confidences.
/// \param exmpls The Example%s whose confidences should be updated.
template<typename EXAMPLE> void Ensemble::set_confidences(Examples<EXAMPLE>& exmpls) const {
	if (exmpls.empty()) return;

	/// Make sure the Ensemble has been tuned first.
	this->tune(const_cast<const Examples<EXAMPLE> &>(exmpls));

	Debug::log(2) << "Setting confidences for " << exmpls.size() << " examples...\n";

	for(typename Examples<EXAMPLE>::iterator e = exmpls.begin();
			e != exmpls.end(); e++)
		e->set_confidence(this->confidence_compute(*e));

	Debug::log(2) << "...done setting confidences for " << exmpls.size() << " examples\n";
	Debug::log(2) << this->stats(exmpls) << "\n";
	Debug::log(2) << stats::resource_usage() << "\n\n";
}

/// Given a Example that has been converted to the correct
/// Example type, actually do the work of comput its confidence.
template<typename EXAMPLE> double Ensemble::confidence_compute_work(const EXAMPLE& e) const {
	LIST<Tree>::const_iterator t;
	double conf = 0;
	unsigned i;
	for (t = _trees.begin(), i = 1; t != _trees.end(); t++, i++) {
		double this_conf = t->confidence(e);
		if (parameter::trace_confidence_paths())
			Debug::log(0) << "Tree #" << i << " adds confidence " << this_conf << "\n\n";
		conf += this_conf;
	}
	return conf;
}

// TEMPLATE INSTANTIATIONS
template string Ensemble::stats(const Examples<DenseFullExample>& exmpls) const;
template void Ensemble::verify_initial_weights(const Examples<Example>& exmpls) const;
template void Ensemble::verify_initial_weights(Examples<SparseFullExample> const&) const;
template void Ensemble::add_and_update<SparseFullExample>(Tree&, Examples<SparseFullExample>&);
template void Ensemble::add_and_update<Example>(Tree&, Examples<Example>&);
template double Ensemble::confidence_compute_work<DenseFullExample>(const DenseFullExample& e) const;
template double Ensemble::confidence_compute_work<IntermediateExample>(const IntermediateExample& e) const;

template void Ensemble::set_confidences(Examples<SparseFullExample>& exmpls) const;	// REMOVEME!
