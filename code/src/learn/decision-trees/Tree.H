/*  $Id: Tree.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Tree.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Tree
 *  \brief A decision tree class.
 *
 *  A decision tree is, essentially, a list of Node%s.
 *  We assume that no leaf splits will be performed on a Tree, unlike a BuildTree.
 *
 */

#ifndef __Tree_H__
#define  __Tree_H__

#include "learn/decision-trees/Node.H"
#include "learn/examples/Example.H"	// REMOVEME

#include <vector>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */
//#include <ext/hash_map>
//#ifndef DOXYGEN
//using namespace __gnu_cxx;
//#endif /* DOXYGEN */

class EnsembleTokenizer;
class BuildTree;
template<typename EXAMPLE> class Examples;

class Tree {
public:
	/// Create an empty tree.
	Tree() { this->initialize(); }

	/// Create a new tree, and weight it.
	/// \param exmpls The source of Example%s used to build
	/// and confidence-rate this hypothesis.
	template<typename EXAMPLE> Tree(const Examples<EXAMPLE>& exmpls) {
		this->initialize();
		this->create(exmpls);
	}

	/// Create a new tree, and weight it.
	/// \param exmpls The source of Example%s used to build
	/// and weight this hypothesis.
	template<typename EXAMPLE> void create(const Examples<EXAMPLE>& exmpls);

	/// Load a tree from an EnsembleTokenizer.
	/// \param lexer The EnsembleTokenizer from which to load the Tree.
	/// \param exmpls The Examples from which to set the initial Weights.
	/// \return True iff the last token we lexed was "Tree" (i.e. there's another Tree to be lexed).
	/// False iff the last token we lexed was EOF.
	/// \todo Allow ourselves to read the Node%s in any order.
	/// \todo Break this up into several functions.
	template<typename EXAMPLE> bool lex(const EnsembleTokenizer& lexer, const Examples<EXAMPLE>& examples);

	/// Convert a tree to a string.
	/// Used for saving hypotheses in Checkpoint.
	string to_string(const string s="") const;

	Tree& operator= (const BuildTree& t);

	/// Find the confidence of some EXAMPLE.
	/// \param e An EXAMPLE.
	/// \return The confidence of that example.
	template<typename EXAMPLE> double confidence(const EXAMPLE& e) const;

	/// Is this a degenerate tree?
	/// \return True if the tree contains no splits, i.e. only a root Node,
	/// or all leaves have confidence below parameter::min_max_leaf_confidence().
	bool is_degenerate() const;

	double penalty() const;

protected:
	/// NodeID of the root Node.
	NodeID root_node;

	/// Scale leaf confidences.
	/// \param alpha Factor by which all leaf confidences are multiplied.
	/// \todo Make this protected.
	void multiply_by(const double alpha);

public:
	/// Find the Node that some EXAMPLE falls into.
	/// \todo Return a NodeID instead of a ptr?
	/// \todo Make this member function of Node?
	/// \todo Make this protected
	/// \todo Make this virtual, don't rewrite for BuildTree
	template<typename EXAMPLE> const Node* find_leaf(const EXAMPLE& e) const;

	/// Find all leaf Node%s in this Tree.
	LIST<const Node*> leaves() const;

	/// Weight the leaves of this tree, and update the Example weights.
	/// \todo Backprune splits that don't reduce loss, and backprune leaves
	/// that don't have enough weight/exmpls to meet the initial splitting
	/// criteria
	/// \todo Make this private.
	template<typename EXAMPLE> void weight_leaves_and_update_examples(Examples<EXAMPLE>& exmpls);

	/// Sanity check that the initial Weights computed from exmpls
	/// match those read from the hypothesis file in this Tree.
	/// \todo It would be much more efficient if we move this to
	/// Ensemble::set_confidences.
	template<typename EXAMPLE> void verify_initial_weights(const Examples<EXAMPLE>& exmpls) const;

private:
	/// Create an empty tree.
	void initialize();

	vector<Node> nodes;
};

#endif /* __Tree_H__ */
