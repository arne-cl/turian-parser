%{
/*  $Id: EnsembleTokenizer.ll 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file EnsembleTokenizer.L
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/decision-trees/EnsembleTokenizer.H"

#include "universal/Debug.H"

#include <cassert>

%}

D	[0-9]
INT	[-+]?{D}+
FLOAT1 	[-+]?(({D}*"."{D}+)|({D}+"."{D}*))(e[-+]?{D}+)?
FLOAT2 	[-+]?(({D}*"."?{D}+)|({D}+"."?{D}*))(e[-+]?{D}+)
WS	[\r\n \t]
SPLITON	"split"[\r\n \t]+"on"
TEXT	[^()\r\n \t]+
PARENTEXT	"("[^\r\n \t]+")"
 
%option noyywrap
%option prefix="ensemble"

%%

{INT}		{ return INT_TOK; }
{FLOAT1}	{ return FLOAT_TOK; }
{FLOAT2}	{ return FLOAT_TOK; }
"Tree"		{ return TREE_TOK; }
"Node"		{ return NODE_TOK; }
"Leaf"		{ return LEAF_TOK; }
"#"		{ return HASH_TOK; }
":"		{ return COLON_TOK; }
"+child"	{ return POSCHILD_TOK; }
"-child"	{ return NEGCHILD_TOK; }
"=="		{ return EQ_TOK; }
"="		{ return EQ_TOK; }
"confidence"	{ return CONF_TOK; }
"conf"		{ return CONF_TOK; }
"depth"		{ return DEPTH_TOK; }
"initial_weights"	{ return INITIAL_WEIGHTS_TOK; }

"use_full_examples"	{ return USE_FULL_EXAMPLES_TOK; }
"true"		{ return TRUE_TOK; }
"false"		{ return FALSE_TOK; }
"empty"		{ return EMPTY_TOK; }

"\["		{ return LSQUARE_BRACKET_TOK; }
"\]"		{ return RSQUARE_BRACKET_TOK; }
","		{ return COMMA_TOK; }

{SPLITON}	{ return SPLITON_TOK; }
{PARENTEXT}	{ return TEXT_TOK; }

"END"		{ return END_TOK; }
<<EOF>>		{ return EOF_TOK; }
{WS}		;
.		{ cerr << "Unexpected token: " << YYText() << "\n"; assert(0); }

%%

/// Consume a specific token.
/// We read a single token from the lexer, and throw an assertion if it
/// does not match the desired token or END_TOK.
/// \param tok The token that must be consumed.
/// \return False iff we consumed END_TOK instead of the desired token.
bool EnsembleTokenizer::want(const token tok) const {
	assert(this->lexer);
	token mytok = (token)this->lexer->yylex();
	assert (mytok == tok || mytok == END_TOK);
	return (mytok == tok);
}

double EnsembleTokenizer::get_float() const {
	assert(this->lexer);
	token tok = (token)this->lexer->yylex();
	assert(tok == INT_TOK || tok == FLOAT_TOK);
	return atof(this->lexer->YYText());
}

int EnsembleTokenizer::get_int() const {
	assert(this->lexer);
	assert(this->lexer->yylex() == INT_TOK);
	return atoi(this->lexer->YYText());
}

/// Lex one token.
/// \return The token consumed.
token EnsembleTokenizer::get() const {
	assert(this->lexer);
	return (token) this->lexer->yylex();
}

/// Retrieve the text of the last token lexed.
/// \return The text of the last token lexed.
const char* EnsembleTokenizer::current_yytext() const {
	return this->lexer->YYText();
}

void EnsembleTokenizer::open(const char *fil) {
	Debug::log(2) << "EnsembleTokenizer::open('" << fil << "')...\n";
	assert(!fin.is_open());
	fin.open(fil, ifstream::in);
	assert(fin.is_open());
	fin.clear();
	fin.seekg(0);

	this->lexer = new ensembleFlexLexer((istream *)&fin, NULL);
	assert(this->lexer);
}

void EnsembleTokenizer::open_cin() {
	this->lexer = new ensembleFlexLexer((istream *)&cin, NULL);
	assert(this->lexer);
}

void EnsembleTokenizer::close() {
	assert(fin.is_open());
	fin.close();
	assert(!fin.is_open());
	delete this->lexer;
	this->lexer = NULL;
}

void EnsembleTokenizer::string(const char *str) {
	if (this->lexer)
		delete this->lexer;

	this->iss.str(str);
	this->lexer = new ensembleFlexLexer(&this->iss, NULL);
	assert(this->lexer);
}
