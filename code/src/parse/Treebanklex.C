/*  $Id: Treebanklex.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Treebanklex.C
 *  \brief Implementation of Treebank lex methods.
 *
 *  These methods read in Penn Treebank-style (bracketed) parse trees.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#include "parse/Treebank.H"
#include "parse/Parser.H"
#include "parse/PennTokenizer.H"
#include "parse/State.H"

#include "universal/Debug.H"
#include "universal/globals.H" // REMOVEME

#include <sstream>

/// Input a sentence (i.e. tree) in the Penn Treebank format.
/// \param lexer A tokenizer for the desired input stream.
/// \return True iff we read a tree (false iff EOF).
/// \todo WRITEME Do something with parse path!
/// \todo Determine whether we want to lex a path, given a global variable.
bool Treebank::lex_sentence(const PennTokenizer &lexer) {
	switch (lexer.get()) {
		case LPAREN_TOK: break;
		case END_TOK: return false;
		default: assert(0);
	}

	// Create a sentence as an empty state.
	sentences.push_back(State());

	// Create a new ID for this sentence.
	ID<Sentence> s;
	s.create();

	this->lex_item(lexer, 0, s);

	// Make sure the chart has as many new actions
	// as there are in the new state's path.

//	// Try constructing sentence as sanity check.
//	Parser sentence(sentences.back());

	if (parameter::treebank_has_parse_paths()) {
		// Read in the parse path
		unsigned l = sentences.back().path().size();
		this->lex_path(lexer, sentences.back());
		assert(l == sentences.back().path().size());
	}

	return true;
}

/// Read a node from the given lexer, and recursively read its children.
/// We assume that LPAREN has already been consumed.
/// \param terminal_cnt The number of terminal items lexed so far.
/// \todo Assert label is a valid constituent or terminal label
const ItemToken* Treebank::lex_item(const PennTokenizer &lexer, unsigned terminal_cnt, const ID<Sentence>& sentence) {
	lexer.want(TEXT_TOK);
	Label label = string_to_label(lexer.current_yytext());

	Word word;
	const ItemToken* new_item;
	switch (lexer.get()) {
		case TEXT_TOK: // This is a leaf item...
			assert(is_terminal_label(label));
			word = string_to_word(lexer.current_yytext());
			lexer.want(RPAREN_TOK);
			new_item = _chart.add(label, word, terminal_cnt, sentence);
			sentences.back().add_item_to_frontier(new_item);
			assert(new_item == sentences.back().frontier().back());
			return new_item;

		case LPAREN_TOK: // This is a non-terminal node...
			// Just break out of the switch statement
			break;

		default: assert(0);
	}
	// We only escape the switch statement if this is a non-terminal
	// node

	assert(is_constituent_label(label));
	assert(!is_empty_constituent(label));

/*
	// Split off label modifiers
	n.label_modifier.clear();
	if (label.find("=") != label.npos) {
		string tmplabel = label.substr(0, label.find("="));
		string tmpmod = label.substr(label.find("="));
		Debug::log(4) << "Split = into '" << tmplabel << "' + '" << tmpmod << "'\n";
		label = tmplabel;
		n.label_modifier = tmpmod + n.label_modifier;
	}
	if (label.find("-") != label.npos) {
		string tmplabel = label.substr(0, label.find("-"));
		string tmpmod = label.substr(label.find("-"));
		Debug::log(4) << "Split - into '" << tmplabel << "' + '" << tmpmod << "'\n";
		label = tmplabel;
		n.label_modifier = tmpmod + n.label_modifier;
	}
	if (label.find("|") != label.npos) {
		string tmplabel = label.substr(0, label.find("|"));
		string tmpmod = label.substr(label.find("|"));
		Debug::log(4) << "Split | into '" << tmplabel << "' + '" << tmpmod << "'\n";
		label = tmplabel;
		n.label_modifier = tmpmod + n.label_modifier;
	}

	// Now, store the unmodified label...
*/

	ItemTokens children;
	children.clear();

	// Since we've already consumed an LPAREN_TOK, read in the first child...
	children.push_back(this->lex_item(lexer, terminal_cnt, sentence));

	// Keep reading in children, until we hit the end of this node
	Action action;
	while(1) {
		switch(lexer.get()) {
			case RPAREN_TOK: // End of node
				action = Action(label, children);
				new_item = _chart.add(action);
				sentences.back().perform(Production(new_item, action));
				return new_item;
			case LPAREN_TOK: // Some child node
				assert(!children.empty());
				assert(children.back()->span().right()+1>terminal_cnt);
				children.push_back(this->lex_item(lexer,
						children.back()->span().right()+1, sentence));
				break;
			default: cerr << lexer.current_yytext() << "\n"; assert(0);
		}
	}

	assert(0); // We should never cleanly leave the above loop
}

/// Read a path from the given lexer.
/// \param lexer A tokenizer for the desired input stream.
/// \param state We use state to replay the parse path and map
/// Spans to current ItemToken*'s for each production.
/// \sideeffects state will be updated to contain the path that we lex.
void Treebank::lex_path(const PennTokenizer &lexer, State& state) {
	lexer.want(LPAREN_TOK);
	state.restart();
	while(lex_production(lexer, state)) {
		;
	}
	assert(state.is_final());
}

/// Read a single production from the given lexer.
/// \param state We use state to replay the parse path and map
/// Spans to current ItemToken*'s for each production.
/// \return True iff we lexed a production. False iff we reached the
/// end of the path (and consumed an RPAREN).
bool Treebank::lex_production(const PennTokenizer &lexer, State& state) {
	switch (lexer.get()) {
		case LPAREN_TOK: break;
		case RPAREN_TOK: return false;
		default: assert(0);
	}

	Label label;
	unsigned left, right;

	lexer.want(TEXT_TOK);
	label = string_to_label(lexer.current_yytext());

	lexer.want(TEXT_TOK);
	assert(string(lexer.current_yytext()) == "<-");

	lexer.want(TEXT_TOK);
	istringstream i1(lexer.current_yytext());
	i1 >> left;

	lexer.want(TEXT_TOK);
	assert(string(lexer.current_yytext()) == "..");

	lexer.want(TEXT_TOK);
	istringstream i2(lexer.current_yytext());
	i2 >> right;

	lexer.want(RPAREN_TOK);

	Span span(left, right);
	ItemTokens children = state.locate_span(span);
	Action a(label, children);
	const ItemToken *i = _chart.get(a);
	Production p(i, a);
	state.perform(p);

	return true;
}
