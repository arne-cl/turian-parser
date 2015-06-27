/*  $Id: BuildTree.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file BuildTree.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class BuildTree
 *  \brief A Tree that is currently being built.
 *
 *  This class is inherits from Tree, and has extra members that keep
 *  track of the progress in building the decision tree.
 *
 *  Instead of Node%s, it contains BuildNode%s, and it keeps track of the frontier
 * (i.e.\ leaf BuildNode%s that are not closed).
 *
 *  \todo Backprune splits that don't reduce loss, and backprune leaves
 *  that don't have enough weight/exmpls to meet the initial splitting
 *  criteria.
 *  \todo Clean up the remove_frontier() / close() logic so that it's more intuitive.
 *
 */

#ifndef __BuildTree_H__
#define  __BuildTree_H__

#include "learn/decision-trees/Tree.H"
#include "learn/decision-trees/BuildNode.H"

#include "learn/features/Features.H"

#include "universal/globals.H"

//#include <ext/hash_map>
#include <vector>
#include <ext/hash_set>
#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
#endif /* DOXYGEN */

template<typename EXAMPLE> class Examples;

class BuildTree : public Tree {
public:
  	/// Build an empty tree.
	BuildTree();

	/// Build a tree from scratch.
	template<typename EXAMPLE> BuildTree(const Examples<EXAMPLE>& exmpls);

	/// \todo Rename this to nodes()
	const vector<BuildNode>& nodevec() const { return const_cast<vector<BuildNode>&> (nodes); }

private:
	void initialize();

	NodeID new_node(const NodeID parent, const unsigned depth=0);

	void remove_from_frontier(const NodeID n);

	/// Build the tree.
	template<typename EXAMPLE> void build(const Examples<EXAMPLE>& exmpls);
	template<typename EXAMPLE> void build_work(const Examples<EXAMPLE>& exmpls);

	/// Try to split all the frontier nodes.
	void split_frontier();

	/// Are we done building this tree?
	/// i.e. Are there no open frontier (leaf) nodes?
	bool is_done() const {
		if (frontier.size() == 0)
			assert(nodes.at(root_node).is_closed());
		else
			assert(!nodes.at(root_node).is_closed());
		return (frontier.size() == 0);
	}

	/// Treat one Example.
	/// Determine which BuildNode it falls into, and maybe split or close
	/// that BuildNode.
	template<typename EXAMPLE> void treat(const EXAMPLE& e);

	/// Find the BuildNode that some Example falls into.
	/// \todo In BuildTree::find_leaf, stop if we've reached a closed node?
	/// \todo Return a NodeID instead of a ptr?
	/// \todo Make this member function of Node?
	/// \todo Remove from BuildTree, keep only in Tree?
	template<typename EXAMPLE> BuildNode* find_leaf(const EXAMPLE& e) const;

	/// Close a node.
	/// When possible, we also close the ancestors of this node.
	/// \param n The NodeID of the node to be closed.
	void close(const NodeID n);

	/// Try to split a node.
	/// If we cannot split it, then we close it.
	/// \param n The NodeID of the node we wish to split.
	/// \return True iff a node was split.
	bool split(const NodeID n);

	/// Perform a node split.
	/// \param n The NodeID of the node we are going to split.
	/// \param f The feature with which to split the node.
	/// \todo Check that none of n's parents have used f
	void perform_split(const NodeID n, const ID<Feature>& f);

	vector<BuildNode> nodes;

	hash_set<NodeID, hash<NodeID>, equal_to<NodeID> > frontier;
};

#endif /* __BuildTree_H__ */
