/*  $Id: Treelex.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Treelex.C
 *  \brief Definition of Tree::lex().
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/decision-trees/Tree.H"
#include "learn/decision-trees/EnsembleTokenizer.H"

#include "learn/examples/Example.H"
#include "learn/examples/Examples.H"

#include "universal/Debug.H"

/// Load a Tree from an EnsembleTokenizer.
/// \param lexer The EnsembleTokenizer from which to load the Tree.
/// \param exmpls The Examples from which to set the initial Weights.
/// \return True iff the last token we lexed was "Tree" (i.e. there's another Tree to be lexed).
/// False iff the last token we lexed was EOF.
/// \todo Allow ourselves to read the nodes in any order.
/// \todo Break this up into several functions.
template<typename EXAMPLE> bool Tree::lex(const EnsembleTokenizer& lexer, const Examples<EXAMPLE>& exmpls) {
	this->initialize();

	NodeID i, pos, neg;
	ID<Feature> f;
	unsigned depth;
	double conf;

	double wpos, wneg;

	hash_map<NodeID, NodeID, hash<NodeID>, equal_to<NodeID> > parents;

	bool done = false;
	bool got_eof = false;
	string fstr;
	// Keep reading nodes until we hit EOF
	while (!done) {
		switch(lexer.get()) {
			case NODE_TOK:
				lexer.want(HASH_TOK);
				i = lexer.get_int();
				lexer.want(COLON_TOK);

				lexer.want(POSCHILD_TOK);
				lexer.want(EQ_TOK);
				pos = lexer.get_int();
				lexer.want(NEGCHILD_TOK);
				lexer.want(EQ_TOK);
				neg = lexer.get_int();

				lexer.want(SPLITON_TOK);
				lexer.want(TEXT_TOK);
				fstr = lexer.current_yytext();
				f = Features::instance()->of_string(fstr.substr(1, fstr.length()-2));

				lexer.want(DEPTH_TOK);
				lexer.want(EQ_TOK);
				depth = lexer.get_int();

//				// REMOVEME:
//				TRACE;
//				if (startid == NO_NODE) startid = i;
//				i -= startid; pos -= startid; neg -= startid;

				assert(nodes.size() == i);
				nodes.push_back(Node(i, pos, neg, f, depth));
				assert(nodes.back().id() == i);
				assert(nodes.at(i).id() == i);
				assert(nodes.size() == i+1);
				parents[pos] = i;
				parents[neg] = i;

				break;

			case LEAF_TOK:
				lexer.want(HASH_TOK);
				i = lexer.get_int();
				lexer.want(COLON_TOK);

				lexer.want(CONF_TOK);
				lexer.want(EQ_TOK);
				conf = lexer.get_float();

				lexer.want(INITIAL_WEIGHTS_TOK);
				lexer.want(EQ_TOK);
				lexer.want(LSQUARE_BRACKET_TOK);
				wpos = lexer.get_float();
				lexer.want(COMMA_TOK);
				wneg = -1 * lexer.get_float();
				lexer.want(RSQUARE_BRACKET_TOK);
				assert(wpos >= 0);
				assert(wneg >= 0);

				lexer.want(DEPTH_TOK);
				lexer.want(EQ_TOK);
				depth = lexer.get_int();

				assert(nodes.size() == i);
				nodes.push_back(Node(i, conf, Weights(wneg, wpos), depth));
				assert(nodes.back().id() == i);
				assert(nodes.at(i).id() == i);
				assert(nodes.size() == i+1);

				break;

			case TREE_TOK:
				done = true;
				got_eof = false;
				break;

			case END_TOK:
				assert(parameter::need_END_token());
				done = true;
				lexer.want(EOF_TOK);
				got_eof = true;
				break;

			case EOF_TOK:
				assert(!parameter::need_END_token());
				done = true;
				got_eof = true;
				break;

			default: assert(0);
		}
	}

	// Add backpointers for all nodes
	vector<Node>::iterator n;
	for (n = nodes.begin(), i=0; n != nodes.end(); n++, i++) {
		assert (&nodes.at(i) == &(*n));
		assert (n->id() == i);
		assert (nodes.at(i).id() == i);
		// If we cannot find a parent for this node,
		// assume it's the root
		if (parents.find(n->id()) == parents.end()) {
//			Debug::log(5) << "Root node = " << n->first << "\n";
			assert(root_node == NO_NODE);
			root_node = n->id();
			assert(n->depth() == 0);
			assert(root_node == 0);		// FIXME
		} else {
//			Debug::log(5) << "Parent of " << n->first << " is " << parents[n->first] << "\n";
			// Rewrite: Don't use [] operator
			n->set_parent(parents[n->id()]);
			assert(n->depth() == nodes.at(parents[n->id()]).depth()+1);
			parents.erase(parents.find(n->id()));
		}
	}

	// Make sure that we've used up every parent pointer.
	// (Otherwise, some children nodes are missing.)
	assert(parents.size() == 0);

	return (!got_eof);
}

// TEMPLATE INSTANTIATIONS
template bool Tree::lex<Example>(EnsembleTokenizer const&, Examples<Example> const&);
