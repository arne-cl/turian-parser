/*  $Id: BuildTree.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file BuildTree.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/decision-trees/Tree.H"
#include "learn/decision-trees/BuildTree.H"

#include "learn/examples/Sampler.H"
#include "learn/examples/Example.H"
#include "learn/examples/FullExample.H"
#include "learn/examples/IntermediateExample.H"
#include "learn/examples/Examples.H"

#include "universal/globals.H"
#include "universal/stats.H"
#include "universal/Debug.H"

/// Build an empty tree.
BuildTree::BuildTree() {
	Debug::log(1) << "BuildTree::BuildTree()...\n";

	this->initialize();

	Debug::log(1) << "...BuildTree::BuildTree()\n";
}

/// Build a tree from scratch.
template<typename EXAMPLE> BuildTree::BuildTree(const Examples<EXAMPLE>& exmpls) {
	Debug::log(1) << "BuildTree::BuildTree(const Examples<" << EXAMPLE::name() << ">& exmpls) at " << stats::current_time() << "...\n";

	this->initialize();

	Debug::log(2) << stats::resource_usage() << "\n";
	if (parameter::downsample_examples_during_feature_selection()) {
		Sampler<EXAMPLE> sample(exmpls, parameter::downsample_examples_during_feature_selection_sample_size());
		this->build(sample);
	} else {
		this->build(exmpls);
	}

	if (parameter::l1_penalty_factor() < parameter::final_l1_penalty_factor()) {
		assert(parameter::vary_l1_penalty());
		return;
	}

	Debug::log(1) << "...BuildTree::BuildTree(const Examples<" << EXAMPLE::name() << ">& exmpls) at " << stats::current_time() << "\n";
	Debug::log(2) << stats::resource_usage() << "\n";
}

/// Initialize the tree.
/// Clear the nodes and frontier, and add a root node.
void BuildTree::initialize() {
	Debug::log(2) << "BuildTree::initialize()...\n";

	nodes.clear();
	frontier.clear();
	root_node = this->new_node(NO_NODE);
	Debug::log(2) << "Root node (#" << root_node << ") created\n";
	assert(root_node != NO_NODE);
	assert(root_node == 0);		// FIXME

	Debug::log(2) << "...BuildTree::initialize()\n";
}

NodeID BuildTree::new_node(const NodeID parent, const unsigned depth) {
	BuildNode n(nodes.size(), parent, depth);
	assert(n.parent() == parent);
	assert(!n.is_closed());
	nodes.push_back(n);
	assert(nodes.back().id() == n.id());
	assert(nodes.size()-1 == n.id());
	assert(nodes.at(nodes.size()-1).id() == n.id());
	frontier.insert(n.id());

	if (depth != 0) Debug::log(2) << "\t";
	Debug::log(2) << "BuildTree::new_node() created Node #" << n.id() << " at depth " << depth;
	if (n.parent() != NO_NODE)
		Debug::log(2) << " with parent #" << n.parent();
	Debug::log(2) << "\n";
	Debug::log(3) << nodes.back().to_string("\t");

	return n.id();
}

void BuildTree::remove_from_frontier(const NodeID n) {
	assert(frontier.find(n) != frontier.end());
	frontier.erase(frontier.find(n));

	Debug::log(3) << "BuildTree::remove_from_frontier() removed node #" << n << " from frontier\n";
	Debug::log(3) << "Frontier now contains " << frontier.size() << " nodes.\n";
}

/// Build a tree from scratch.
/// \todo FIXME!!! If a frontier node was created by an eager split,
/// then at the end of this pass over the examples that node may not have
/// seen all its examples in the pass. (It misses the ones prior to its
/// creation.) Hence, we shouldn't force a split on this frontier node yet!
/// \todo Allow a certain number of initial examples to be discarded?
/// \internal Do we have any reason to tell the example source that we've
/// stopped (potentially prematurely)?
template<typename EXAMPLE> void BuildTree::build(const Examples<EXAMPLE>& exmpls) {
	if (parameter::cache_example_feature_vectors_before_decision_tree_building()) {
		this->build_work(Examples<SparseFullExample>(exmpls));
	} else {
		this->build_work(exmpls);
	}
}
template<> void BuildTree::build(const Examples<SparseFullExample>& exmpls) {
	this->build_work(exmpls);
}

template<typename EXAMPLE> void BuildTree::build_work(const Examples<EXAMPLE>& exmpls) {
	Debug::log(1) << "\nBuildTree::build_work(Examples<" << EXAMPLE::name() << ">)...\n";

	unsigned totcnt = 0;

	Double tot_weight;

	unsigned pass = 0;
	while(!this->is_done()) {
		Debug::log(1) << "Pass #" << pass << " started at " << stats::current_time() << ".\n";

		for(typename Examples<EXAMPLE>::const_iterator ex = exmpls.begin(); ex != exmpls.end() && !this->is_done(); ex++) {
			this->treat(*ex);
			totcnt++;
			tot_weight += ex->weight();
			if (totcnt % 100000 == 0)
				Debug::log(2) << "\tProcessed " << totcnt << " examples with " << frontier.size() << "/" << (nodes.size()+1)/2 << " frontier nodes\n";
			else if (totcnt % 10000 == 0)
				Debug::log(3) << "\tProcessed " << totcnt << " examples with " << frontier.size() << "/" << (nodes.size()+1)/2 << " frontier nodes\n";
			else if (totcnt % 1000 == 0)
				Debug::log(4) << "\tProcessed " << totcnt << " examples with " << frontier.size() << "/" << (nodes.size()+1)/2 << " frontier nodes\n";
		}
		assert(!frontier.empty());
		pass++;
		Debug::log(1) << "Pass #" << pass << " done at " << stats::current_time() << ".\n";

		this->split_frontier();
	}
	assert(frontier.empty());

	Debug::log(1) << "...BuildTree::build_work(Examples<" << EXAMPLE::name() << ">)\n";
}
template<> void BuildTree::build_work(const Examples<Example>& exmpls) {
	this->build_work(Examples<IntermediateExample>(exmpls));
}

/// Try to split all the frontier nodes.
void BuildTree::split_frontier() {
	hash_set<NodeID, hash<NodeID>, equal_to<NodeID> > frontier_copy = frontier;
	hash_set<NodeID, hash<NodeID>, equal_to<NodeID> >::iterator n;
	for (n = frontier_copy.begin(); n != frontier_copy.end(); n++)
		this->split(*n);
}

/// Treat one Example.
/// Determine which BuildNode it falls into, and maybe split or close
/// that BuildNode.
template<typename EXAMPLE> void BuildTree::treat(const EXAMPLE& e) {
	if (Debug::will_log(6)) {
		Debug::log(6) << "\tBuildTree::treat(" << EXAMPLE::name() << "& e)...\n";
		//Debug::log(6) << e.to_string("\t\t");
	}

	// Find the node that e falls into,
	// and pass it the example.
	BuildNode& n = *this->find_leaf(e);
	n.treat(e);

	Debug::log(6) << "\t...BuildTree::treat(" << EXAMPLE::name() << "& e)\n";
}

/// Find the BuildNode that some Example falls into.
/// \todo In BuildTree::find_leaf, stop if we've reached a closed node?
/// \todo Return a NodeID instead of a ptr?
/// \todo Make this member function of Node?
/// \todo Remove from BuildTree, keep only in Tree?
template<typename EXAMPLE> BuildNode* BuildTree::find_leaf(const EXAMPLE& e) const {
	const BuildNode* n = &nodes.at(root_node);
	const BuildNode* newn;

	while (!n->is_leaf()) {
		if (e.has_feature(n->split()))
			newn = &nodes.at(n->pos_child());
		else
			newn = &nodes.at(n->neg_child());
		assert(newn->parent() == n->id());
		n = newn;
	}
	return (BuildNode*)n;
}
template<> BuildNode* BuildTree::find_leaf(const Example& e) const {
	Debug::warning(__FILE__, __LINE__, "Why are you converting the Example here?");
	return this->find_leaf(IntermediateExample(e));
}

/// Close a node.
/// When possible, we also close the ancestors of this node.
/// \param n The NodeID of the node to be closed.
void BuildTree::close(const NodeID n) {
	Debug::log(3) << "BuildTree::close(BuildNode #" << n << ")\n";

	BuildNode& node = nodes.at(n);
	node.close();
	assert(node.id() == n);

	// If n has a parent node, see if both siblings are closed.
	// If so, then we can also close the parent node.
	if (node.parent() != NO_NODE) {
		const BuildNode& p = nodes.at(node.parent());
		assert(p.id() == node.parent());
		if (nodes.at(p.pos_child()).is_closed() &&
				nodes.at(p.neg_child()).is_closed())
			this->close(p.id());

		assert(p.id() == node.parent());
		assert(nodes.at(p.pos_child()).is_closed() ||
				nodes.at(p.neg_child()).is_closed());
	}
}

/// Try to split a node.
/// If we cannot split it, then we close it.
/// \param n The NodeID of the node we wish to split.
/// \return True iff a node was split.
bool BuildTree::split(const NodeID n) {
	Debug::log(6) << "\tBuildTree::split()...\n";
	BuildNode& node = nodes.at(n);

	const ID<Feature>& f = node.best_split();
	if (!f.empty()) {
		this->perform_split(n, f);
		return true;
	} else {
		Debug::log(2) << "Cannot find a split for node #" << n << ". Closing...\n";
		this->close(n);
		this->remove_from_frontier(n);
		return false;
	}
}

/// Perform a node split.
/// \param n The NodeID of node we are going to split.
/// \param f The feature with which to split the node.
/// \todo Check that none of n's parents have used f
/// \todo Debug output l1, old conf, new confs, frontier size
void BuildTree::perform_split(const NodeID n, const ID<Feature>& f) {
	Debug::log(2) << "BuildTree::perform_split()...\n";

	assert(nodes.at(n).id() == n);
	assert(nodes.at(n).is_leaf());

	// WRITEME: Check that none of n's parents have used f

	NodeID pos = this->new_node(n, nodes.at(n).depth()+1);
	NodeID neg = this->new_node(n, nodes.at(n).depth()+1);

	assert(nodes.at(n).depth()+1 == nodes.at(pos).depth());
	assert(nodes.at(n).depth()+1 == nodes.at(neg).depth());

	nodes.at(n).perform_split(f, pos, neg);
	Debug::log(5) << "Split node:\n";
	Debug::log(5) << nodes.at(n).to_string("\t");
	// WRITEME: Debug output split f, l1, old conf, new confs, frontier size
	this->remove_from_frontier(n);

	Debug::log(2) << "...BuildTree::perform_split()\n";

	if (nodes.at(pos).depth() == parameter::max_depth()) {
		assert(nodes.at(neg).depth() == parameter::max_depth());
		this->remove_from_frontier(pos);
		this->remove_from_frontier(neg);
		this->close(pos);
		this->close(neg);
	} else {
		assert(nodes.at(pos).depth() < parameter::max_depth());
		assert(nodes.at(neg).depth() < parameter::max_depth());
	}
}

// TEMPLATE INSTANTIATIONS
template BuildTree::BuildTree(Examples<SparseFullExample> const&);
template BuildTree::BuildTree(Examples<Example> const&);
