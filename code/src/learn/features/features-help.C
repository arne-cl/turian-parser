/*  $Id: features-help.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file features-help.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/features/features-help.H"

#include "learn/examples/IntermediateExample.H"

#include "parse/ItemToken.H"
#include "parse/State.H"

#include "universal/globals.H"		// For PRIME
#include "universal/posclass.H"
#include "universal/vocabulary.H"

#include <cassert>
#include <ostream>
#include <sstream>

size_t feature_values_hash(int x1) {
	return x1;
}

size_t feature_values_hash(int x1, int x2) {
	return x1 + PRIME*x2;
}

size_t feature_values_hash(int x1, int x2, int x3) {
	return x1 + PRIME*(x2 + PRIME*x3);
}

size_t feature_values_hash(int x1, int x2, int x3, int x4) {
	return x1 + PRIME*(x2 + PRIME*(x3 + PRIME*x4));
}

size_t feature_values_hash(int x1, int x2, int x3, int x4, int x5) {
	return x1 + PRIME*(x2 + PRIME*(x3 + PRIME*(x4 + PRIME*x5)));
}

size_t feature_values_hash(int x1, int x2, int x3, int x4, int x5, int x6) {
	return x1 + PRIME*(x2 + PRIME*(x3 + PRIME*(x4 + PRIME*(x5 + PRIME*x6))));
}

size_t feature_values_hash(int x1, int x2, int x3, int x4, int x5, int x6, int x7) {
	return x1 + PRIME*(x2 + PRIME*(x3 + PRIME*(x4 + PRIME*(x5 + PRIME*(x6 + PRIME*x7)))));
}

void sanity_check_which_span_ty(which_span_ty x) {
	switch (x) {
		case LCONTEXT: break;
		case RCONTEXT: break;
		case SPAN: break;
		default: assert(0);
	}
}

void sanity_check_span_where_ty(span_where_ty x) {
	switch (x) {
		case START_ITEM: break;
		case HEAD_ITEM: break;
		case END_ITEM: break;
		default: assert(0);
	}
}

void sanity_check_which_span_where_ty(which_span_ty which, span_where_ty where) {
	sanity_check_which_span_ty(which);
	sanity_check_span_where_ty(where);

	// Only an action's span has a head item, not the left context or right context
	if (where == HEAD_ITEM) assert(which == SPAN);

	// Same for the end ITEM
	if (where == END_ITEM) assert(which == SPAN);
}


void sanity_check_item_predicate_ty(item_predicate_ty x) {
	switch (x) {
		case IS_TERMINAL_PREDICATE: assert(parameter::use_redundant_feature_predicates()); break;
		case LABEL_PREDICATE: break;
		case HEADLABEL_PREDICATE: break;
		case HEADTAG_PREDICATE: break;
		case HEADTAGCLASS_PREDICATE:
			assert(parameter::use_posclass()); break;
		case HEADWORD_PREDICATE: break;
		case EXISTS_PREDICATE: break;
		case IS_VERBAL_HEADTAG_PREDICATE:
			assert(parameter::use_redundant_feature_predicates());
			break;
		case IS_VERBAL_LABEL_PREDICATE:
			assert(parameter::use_redundant_feature_predicates());
			break;
		case DOMINATES_SOME_VERB_PREDICATE:
			assert(parameter::use_redundant_feature_predicates());
			assert(parameter::use_esoteric_feature_predicates());
			break;
		case ARE_ALL_CHILDREN_TERMINALS_PREDICATE:
			assert(parameter::use_redundant_feature_predicates());
			assert(parameter::use_esoteric_feature_predicates());
			break;
		case ONLY_ONE_CHILD_PREDICATE:
			assert(parameter::use_redundant_feature_predicates());
			assert(parameter::use_esoteric_feature_predicates());
			break;
		case IS_RIGHT_RECURSIVE_NP_PREDICATE:
			assert(parameter::use_redundant_feature_predicates());
			assert(parameter::use_esoteric_feature_predicates());
			break;
		case IS_POSSESSIVE_NP_KLEIN_MANNING_PREDICATE:
			assert(parameter::use_redundant_feature_predicates());
			assert(parameter::use_esoteric_feature_predicates());
			break;
		case IS_POSSESSIVE_NP_MACINTYRE_PREDICATE:
			assert(parameter::use_redundant_feature_predicates());
			assert(parameter::use_esoteric_feature_predicates());
			break;
		case IS_GAPPED_S_PREDICATE:
			assert(parameter::use_redundant_feature_predicates());
			assert(parameter::use_esoteric_feature_predicates());
			break;
		case IS_BASAL_NP_PREDICATE:
			assert(parameter::use_redundant_feature_predicates());
			assert(parameter::use_esoteric_feature_predicates());
			break;
		case IS_COORDINATED_PHRASE_PREDICATE:
			assert(parameter::use_redundant_feature_predicates());
			assert(parameter::use_esoteric_feature_predicates());
			break;
		case IS_CONTENT_WORD: assert(parameter::use_redundant_feature_predicates()); break;
		case IS_NOUN: assert(parameter::use_redundant_feature_predicates()); break;
		case IS_NOUN_PHRASE: assert(parameter::use_redundant_feature_predicates()); break;
		case IS_WH_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_V_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_NN_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_JJ_JJS_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_JJ_JJR_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_RB_RP_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_RBR_RBS_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_PP_ADVP_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_PP_SBAR_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_SBAR_ADJP_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_SBAR_ADJP_UCP_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_EX_PRP_QP_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_DT_PRPP_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_VBP_MD_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_MD_TO_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_TO_IN_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_WDT_WP_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_HASH_DOLLAR_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_S_SINV_SBARQ_FRAG_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_S_SINV_SBARQ_FRAG_SQ_X_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_S_SINV_SBARQ_FRAG_SQ_X_INTJ_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_COMMA_COLON_LABEL: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_V_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_NN_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_JJ_JJS_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_JJ_JJR_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_RB_RP_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_RBR_RBS_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_EX_PRP_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_DT_PRPP_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_VBP_MD_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_MD_TO_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_TO_IN_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_WDT_WP_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_HASH_DOLLAR_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_COMMA_COLON_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;

		default: assert(0);
	}
}

void sanity_check_terminal_predicate_ty(terminal_predicate_ty x) {
	switch (x) {
		case HEADTAG_PREDICATE: break;
		case HEADTAGCLASS_PREDICATE:
			assert(parameter::use_posclass()); break;
		case HEADWORD_PREDICATE: break;
		case EXISTS_PREDICATE: break;
		case IS_VERBAL_HEADTAG_PREDICATE: assert(parameter::use_redundant_feature_predicates()); break;
		case IS_VERBAL_LABEL_PREDICATE: assert(parameter::use_redundant_feature_predicates()); break;
		case IS_CONTENT_WORD: assert(parameter::use_redundant_feature_predicates()); break;
		case IS_NOUN: assert(parameter::use_redundant_feature_predicates()); break;
		case IS_V_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_NN_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_JJ_JJS_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_JJ_JJR_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_RB_RP_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_RBR_RBS_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_EX_PRP_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_DT_PRPP_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_VBP_MD_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_MD_TO_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_TO_IN_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_WDT_WP_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_HASH_DOLLAR_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		case IS_COMMA_COLON_HEADTAG: assert(parameter::use_clustered_feature_predicates()); assert(parameter::use_redundant_feature_predicates()); break;
		default: assert(0);
	}
}

void sanity_check_length_count_ty(length_count_ty x) {
	switch (x) {
		case ITEM_COUNT: break;
		case TERMINAL_COUNT: assert(parameter::use_terminal_features()); break;
		default: assert(0);
	}
}

void sanity_check_length_cmp_ty(length_cmp_ty x) {
	switch (x) {
		case GREATER_THAN: break;
		case EQUAL_TO: assert(parameter::length_equal_to_feature()); break;
		default: assert(0);
	}
}

void sanity_check_quant_ty(quant_ty x) {
	switch (x) {
		case THERE_EXISTS: break;
		case NOT_ALL: break;
		default: assert(0);
	}
}

void sanity_check_item_group_ty(item_group_ty x) {
	switch (x) {
		case ALL_ITEMS: break;
//		case ALL_LCONTEXT_ITEMS: assert(parameter::context_groups_for_exists_features()); break;
//		case ALL_RCONTEXT_ITEMS: assert(parameter::context_groups_for_exists_features()); break;
		case ALL_LCONTEXT_ITEMS: break;
		case ALL_RCONTEXT_ITEMS: break;
		case ALL_SPAN_ITEMS: break;
		case NONLEFTMOST_SPAN_ITEMS: break;
		case NONRIGHTMOST_SPAN_ITEMS: break;
		case NONHEAD_SPAN_ITEMS: break;
		case LEFT_OF_HEAD_SPAN_ITEMS: break;
		case RIGHT_OF_HEAD_SPAN_ITEMS: break;
		case HEAD_OR_LEFT_OF_HEAD_SPAN_ITEMS: break;
		case HEAD_OR_RIGHT_OF_HEAD_SPAN_ITEMS: break;
		default: assert(0);
	}
}

void sanity_check_predicate_and_value(item_predicate_ty predicate, unsigned value) {
	switch (predicate) {
		case IS_TERMINAL_PREDICATE:
			assert(value == (unsigned)true || value == (unsigned)false);
			break;

		case LABEL_PREDICATE:
			assert(is_label(value));
			break;

		case HEADLABEL_PREDICATE:
			assert(value == NO_LABEL || is_constituent_label(value));
			break;

		case HEADTAG_PREDICATE:
			assert(is_terminal_label(value));
			break;

		case HEADTAGCLASS_PREDICATE:
			assert(is_posclass(value));
			break;

		case HEADWORD_PREDICATE:
			assert(is_word(value));
			break;

		case EXISTS_PREDICATE:
		case IS_VERBAL_HEADTAG_PREDICATE:
		case IS_VERBAL_LABEL_PREDICATE:
		case DOMINATES_SOME_VERB_PREDICATE:
		case ARE_ALL_CHILDREN_TERMINALS_PREDICATE:
		case ONLY_ONE_CHILD_PREDICATE:
		case IS_RIGHT_RECURSIVE_NP_PREDICATE:
		case IS_POSSESSIVE_NP_KLEIN_MANNING_PREDICATE:
		case IS_POSSESSIVE_NP_MACINTYRE_PREDICATE:
		case IS_GAPPED_S_PREDICATE:
		case IS_BASAL_NP_PREDICATE:
		case IS_COORDINATED_PHRASE_PREDICATE:
		case IS_CONTENT_WORD:
		case IS_NOUN:
		case IS_NOUN_PHRASE:
		case IS_WH_LABEL:
		case IS_V_LABEL:
		case IS_NN_LABEL:
		case IS_JJ_JJS_LABEL:
		case IS_JJ_JJR_LABEL:
		case IS_RB_RP_LABEL:
		case IS_RBR_RBS_LABEL:
		case IS_PP_ADVP_LABEL:
		case IS_PP_SBAR_LABEL:
		case IS_SBAR_ADJP_LABEL:
		case IS_SBAR_ADJP_UCP_LABEL:
		case IS_EX_PRP_QP_LABEL:
		case IS_DT_PRPP_LABEL:
		case IS_VBP_MD_LABEL:
		case IS_MD_TO_LABEL:
		case IS_TO_IN_LABEL:
		case IS_WDT_WP_LABEL:
		case IS_HASH_DOLLAR_LABEL:
		case IS_S_SINV_SBARQ_FRAG_LABEL:
		case IS_S_SINV_SBARQ_FRAG_SQ_X_LABEL:
		case IS_S_SINV_SBARQ_FRAG_SQ_X_INTJ_LABEL:
		case IS_COMMA_COLON_LABEL:
		case IS_V_HEADTAG:
		case IS_NN_HEADTAG:
		case IS_JJ_JJS_HEADTAG:
		case IS_JJ_JJR_HEADTAG:
		case IS_RB_RP_HEADTAG:
		case IS_RBR_RBS_HEADTAG:
		case IS_EX_PRP_HEADTAG:
		case IS_DT_PRPP_HEADTAG:
		case IS_VBP_MD_HEADTAG:
		case IS_MD_TO_HEADTAG:
		case IS_TO_IN_HEADTAG:
		case IS_WDT_WP_HEADTAG:
		case IS_HASH_DOLLAR_HEADTAG:
		case IS_COMMA_COLON_HEADTAG:
			assert(value == (unsigned)true || value == (unsigned)false);
			break;
			
		default:
			assert(0);
	}
}

/// \todo Also sanity check against parameter::max_context_offset() +
/// which (i.e. perhaps LCONTEXT or RCONTEXT)
void sanity_check_where_offset(span_where_ty where, int offset) {
	switch (where) {
		case START_ITEM:
			assert(offset >= 0 && offset <= (int)parameter::max_offset());
			break;

		case HEAD_ITEM:
			assert(offset >= -(int)parameter::max_offset() && offset <= (int)parameter::max_offset());
			break;

		case END_ITEM:
			assert(offset <= 0 && offset >= -(int)parameter::max_offset());
			break;

		default: assert(0);
	}
}

/// \todo Also sanity check against parameter::max_terminal_context_offset() +
/// which (i.e. perhaps LCONTEXT or RCONTEXT)
void sanity_check_terminal_where_offset(span_where_ty where, int offset) {
	switch (where) {
		case START_ITEM:
			assert(offset >= 0 && offset <= (int)parameter::max_terminal_offset());
			break;

		case HEAD_ITEM:
			assert(offset >= -(int)parameter::max_terminal_offset() && offset <= (int)parameter::max_terminal_offset());
			break;

		case END_ITEM:
			assert(offset <= 0 && offset >= -(int)parameter::max_terminal_offset());
			break;

		default: assert(0);
	}
}

void sanity_check_child_where_offset(span_where_ty where, int offset) {
	switch (where) {
		case START_ITEM:
			assert(offset >= 0 && offset <= (int)parameter::max_child_offset());
			break;

		case HEAD_ITEM:
			assert(offset >= -(int)parameter::max_child_offset() && offset <= (int)parameter::max_child_offset());
			break;

		case END_ITEM:
			assert(offset <= 0 && offset >= -(int)parameter::max_child_offset());
			break;

		default: assert(0);
	}
}

void sanity_check_range_group_ty(range_group_ty x) {
	assert(parameter::use_range_features());
	switch (x) {
		case SPAN_START_RANGE: return;
		case SPAN_END_RANGE: return;
		case LEFT_OF_SPAN_HEAD_RANGE: return;
		case RIGHT_OF_SPAN_HEAD_RANGE: return;
		case LCONTEXT_RANGE: assert(parameter::use_context_features()); return;
		case RCONTEXT_RANGE: assert(parameter::use_context_features()); return;
		default: assert(0);
	}
}

const string which_span_string(which_span_ty which) {
	switch (which) {
		case LCONTEXT: return "left_context";
		case RCONTEXT: return "right_context";
		case SPAN: return "span";
		default: assert(0);
	}
}

bool is_binary_predicate(item_predicate_ty predicate) {
	switch (predicate) {
		case LABEL_PREDICATE:
		case HEADLABEL_PREDICATE:
		case HEADTAG_PREDICATE:
		case HEADTAGCLASS_PREDICATE:
		case HEADWORD_PREDICATE:
			return false;

		case EXISTS_PREDICATE:
		case IS_VERBAL_HEADTAG_PREDICATE:
		case IS_VERBAL_LABEL_PREDICATE:
		case DOMINATES_SOME_VERB_PREDICATE:
		case ARE_ALL_CHILDREN_TERMINALS_PREDICATE:
		case ONLY_ONE_CHILD_PREDICATE:
		case IS_RIGHT_RECURSIVE_NP_PREDICATE:
		case IS_POSSESSIVE_NP_KLEIN_MANNING_PREDICATE:
		case IS_POSSESSIVE_NP_MACINTYRE_PREDICATE:
		case IS_GAPPED_S_PREDICATE:
		case IS_BASAL_NP_PREDICATE:
		case IS_COORDINATED_PHRASE_PREDICATE:
		case IS_CONTENT_WORD:
		case IS_NOUN:
		case IS_NOUN_PHRASE:
		case IS_WH_LABEL:
		case IS_V_LABEL:
		case IS_NN_LABEL:
		case IS_JJ_JJS_LABEL:
		case IS_JJ_JJR_LABEL:
		case IS_RB_RP_LABEL:
		case IS_RBR_RBS_LABEL:
		case IS_PP_ADVP_LABEL:
		case IS_PP_SBAR_LABEL:
		case IS_SBAR_ADJP_LABEL:
		case IS_SBAR_ADJP_UCP_LABEL:
		case IS_EX_PRP_QP_LABEL:
		case IS_DT_PRPP_LABEL:
		case IS_VBP_MD_LABEL:
		case IS_MD_TO_LABEL:
		case IS_TO_IN_LABEL:
		case IS_WDT_WP_LABEL:
		case IS_HASH_DOLLAR_LABEL:
		case IS_S_SINV_SBARQ_FRAG_LABEL:
		case IS_S_SINV_SBARQ_FRAG_SQ_X_LABEL:
		case IS_S_SINV_SBARQ_FRAG_SQ_X_INTJ_LABEL:
		case IS_COMMA_COLON_LABEL:
		case IS_V_HEADTAG:
		case IS_NN_HEADTAG:
		case IS_JJ_JJS_HEADTAG:
		case IS_JJ_JJR_HEADTAG:
		case IS_RB_RP_HEADTAG:
		case IS_RBR_RBS_HEADTAG:
		case IS_EX_PRP_HEADTAG:
		case IS_DT_PRPP_HEADTAG:
		case IS_VBP_MD_HEADTAG:
		case IS_MD_TO_HEADTAG:
		case IS_TO_IN_HEADTAG:
		case IS_WDT_WP_HEADTAG:
		case IS_HASH_DOLLAR_HEADTAG:
		case IS_COMMA_COLON_HEADTAG:
		case IS_TERMINAL_PREDICATE:
			return true;

		default: assert(0);
	}
}

const string where_offset_string(span_where_ty where, int offset) {
	ostringstream o;
	switch (where) {
		case START_ITEM: o << "start"; break;
		case HEAD_ITEM: o << "head"; break;
		case END_ITEM: o << "end"; break;
		default: assert(0);
	}
//	if (offset >= 0) {
	if (offset > 0) {
		o << "+" << offset;
	} else if (offset < 0) {
		o << offset;
	}
	return o.str();
}

const string predicate_value_string(item_predicate_ty predicate, unsigned value) {
	ostringstream o;
	switch (predicate) {
		case IS_TERMINAL_PREDICATE:
			o << "is_terminal?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case LABEL_PREDICATE:
			assert(is_label(value));
			o << "label=='" << label_string(value) << "'";
			break;

		case HEADLABEL_PREDICATE:
			assert(value == NO_LABEL || is_constituent_label(value));
			if (value == NO_LABEL)
				o << "headlabel=='NONE'";
			else
				o << "headlabel=='" << label_string(value) << "'";
			break;

		case HEADTAG_PREDICATE:
			assert(is_terminal_label(value));
			o << "headtag=='" << label_string(value) << "'";
			break;

		case HEADTAGCLASS_PREDICATE:
			assert(is_posclass(value));
			o << "headtagclass=='" << posclass_string(value) << "'";
			break;

		case HEADWORD_PREDICATE:
			assert(is_word(value));
//			o << "headword=='" << word_string(value) << "'";
			o << "headword==\"" << word_string(value) << "\"";
			break;

		case EXISTS_PREDICATE:
			o << "exists?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_VERBAL_HEADTAG_PREDICATE:
			o << "is_verbal_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_VERBAL_LABEL_PREDICATE:
			o << "is_verbal_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case DOMINATES_SOME_VERB_PREDICATE:
			o << "dominates_some_verb?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case ARE_ALL_CHILDREN_TERMINALS_PREDICATE:
			o << "are_all_children_terminals?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case ONLY_ONE_CHILD_PREDICATE:
			o << "only_one_child?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_RIGHT_RECURSIVE_NP_PREDICATE:
			o << "is_right_recursive_NP==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_POSSESSIVE_NP_KLEIN_MANNING_PREDICATE:
			o << "is_possessive_NP_Klein_and_Manning_test?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_POSSESSIVE_NP_MACINTYRE_PREDICATE:
			o << "is_possessive_NP_MacIntyre_test?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_GAPPED_S_PREDICATE:
			o << "is_gapped_S?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_BASAL_NP_PREDICATE:
			o << "is_basal_NP?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_COORDINATED_PHRASE_PREDICATE:
			o << "is_coordinated_phrase?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_CONTENT_WORD:
			o << "is_content_word?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_NOUN:
			o << "is_noun?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_NOUN_PHRASE:
			o << "is_noun_phrase?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_WH_LABEL:
			o << "is_WH_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_V_LABEL:
			o << "is_V_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_NN_LABEL:
			o << "is_NN_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_JJ_JJS_LABEL:
			o << "is_JJ_JJS_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_JJ_JJR_LABEL:
			o << "is_JJ_JJR_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_RB_RP_LABEL:
			o << "is_RB_RP_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_RBR_RBS_LABEL:
			o << "is_RBR_RBS_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_PP_ADVP_LABEL:
			o << "is_PP_ADVP_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_PP_SBAR_LABEL:
			o << "is_PP_SBAR_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_SBAR_ADJP_LABEL:
			o << "is_SBAR_ADJP_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_SBAR_ADJP_UCP_LABEL:
			o << "is_SBAR_ADJP_UCP_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_EX_PRP_QP_LABEL:
			o << "is_EX_PRP_QP_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_DT_PRPP_LABEL:
			o << "is_DT_PRPP_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_VBP_MD_LABEL:
			o << "is_VBP_MD_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_MD_TO_LABEL:
			o << "is_MD_TO_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_TO_IN_LABEL:
			o << "is_TO_IN_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_WDT_WP_LABEL:
			o << "is_WDT_WP_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_HASH_DOLLAR_LABEL:
			o << "is_HASH_DOLLAR_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_S_SINV_SBARQ_FRAG_LABEL:
			o << "is_S_SINV_SBARQ_FRAG_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_S_SINV_SBARQ_FRAG_SQ_X_LABEL:
			o << "is_S_SINV_SBARQ_FRAG_SQ_X_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_S_SINV_SBARQ_FRAG_SQ_X_INTJ_LABEL:
			o << "is_S_SINV_SBARQ_FRAG_SQ_X_INTJ_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_COMMA_COLON_LABEL:
			o << "is_COMMA_COLON_label?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_V_HEADTAG:
			o << "is_V_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_NN_HEADTAG:
			o << "is_NN_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_JJ_JJS_HEADTAG:
			o << "is_JJ_JJS_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_JJ_JJR_HEADTAG:
			o << "is_JJ_JJR_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_RB_RP_HEADTAG:
			o << "is_RB_RP_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_RBR_RBS_HEADTAG:
			o << "is_RBR_RBS_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_EX_PRP_HEADTAG:
			o << "is_EX_PRP_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_DT_PRPP_HEADTAG:
			o << "is_DT_PRPP_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_VBP_MD_HEADTAG:
			o << "is_VBP_MD_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_MD_TO_HEADTAG:
			o << "is_MD_TO_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_TO_IN_HEADTAG:
			o << "is_TO_IN_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_WDT_WP_HEADTAG:
			o << "is_WDT_WP_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_HASH_DOLLAR_HEADTAG:
			o << "is_HASH_DOLLAR_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		case IS_COMMA_COLON_HEADTAG:
			o << "is_COMMA_COLON_headtag?==";
			if (value == (unsigned)true) o << "True";
			else if (value == (unsigned)false) o << "False";
			else assert(0);
			break;

		default:
			assert(0);
	}
	return o.str();
}

const string item_group_string(item_group_ty group) {
	switch (group) {
		case ALL_ITEMS: return "frontier";
		case ALL_LCONTEXT_ITEMS: return "lcontext";
		case ALL_RCONTEXT_ITEMS: return "rcontext";
		case ALL_SPAN_ITEMS: return "span";
		case NONLEFTMOST_SPAN_ITEMS: return "nonleftmost_in_span";
		case NONRIGHTMOST_SPAN_ITEMS: return "nonrightmost_in_span";
		case NONHEAD_SPAN_ITEMS: return "nonhead_in_span";
		case LEFT_OF_HEAD_SPAN_ITEMS: return "left_of_head_in_span";
		case RIGHT_OF_HEAD_SPAN_ITEMS: return "right_of_head_in_span";
		case HEAD_OR_LEFT_OF_HEAD_SPAN_ITEMS: return "head_or_left_of_head_in_span";
		case HEAD_OR_RIGHT_OF_HEAD_SPAN_ITEMS: return "head_or_right_of_head_in_span";
		default: assert(0);
	}
}

const string range_string(range_group_ty group, unsigned range) {
	ostringstream o;
	switch(group) {
		case SPAN_START_RANGE:
			o << "first_" << range << "_span_items";
			break;
		case SPAN_END_RANGE:
			o << "last_" << range << "_span_items";
			break;
		case LEFT_OF_SPAN_HEAD_RANGE:
			o << range << "_span_items_left_of_head";
			break;
		case RIGHT_OF_SPAN_HEAD_RANGE:
			o << range << "_span_items_right_of_head";
			break;
		case LCONTEXT_RANGE:
			o << "first_" << range << "_lcontext_items";
			break;
		case RCONTEXT_RANGE:
			o << "first_" << range << "_rcontext_items";
			break;
		default: assert(0);
	}
	return o.str();
}


unsigned max_group_range(range_group_ty group) {
	switch (group) {
		case LCONTEXT_RANGE:
		case RCONTEXT_RANGE:
			return parameter::max_context_range();
		case SPAN_START_RANGE:
		case SPAN_END_RANGE:
		case LEFT_OF_SPAN_HEAD_RANGE:
		case RIGHT_OF_SPAN_HEAD_RANGE:
			return parameter::max_range();
		default:
			assert(0);
	}
	assert(0); return 0;
}


static LIST<which_span_ty> cached_which_span;
static LIST<span_where_ty> cached_span_where;
static AutoVector<LIST<span_where_ty> > cached_which_span_where;
static LIST<item_predicate_ty> cached_item_predicates;
static LIST<terminal_predicate_ty> cached_terminal_predicates;
static LIST<item_group_ty> cached_item_groups;
static LIST<range_group_ty> cached_range_groups;
static AutoVector<LIST<int> > cached_where_offsets;
static AutoVector<LIST<int> > cached_child_where_offsets;
static AutoVector<LIST<unsigned> > cached_predicate_values;
static AutoVector<unsigned> cached_max_predicate_value;
static LIST<unsigned> cached_bool_values;
static bool is_init = false;

/// \todo Sanity check cache values at the end.
/// \todo Use parameter::max_context_offset() + which (i.e. perhaps
/// LCONTEXT or RCONTEXT) when storing offsets
static void initialize_cache() {
	if (is_init) return;

	cached_bool_values.push_back((unsigned)false);
	cached_bool_values.push_back((unsigned)true);

	if (parameter::use_context_features()) {
		cached_which_span.push_back(LCONTEXT);
		cached_which_span.push_back(RCONTEXT);
	}
	cached_which_span.push_back(SPAN);

	cached_span_where.push_back(START_ITEM);
	cached_span_where.push_back(END_ITEM);
	cached_span_where.push_back(HEAD_ITEM);

	//if (parameter::use_range_features()) {
	cached_range_groups.push_back(SPAN_START_RANGE);
	cached_range_groups.push_back(SPAN_END_RANGE);
	cached_range_groups.push_back(LEFT_OF_SPAN_HEAD_RANGE);
	cached_range_groups.push_back(RIGHT_OF_SPAN_HEAD_RANGE);
	if (parameter::use_context_features()) {
		cached_range_groups.push_back(LCONTEXT_RANGE);
		cached_range_groups.push_back(RCONTEXT_RANGE);
	}
	//}

	for (LIST<which_span_ty>::const_iterator i = cached_which_span.begin(); i != cached_which_span.end(); i++) {
		cached_which_span_where[*i].push_back(START_ITEM);
		
		// Only an action's span has a head item, not the left context or right context
		if (*i == SPAN) cached_which_span_where[*i].push_back(HEAD_ITEM);

		// Same for the end item.
		if (*i == SPAN) cached_which_span_where[*i].push_back(END_ITEM);
	}
	cached_which_span_where.lock();

	if (parameter::use_redundant_feature_predicates())
		cached_item_predicates.push_back(IS_TERMINAL_PREDICATE);
	cached_item_predicates.push_back(LABEL_PREDICATE);
//	cached_item_predicates.push_back(HEADLABEL_PREDICATE);
	cached_item_predicates.push_back(HEADTAG_PREDICATE);
	if (parameter::use_posclass())
		cached_item_predicates.push_back(HEADTAGCLASS_PREDICATE);
	cached_item_predicates.push_back(HEADWORD_PREDICATE);
//	cached_item_predicates.push_back(EXISTS_PREDICATE);

	if (parameter::use_redundant_feature_predicates()) {
		cached_item_predicates.push_back(IS_VERBAL_HEADTAG_PREDICATE);
		cached_item_predicates.push_back(IS_VERBAL_LABEL_PREDICATE);
		if (parameter::use_esoteric_feature_predicates()) {
			cached_item_predicates.push_back(DOMINATES_SOME_VERB_PREDICATE);
//			cached_item_predicates.push_back(ARE_ALL_CHILDREN_TERMINALS_PREDICATE);
			cached_item_predicates.push_back(ONLY_ONE_CHILD_PREDICATE);
//			cached_item_predicates.push_back(IS_RIGHT_RECURSIVE_NP_PREDICATE);
//			cached_item_predicates.push_back(IS_POSSESSIVE_NP_KLEIN_MANNING_PREDICATE);
//			cached_item_predicates.push_back(IS_POSSESSIVE_NP_MACINTYRE_PREDICATE);
			cached_item_predicates.push_back(IS_GAPPED_S_PREDICATE);
			cached_item_predicates.push_back(IS_BASAL_NP_PREDICATE);
			cached_item_predicates.push_back(IS_COORDINATED_PHRASE_PREDICATE);
		}
		cached_item_predicates.push_back(IS_CONTENT_WORD);
		cached_item_predicates.push_back(IS_NOUN);
		cached_item_predicates.push_back(IS_NOUN_PHRASE);
		if (parameter::use_clustered_feature_predicates()) {
			cached_item_predicates.push_back(IS_WH_LABEL);
			cached_item_predicates.push_back(IS_V_LABEL);
			cached_item_predicates.push_back(IS_NN_LABEL);
//			cached_item_predicates.push_back(IS_JJ_JJS_LABEL);
			cached_item_predicates.push_back(IS_JJ_JJR_LABEL);
			cached_item_predicates.push_back(IS_RB_RP_LABEL);
			cached_item_predicates.push_back(IS_RBR_RBS_LABEL);
//			cached_item_predicates.push_back(IS_PP_ADVP_LABEL);
//			cached_item_predicates.push_back(IS_PP_SBAR_LABEL);
//			cached_item_predicates.push_back(IS_SBAR_ADJP_LABEL);
//			cached_item_predicates.push_back(IS_SBAR_ADJP_UCP_LABEL);
			cached_item_predicates.push_back(IS_EX_PRP_QP_LABEL);
			cached_item_predicates.push_back(IS_DT_PRPP_LABEL);
			cached_item_predicates.push_back(IS_VBP_MD_LABEL);
			cached_item_predicates.push_back(IS_MD_TO_LABEL);
			cached_item_predicates.push_back(IS_TO_IN_LABEL);
			cached_item_predicates.push_back(IS_WDT_WP_LABEL);
			cached_item_predicates.push_back(IS_HASH_DOLLAR_LABEL);
			cached_item_predicates.push_back(IS_S_SINV_SBARQ_FRAG_LABEL);
			cached_item_predicates.push_back(IS_S_SINV_SBARQ_FRAG_SQ_X_LABEL);
//			cached_item_predicates.push_back(IS_S_SINV_SBARQ_FRAG_SQ_X_INTJ_LABEL);
//			cached_item_predicates.push_back(IS_COMMA_COLON_LABEL);
			cached_item_predicates.push_back(IS_V_HEADTAG);
			cached_item_predicates.push_back(IS_NN_HEADTAG);
//			cached_item_predicates.push_back(IS_JJ_JJS_HEADTAG);
//			cached_item_predicates.push_back(IS_JJ_JJR_HEADTAG);
//			cached_item_predicates.push_back(IS_RB_RP_HEADTAG);
//			cached_item_predicates.push_back(IS_RBR_RBS_HEADTAG);
			cached_item_predicates.push_back(IS_EX_PRP_HEADTAG);
//			cached_item_predicates.push_back(IS_DT_PRPP_HEADTAG);
//			cached_item_predicates.push_back(IS_VBP_MD_HEADTAG);
//			cached_item_predicates.push_back(IS_MD_TO_HEADTAG);
			cached_item_predicates.push_back(IS_TO_IN_HEADTAG);
			cached_item_predicates.push_back(IS_WDT_WP_HEADTAG);
//			cached_item_predicates.push_back(IS_HASH_DOLLAR_HEADTAG);
//			cached_item_predicates.push_back(IS_COMMA_COLON_HEADTAG);
		}
	}

	cached_terminal_predicates.push_back(HEADTAG_PREDICATE);
	if (parameter::use_posclass())
		cached_terminal_predicates.push_back(HEADTAGCLASS_PREDICATE);
	cached_terminal_predicates.push_back(HEADWORD_PREDICATE);
//	cached_terminal_predicates.push_back(EXISTS_PREDICATE);

	if (parameter::use_redundant_feature_predicates()) {
		cached_terminal_predicates.push_back(IS_VERBAL_LABEL_PREDICATE);
		cached_terminal_predicates.push_back(IS_CONTENT_WORD);
		cached_terminal_predicates.push_back(IS_NOUN);
		if (parameter::use_clustered_feature_predicates()) {
			cached_terminal_predicates.push_back(IS_V_HEADTAG);
			cached_terminal_predicates.push_back(IS_NN_HEADTAG);
//			cached_terminal_predicates.push_back(IS_JJ_JJS_HEADTAG);
//			cached_terminal_predicates.push_back(IS_JJ_JJR_HEADTAG);
//			cached_terminal_predicates.push_back(IS_RB_RP_HEADTAG);
//			cached_terminal_predicates.push_back(IS_RBR_RBS_HEADTAG);
			cached_terminal_predicates.push_back(IS_EX_PRP_HEADTAG);
//			cached_terminal_predicates.push_back(IS_DT_PRPP_HEADTAG);
//			cached_terminal_predicates.push_back(IS_VBP_MD_HEADTAG);
//			cached_terminal_predicates.push_back(IS_MD_TO_HEADTAG);
			cached_terminal_predicates.push_back(IS_TO_IN_HEADTAG);
			cached_terminal_predicates.push_back(IS_WDT_WP_HEADTAG);
//			cached_terminal_predicates.push_back(IS_HASH_DOLLAR_HEADTAG);
//			cached_terminal_predicates.push_back(IS_COMMA_COLON_HEADTAG);
		}
	}

	if (parameter::use_context_features()) {
		cached_item_groups.push_back(ALL_ITEMS);
		cached_item_groups.push_back(ALL_LCONTEXT_ITEMS);
		cached_item_groups.push_back(ALL_RCONTEXT_ITEMS);
	}
	cached_item_groups.push_back(ALL_SPAN_ITEMS);
	cached_item_groups.push_back(NONLEFTMOST_SPAN_ITEMS);
	cached_item_groups.push_back(NONRIGHTMOST_SPAN_ITEMS);
	cached_item_groups.push_back(NONHEAD_SPAN_ITEMS);
	cached_item_groups.push_back(LEFT_OF_HEAD_SPAN_ITEMS);
	cached_item_groups.push_back(RIGHT_OF_HEAD_SPAN_ITEMS);
	cached_item_groups.push_back(HEAD_OR_LEFT_OF_HEAD_SPAN_ITEMS);
	cached_item_groups.push_back(HEAD_OR_RIGHT_OF_HEAD_SPAN_ITEMS);

	for (LIST<span_where_ty>::const_iterator i = cached_span_where.begin(); i != cached_span_where.end(); i++) {
		int min_offset, max_offset;
		switch (*i) {
			case START_ITEM:
				min_offset = 0;
				max_offset = (int)parameter::max_offset();
				break;

			case HEAD_ITEM:
				min_offset = -(int)parameter::max_offset();
				max_offset = (int)parameter::max_offset();
				break;
	
			case END_ITEM:
				min_offset = -(int)parameter::max_offset();
				max_offset = 0;
				break;

			default: assert(0);
		}
		for (int off = min_offset; off <= max_offset; off++)
			cached_where_offsets[*i].push_back(off);
	}
	cached_where_offsets.lock();

	for (LIST<span_where_ty>::const_iterator i = cached_span_where.begin(); i != cached_span_where.end(); i++) {
		int min_offset, max_offset;
		switch (*i) {
			case START_ITEM:
				min_offset = 0;
				max_offset = (int)parameter::max_child_offset();
				break;
	
			case HEAD_ITEM:
				min_offset = -(int)parameter::max_child_offset();
				max_offset = (int)parameter::max_child_offset();
				break;
	
			case END_ITEM:
				min_offset = -(int)parameter::max_child_offset();
				max_offset = 0;
				break;

			default: assert(0);
		}
		for (int off = min_offset; off <= max_offset; off++)
			cached_child_where_offsets[*i].push_back(off);
	}
	cached_child_where_offsets.lock();

	cached_predicate_values[IS_TERMINAL_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[IS_TERMINAL_PREDICATE].push_back((unsigned)true);

	for (LIST<Label>::const_iterator l = all_labels().begin(); l != all_labels().end(); l++)
		cached_predicate_values[LABEL_PREDICATE].push_back(*l);
	cached_predicate_values[HEADLABEL_PREDICATE].push_back(NO_LABEL);
	for (LIST<Label>::const_iterator l = all_constituent_labels().begin(); l != all_constituent_labels().end(); l++)
		cached_predicate_values[HEADLABEL_PREDICATE].push_back(*l);
	for (LIST<Label>::const_iterator l = all_terminal_labels().begin(); l != all_terminal_labels().end(); l++)
		cached_predicate_values[HEADTAG_PREDICATE].push_back(*l);
	for (LIST<Posclass>::const_iterator p = all_posclass().begin(); p != all_posclass().end(); p++)
		cached_predicate_values[HEADTAGCLASS_PREDICATE].push_back(*p);
	for (LIST<Word>::const_iterator w = all_words().begin(); w != all_words().end(); w++)
		cached_predicate_values[HEADWORD_PREDICATE].push_back(*w);

	cached_predicate_values[EXISTS_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[EXISTS_PREDICATE].push_back((unsigned)true);
	cached_predicate_values[IS_VERBAL_HEADTAG_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[IS_VERBAL_HEADTAG_PREDICATE].push_back((unsigned)true);
	cached_predicate_values[IS_VERBAL_LABEL_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[IS_VERBAL_LABEL_PREDICATE].push_back((unsigned)true);
	cached_predicate_values[DOMINATES_SOME_VERB_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[DOMINATES_SOME_VERB_PREDICATE].push_back((unsigned)true);
//	cached_predicate_values[ARE_ALL_CHILDREN_TERMINALS_PREDICATE].push_back((unsigned)false);
//	cached_predicate_values[ARE_ALL_CHILDREN_TERMINALS_PREDICATE].push_back((unsigned)true);
	cached_predicate_values[ONLY_ONE_CHILD_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[ONLY_ONE_CHILD_PREDICATE].push_back((unsigned)true);
	cached_predicate_values[IS_RIGHT_RECURSIVE_NP_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[IS_RIGHT_RECURSIVE_NP_PREDICATE].push_back((unsigned)true);
	cached_predicate_values[IS_POSSESSIVE_NP_KLEIN_MANNING_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[IS_POSSESSIVE_NP_KLEIN_MANNING_PREDICATE].push_back((unsigned)true);
	cached_predicate_values[IS_POSSESSIVE_NP_MACINTYRE_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[IS_POSSESSIVE_NP_MACINTYRE_PREDICATE].push_back((unsigned)true);
	cached_predicate_values[IS_GAPPED_S_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[IS_GAPPED_S_PREDICATE].push_back((unsigned)true);
	cached_predicate_values[IS_BASAL_NP_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[IS_BASAL_NP_PREDICATE].push_back((unsigned)true);
	cached_predicate_values[IS_COORDINATED_PHRASE_PREDICATE].push_back((unsigned)false);
	cached_predicate_values[IS_COORDINATED_PHRASE_PREDICATE].push_back((unsigned)true);
	cached_predicate_values[IS_CONTENT_WORD].push_back((unsigned)false);
	cached_predicate_values[IS_CONTENT_WORD].push_back((unsigned)true);
	cached_predicate_values[IS_NOUN].push_back((unsigned)false);
	cached_predicate_values[IS_NOUN].push_back((unsigned)true);
	cached_predicate_values[IS_NOUN_PHRASE].push_back((unsigned)false);
	cached_predicate_values[IS_NOUN_PHRASE].push_back((unsigned)true);
	cached_predicate_values[IS_V_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_V_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_WH_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_WH_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_NN_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_NN_LABEL].push_back((unsigned)true);
//	cached_predicate_values[IS_JJ_JJS_LABEL].push_back((unsigned)false);
//	cached_predicate_values[IS_JJ_JJS_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_JJ_JJR_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_JJ_JJR_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_RB_RP_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_RB_RP_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_RBR_RBS_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_RBR_RBS_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_PP_ADVP_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_PP_ADVP_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_PP_SBAR_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_PP_SBAR_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_SBAR_ADJP_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_SBAR_ADJP_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_SBAR_ADJP_UCP_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_SBAR_ADJP_UCP_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_EX_PRP_QP_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_EX_PRP_QP_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_DT_PRPP_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_DT_PRPP_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_VBP_MD_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_VBP_MD_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_MD_TO_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_MD_TO_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_TO_IN_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_TO_IN_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_WDT_WP_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_WDT_WP_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_HASH_DOLLAR_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_HASH_DOLLAR_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_S_SINV_SBARQ_FRAG_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_S_SINV_SBARQ_FRAG_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_S_SINV_SBARQ_FRAG_SQ_X_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_S_SINV_SBARQ_FRAG_SQ_X_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_S_SINV_SBARQ_FRAG_SQ_X_INTJ_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_S_SINV_SBARQ_FRAG_SQ_X_INTJ_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_COMMA_COLON_LABEL].push_back((unsigned)false);
	cached_predicate_values[IS_COMMA_COLON_LABEL].push_back((unsigned)true);
	cached_predicate_values[IS_V_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_V_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_NN_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_NN_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_JJ_JJS_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_JJ_JJS_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_JJ_JJR_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_JJ_JJR_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_RB_RP_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_RB_RP_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_RBR_RBS_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_RBR_RBS_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_EX_PRP_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_EX_PRP_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_DT_PRPP_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_DT_PRPP_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_VBP_MD_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_VBP_MD_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_MD_TO_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_MD_TO_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_TO_IN_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_TO_IN_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_WDT_WP_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_WDT_WP_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_HASH_DOLLAR_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_HASH_DOLLAR_HEADTAG].push_back((unsigned)true);
	cached_predicate_values[IS_COMMA_COLON_HEADTAG].push_back((unsigned)false);
	cached_predicate_values[IS_COMMA_COLON_HEADTAG].push_back((unsigned)true);

	cached_predicate_values.lock();

	for (LIST<item_predicate_ty>::const_iterator i = cached_item_predicates.begin(); i != cached_item_predicates.end(); i++) {
		const item_predicate_ty& p = *i;
		cached_max_predicate_value[p] = 0;
		LIST<unsigned>::const_iterator j;
		for (j = cached_predicate_values.at(p).begin(); j != cached_predicate_values.at(p).end(); j++)
			if (cached_max_predicate_value[p] < *j)
				cached_max_predicate_value[p] = *j;
	}
	cached_max_predicate_value.lock();

	is_init = true;
}

const LIST<which_span_ty>& all_which_span() {
	initialize_cache();
	return cached_which_span;
}

const LIST<span_where_ty>& all_span_where() {
	initialize_cache();
	return cached_span_where;
}

const LIST<span_where_ty>& all_span_where(which_span_ty which) {
	sanity_check_which_span_ty(which);
	initialize_cache();
	return cached_which_span_where.at(which);
}

const LIST<item_predicate_ty>& all_item_predicates() {
	initialize_cache();
	return cached_item_predicates;
}

const LIST<terminal_predicate_ty>& all_terminal_predicates() {
	initialize_cache();
	return cached_terminal_predicates;
}

const LIST<item_group_ty>& all_item_groups() {
	initialize_cache();
	return cached_item_groups;
}

const LIST<range_group_ty>& all_range_groups() {
	initialize_cache();
	return cached_range_groups;
}

const LIST<int>& all_where_offsets(span_where_ty where) {
	sanity_check_span_where_ty(where);
	initialize_cache();
	return cached_where_offsets.at(where);
}

const LIST<int>& all_child_where_offsets(span_where_ty where) {
	sanity_check_span_where_ty(where);
	initialize_cache();
	return cached_child_where_offsets.at(where);
}

/// Find all possible values for some predicate.
/// \param predicate The predicate which values we want.
/// \return A list of all possible values of this predicate.
/// \todo Do we really want special case handling for boolean predicates?
/// Reason in favor of special case handling for boolean predicates:
///	- We don't consider redundant features. Only one suffices.
/// Reason against special case handling for boolean predicates:
///	- Cleaner, simpler code.
const LIST<unsigned>& all_predicate_values(item_predicate_ty predicate) {
	sanity_check_item_predicate_ty(predicate);
	initialize_cache();
	return cached_predicate_values.at(predicate);
}

/// Retrieve the maximum possible value for some predicate.
/// \todo This is really ugly. Outsource the work to methods in vocabulary.C posclass.C.
unsigned max_predicate_value(item_predicate_ty predicate) {
	sanity_check_item_predicate_ty(predicate);
	initialize_cache();
	return cached_max_predicate_value.at(predicate);
}

/// Determine if a predicate holds on an item.
/// \param predicate The predicate type to be checked.
/// \param value The desired value of the predicate.
/// \param item The item to check the predicate value.
/// \return True iff the example has the desired predicate value.
bool predicate_holds(item_predicate_ty predicate, unsigned value, const ItemToken& item) {
	sanity_check_predicate_and_value(predicate, value);
	return value == get_predicate_value(predicate, item);
}

unsigned get_predicate_value(item_predicate_ty predicate, const ItemToken& item) {
	switch (predicate) {
		case IS_TERMINAL_PREDICATE:
			return item.is_terminal();

		case LABEL_PREDICATE:
			return item.label();

		case HEADLABEL_PREDICATE:
			return item.headlabel();

		case HEADTAG_PREDICATE:
			return item.headtag();

		case HEADTAGCLASS_PREDICATE:
			return item.headtagclass();

		case HEADWORD_PREDICATE:
			return item.headword();

		case EXISTS_PREDICATE:
			return true;

		case IS_VERBAL_HEADTAG_PREDICATE:
			return item.is_verbal_headtag();

		case IS_VERBAL_LABEL_PREDICATE:
			return item.is_verbal_label();

		case DOMINATES_SOME_VERB_PREDICATE:
			return item.dominates_some_verb();

		case ONLY_ONE_CHILD_PREDICATE:
			return item.only_one_child();

			/*
		case ARE_ALL_CHILDREN_TERMINALS_PREDICATE:
			return item.are_all_children_terminals();

		case IS_RIGHT_RECURSIVE_NP_PREDICATE:
			return item.is_right_recursive_NP();

		case IS_POSSESSIVE_NP_KLEIN_MANNING_PREDICATE:
			return item.is_possessive_NP_Klein_and_Manning_test();

		case IS_POSSESSIVE_NP_MACINTYRE_PREDICATE:
			return item.is_possessive_NP_MacIntyre_test();
			*/

		case IS_BASAL_NP_PREDICATE:
			return item.is_basal_NP();

		case IS_GAPPED_S_PREDICATE:
			return item.is_gapped_S();

		case IS_COORDINATED_PHRASE_PREDICATE:
			return item.is_coordinated_phrase();

		case IS_CONTENT_WORD:
			return item.is_content_word();

		case IS_NOUN:
			return item.is_noun();

		case IS_NOUN_PHRASE:
			return item.is_noun_phrase();

		case IS_WH_LABEL:
			return item.is_WH_label();

		case IS_V_LABEL:
			return item.is_V_label();

		case IS_NN_LABEL:
			return item.is_NN_label();

		case IS_JJ_JJS_LABEL:
			return item.is_JJ_JJS_label();

		case IS_JJ_JJR_LABEL:
			return item.is_JJ_JJR_label();

		case IS_RB_RP_LABEL:
			return item.is_RB_RP_label();

		case IS_RBR_RBS_LABEL:
			return item.is_RBR_RBS_label();

		case IS_PP_ADVP_LABEL:
			return item.is_PP_ADVP_label();

		case IS_PP_SBAR_LABEL:
			return item.is_PP_SBAR_label();

		case IS_SBAR_ADJP_LABEL:
			return item.is_SBAR_ADJP_label();

		case IS_SBAR_ADJP_UCP_LABEL:
			return item.is_SBAR_ADJP_UCP_label();

		case IS_EX_PRP_QP_LABEL:
			return item.is_EX_PRP_QP_label();

		case IS_DT_PRPP_LABEL:
			return item.is_DT_PRPP_label();

		case IS_VBP_MD_LABEL:
			return item.is_VBP_MD_label();

		case IS_MD_TO_LABEL:
			return item.is_MD_TO_label();

		case IS_TO_IN_LABEL:
			return item.is_TO_IN_label();

		case IS_WDT_WP_LABEL:
			return item.is_WDT_WP_label();

		case IS_HASH_DOLLAR_LABEL:
			return item.is_HASH_DOLLAR_label();

		case IS_S_SINV_SBARQ_FRAG_LABEL:
			return item.is_S_SINV_SBARQ_FRAG_label();

		case IS_S_SINV_SBARQ_FRAG_SQ_X_LABEL:
			return item.is_S_SINV_SBARQ_FRAG_SQ_X_label();

		case IS_S_SINV_SBARQ_FRAG_SQ_X_INTJ_LABEL:
			return item.is_S_SINV_SBARQ_FRAG_SQ_X_INTJ_label();

		case IS_COMMA_COLON_LABEL:
			return item.is_COMMA_COLON_label();

		case IS_V_HEADTAG:
			return item.is_V_headtag();

		case IS_NN_HEADTAG:
			return item.is_NN_headtag();

		case IS_JJ_JJS_HEADTAG:
			return item.is_JJ_JJS_headtag();

		case IS_JJ_JJR_HEADTAG:
			return item.is_JJ_JJR_headtag();

		case IS_RB_RP_HEADTAG:
			return item.is_RB_RP_headtag();

		case IS_RBR_RBS_HEADTAG:
			return item.is_RBR_RBS_headtag();

		case IS_EX_PRP_HEADTAG:
			return item.is_EX_PRP_headtag();

		case IS_DT_PRPP_HEADTAG:
			return item.is_DT_PRPP_headtag();

		case IS_VBP_MD_HEADTAG:
			return item.is_VBP_MD_headtag();

		case IS_MD_TO_HEADTAG:
			return item.is_MD_TO_headtag();

		case IS_TO_IN_HEADTAG:
			return item.is_TO_IN_headtag();

		case IS_WDT_WP_HEADTAG:
			return item.is_WDT_WP_headtag();

		case IS_HASH_DOLLAR_HEADTAG:
			return item.is_HASH_DOLLAR_headtag();

		case IS_COMMA_COLON_HEADTAG:
			return item.is_COMMA_COLON_headtag();

		default: assert(0);
	}
}

const ItemToken* get_which_where_offset_item(which_span_ty which, span_where_ty where, int offset, const IntermediateExample& e) {
	const ItemTokens* items = NULL;
	int loc;

	sanity_check_which_span_where_ty(which, where);
	sanity_check_where_offset(where, offset);

	switch (which) {
		case LCONTEXT:
			items = &e.lcontext();
			break;

		case RCONTEXT:
			items = &e.rcontext();
			break;
		
		case SPAN:
			items = &e.span();
			break;

		default: assert(0);
	}
	assert(items);

	switch (where) {
		case START_ITEM:
			loc = 0;
			break;

		case HEAD_ITEM:
			assert(which == SPAN);
			loc = e.head();
			break;

		case END_ITEM:
			loc = items->size()-1;
			break;

		default: assert(0);
	}

	loc += offset;
	if (loc < 0 || loc >= (int)items->size()) return NULL;
	return items->at(loc);
}

const ItemToken* get_child_where_offset_item(span_where_ty where, int offset, const ItemToken& item) {
	if (item.is_terminal()) return NULL;
	
	const ItemTokens* children = &item.children();
	assert(children);
	
	int loc;

	sanity_check_child_where_offset(where, offset);

	switch (where) {
		case START_ITEM:
			loc = 0;
			break;

		case HEAD_ITEM:
			loc = item.head();
			break;

		case END_ITEM:
			loc = children->size()-1;
			break;

		default: assert(0);
	}

	loc += offset;
	if (loc < 0 || loc >= (int)children->size()) return NULL;
	return children->at(loc);
}

const ItemToken* get_range_group_offset_item(range_group_ty group, unsigned o, const IntermediateExample& e) {
	sanity_check_range_group_ty(group);

	// unsigned => signed cast
	int offset = o;
	assert(offset >= 1 && offset <= (int)max_group_range(group));

	switch(group) {
		case SPAN_START_RANGE:
			assert(!e.span().empty());
			if (offset-1 >= (int)e.span().size()) return NULL;
			return e.span().at((int)offset-1);
		case SPAN_END_RANGE:
			assert(!e.span().empty());
			if ((int)e.span().size()-offset < 0) return NULL;
			return e.span().at((int)e.span().size()-offset);
		case LEFT_OF_SPAN_HEAD_RANGE:
			assert(!e.span().empty());
			if ((int)e.head()-offset < 0) return NULL;
			return e.span().at((int)e.head()-offset);
		case RIGHT_OF_SPAN_HEAD_RANGE:
			assert(!e.span().empty());
			if ((int)e.head()+offset >= (int)e.span().size()) return NULL;
			return e.span().at((int)e.head()+offset);
		case LCONTEXT_RANGE:
			if (offset-1 >= (int)e.lcontext().size()) return NULL;
			return e.lcontext().at(offset-1);
		case RCONTEXT_RANGE:
			if (offset-1 >= (int)e.rcontext().size()) return NULL;
			return e.rcontext().at(offset-1);
		default: assert(0);
	}
}

const ItemToken* get_which_where_offset_terminal(which_span_ty which, span_where_ty where, int offset, const IntermediateExample& e) {
	const ItemTokens* items = NULL;
	ItemTokens items2;
	sanity_check_which_span_where_ty(which, where);
	sanity_check_terminal_where_offset(where, offset);

	int loc;

	if (which == LCONTEXT) {
		if (e.lcontext().empty()) return NULL;

		//items = &e.lcontext();
		// This is really slow
		for (ItemTokens::const_reverse_iterator i = e.lcontext().rbegin();
				i != e.lcontext().rend(); i++)
			items2.push_back(*i);
		items = &items2;

		assert(where == START_ITEM);
		loc = items->span().right() - offset;
	} else if (which == RCONTEXT) {
		if (e.rcontext().empty()) return NULL;

		items = &e.rcontext();
		assert(where == START_ITEM);
		loc = items->span().left() + offset;
	} else if (which == SPAN) {
		if (e.span().empty()) return NULL;

		items = &e.span();
		switch (where) {
			case START_ITEM:
				loc = items->span().left() + offset;
				break;

			case HEAD_ITEM:
				loc = e.head_terminal().span().left() + offset;
				break;

			case END_ITEM:
				loc = items->span().right() + offset;
				break;

			default: assert(0);
		}
	} else { assert(0); }
	assert(items);

	if (loc < 0 || loc > (int)e.state().span().right()) return NULL;

	Span s(loc);
	// We must be below items for this feature to be
	// valid.
	if (!(s < items->span() || s == items->span()))
		return NULL;
	
	return &e.state().locate_terminal(s);
}

/// Get the items in a certain group from an example.
/// \param group The group of items to retrieve.
/// \param e The example from which to retrieve the items.
/// \param items The list into which the items are retrieved.
void get_group_items(item_group_ty group, const IntermediateExample& e, ItemTokens& items) {
	ItemTokens::const_iterator i;
	unsigned j;

	switch (group) {
		case ALL_ITEMS:
			items = e.frontier();
			break;

		case ALL_LCONTEXT_ITEMS:
			items = e.lcontext();
			break;

		case ALL_RCONTEXT_ITEMS:
			items = e.rcontext();
			break;

		case ALL_SPAN_ITEMS:
			items = e.span();
			break;

		case NONLEFTMOST_SPAN_ITEMS:
			items.clear();
			for (i = e.span().begin()+1; i != e.span().end(); i++)
				items.push_back(*i);
			break;

		case NONRIGHTMOST_SPAN_ITEMS:
			items.clear();
			for (i = e.span().begin(); i != e.span().end()-1; i++)
				items.push_back(*i);
			break;

		case NONHEAD_SPAN_ITEMS:
			items.clear();
			for (i = e.span().begin(), j = 0; i != e.span().end(); i++, j++) {
				if (j == e.head())
					continue;
				items.push_back(*i);
			}
			break;

		case LEFT_OF_HEAD_SPAN_ITEMS:
			items.clear();
			for (i = e.span().begin(), j = 0; i != e.span().end(); i++, j++) {
				if (j >= e.head())
					continue;
				items.push_back(*i);
			}
			break;

		case RIGHT_OF_HEAD_SPAN_ITEMS:
			items.clear();
			for (i = e.span().begin(), j = 0; i != e.span().end(); i++, j++) {
				if (j <= e.head())
					continue;
				items.push_back(*i);
			}
			break;

		case HEAD_OR_LEFT_OF_HEAD_SPAN_ITEMS:
			items.clear();
			for (i = e.span().begin(), j = 0; i != e.span().end(); i++, j++) {
				if (j > e.head())
					continue;
				items.push_back(*i);
			}
			break;

		case HEAD_OR_RIGHT_OF_HEAD_SPAN_ITEMS:
			items.clear();
			for (i = e.span().begin(), j = 0; i != e.span().end(); i++, j++) {
				if (j < e.head())
					continue;
				items.push_back(*i);
			}
			break;

		default: assert(0);
	}
}

/// Get terminals dominated by the items in a certain group from an example.
/// \param group The group of items which terminals to retrieve.
/// \param e The example from which to retrieve the items+terminals.
/// \param terminals The list into which the terminals are retrieved.
void get_group_terminals(item_group_ty group, const IntermediateExample& e, ItemTokens& terminals) {
	ItemTokens items;
	get_group_items(group, e, items);
	
	terminals.clear();
	for (ItemTokens::const_iterator i = items.begin(); i != items.end(); i++) {
		const ItemToken& item = **i;
		item.get_terminals(terminals);
	}
}

/*
/// Uniq the values of a feature list.
/// \param flist The feature list to be uniq'ed.
/// \todo Faster just to work with hash_sets directly?
void uniq_FeaturePtrs(FeaturePtrs& flist) {
	hash_set<FeaturePtr, hash<FeaturePtr>, equal_to<FeaturePtr>> fset;
        fset.insert(flist.begin(), flist.end());

	flist.clear();
	flist.insert(flist.begin(), fset.begin(), fset.end());
	assert(flist.size() == fset.size());
}
*/

const LIST<unsigned>& all_bool() {
	return cached_bool_values;
}

/*
 *
 *  Getting a function ptr to a non-static C++ member function is tricky.
 *  ( cf. http://www.function-pointer.org/callback.html#member)
 *  
//void add_predicate(item_predicate_ty predicate, string str, const LIST<unsigned>& (*all_values_ptr)(), bool (*get_value)(const ItemToken& i)) {
void add_predicate(item_predicate_ty predicate, string str, const LIST<unsigned>& (*all_values_ptr)(), bool (*get_value)() const) {
}

void init_predicates() {
	add_predicate(IS_TERMINAL_PREDICATE, "is_terminal?", all_bool, ItemToken::is_terminal);
	add_predicate(LABEL_PREDICATE, "label", all_labels, ItemToken::label);
	add_predicate(HEADLABEL_PREDICATE, "headlabel", all_constituent_labels, ItemToken::headlabel);
	add_predicate(HEADTAG_PREDICATE, "headtag", all_terminal_labels, ItemToken::headtag);
	add_predicate(HEADTAGCLASS_PREDICATE, "headtagclass", all_posclass, ItemToken::headtagclass);
	add_predicate(HEADWORD_PREDICATE, "headword", all_words, ItemToken::headword);
	add_predicate(EXISTS_PREDICATE, "exists?", all_bool, ItemToken::exists);
	add_predicate(IS_VERBAL_HEADTAG_PREDICATE, "is_verbal_headtag?", all_bool, ItemToken::is_verbal_headtag);
	add_predicate(IS_VERBAL_LABEL_PREDICATE, "is_verbal_label?", all_bool, ItemToken::is_verbal_label);
	add_predicate(DOMINATES_SOME_VERB_PREDICATE, "dominates_some_verb?", all_bool, ItemToken::dominates_some_verb);
	add_predicate(ARE_ALL_CHILDREN_TERMINALS_PREDICATE, "are_all_children_terminals?", all_bool, ItemToken::are_all_children_terminals);
	add_predicate(ONLY_ONE_CHILD_PREDICATE, "only_one_child?", all_bool, ItemToken::only_one_child);
	add_predicate(IS_RIGHT_RECURSIVE_NP_PREDICATE, "is_right_recursive_NP?", all_bool, ItemToken::is_right_recursive_NP);
	add_predicate(IS_POSSESSIVE_NP_KLEIN_MANNING_PREDICATE, "is_possessive_NP_Klein_Manning?", all_bool, ItemToken::is_possessive_NP_Klein_Manning);
	add_predicate(IS_POSSESSIVE_NP_MACINTYRE_PREDICATE, "is_possessive_NP_MacIntyre?", all_bool, ItemToken::is_possessive_NP_MacIntyre);
	add_predicate(IS_GAPPED_S_PREDICATE, "is_gapped_S?", all_bool, ItemToken::is_gapped_S);
	add_predicate(IS_BASAL_NP_PREDICATE, "is_basal_NP?", all_bool, ItemToken::is_basal_NP);
	add_predicate(IS_COORDINATED_PHRASE_PREDICATE, "is_coordinated_phrase?", all_bool, ItemToken::is_coordinated_phrase);
	add_predicate(IS_CONTENT_WORD, "is_content_word?", all_bool, ItemToken::is_content_word);
	add_predicate(IS_NOUN, "is_noun?", all_bool, ItemToken::is_noun);
	add_predicate(IS_NOUN_PHRASE, "is_noun_phrase?", all_bool, ItemToken::is_noun_phrase);
	add_predicate(IS_WH_LABEL, "is_WH_label", all_bool, ItemToken::is_WH_label);
	add_predicate(IS_V_LABEL, "is_V_label", all_bool, ItemToken::is_V_label);
	add_predicate(IS_NN_LABEL, "is_NN_label", all_bool, ItemToken::is_NN_label);
	add_predicate(IS_JJ_JJS_LABEL, "is_JJ_JJS_label", all_bool, ItemToken::is_JJ_JJS_label);
	add_predicate(IS_JJ_JJR_LABEL, "is_JJ_JJR_label", all_bool, ItemToken::is_JJ_JJR_label);
	add_predicate(IS_RB_RP_LABEL, "is_RB_RP_label", all_bool, ItemToken::is_RB_RP_label);
	add_predicate(IS_RBR_RBS_LABEL, "is_RBR_RBS_label", all_bool, ItemToken::is_RBR_RBS_label);
	add_predicate(IS_PP_ADVP_LABEL, "is_PP_ADVP_label", all_bool, ItemToken::is_PP_ADVP_label);
	add_predicate(IS_PP_SBAR_LABEL, "is_PP_SBAR_label", all_bool, ItemToken::is_PP_SBAR_label);
	add_predicate(IS_SBAR_ADJP_LABEL, "is_SBAR_ADJP_label", all_bool, ItemToken::is_SBAR_ADJP_label);
	add_predicate(IS_SBAR_ADJP_UCP_LABEL, "is_SBAR_ADJP_UCP_label", all_bool, ItemToken::is_SBAR_ADJP_UCP_label);
	add_predicate(IS_EX_PRP_QP_LABEL, "is_EX_PRP_QP_label", all_bool, ItemToken::is_EX_PRP_QP_label);
	add_predicate(IS_DT_PRPP_LABEL, "is_DT_PRPP_label", all_bool, ItemToken::is_DT_PRPP_label);
	add_predicate(IS_VBP_MD_LABEL, "is_VBP_MD_label", all_bool, ItemToken::is_VBP_MD_label);
	add_predicate(IS_MD_TO_LABEL, "is_MD_TO_label", all_bool, ItemToken::is_MD_TO_label);
	add_predicate(IS_TO_IN_LABEL, "is_TO_IN_label", all_bool, ItemToken::is_TO_IN_label);
	add_predicate(IS_WDT_WP_LABEL, "is_WDT_WP_label", all_bool, ItemToken::is_WDT_WP_label);
	add_predicate(IS_HASH_DOLLAR_LABEL, "is_HASH_DOLLAR_label", all_bool, ItemToken::is_HASH_DOLLAR_label);
	add_predicate(IS_S_SINV_SBARQ_FRAG_LABEL, "is_S_SINV_SBARQ_FRAG_label", all_bool, ItemToken::is_S_SINV_SBARQ_FRAG_label);
	add_predicate(IS_S_SINV_SBARQ_FRAG_SQ_X_LABEL, "is_S_SINV_SBARQ_FRAG_SQ_X_label", all_bool, ItemToken::is_S_SINV_SBARQ_FRAG_SQ_X_label);
	add_predicate(IS_S_SINV_SBARQ_FRAG_SQ_X_INTJ_LABEL, "is_S_SINV_SBARQ_FRAG_SQ_X_INTJ_label", all_bool, ItemToken::is_S_SINV_SBARQ_FRAG_SQ_X_INTJ_label);
	add_predicate(IS_COMMA_COLON_LABEL, "is_COMMA_COLON_label", all_bool, ItemToken::is_COMMA_COLON_label);
	add_predicate(IS_V_HEADTAG, "is_V_headtag", all_bool, ItemToken::is_V_headtag);
	add_predicate(IS_NN_HEADTAG, "is_NN_headtag", all_bool, ItemToken::is_NN_headtag);
	add_predicate(IS_JJ_JJS_HEADTAG, "is_JJ_JJS_headtag", all_bool, ItemToken::is_JJ_JJS_headtag);
	add_predicate(IS_JJ_JJR_HEADTAG, "is_JJ_JJR_headtag", all_bool, ItemToken::is_JJ_JJR_headtag);
	add_predicate(IS_RB_RP_HEADTAG, "is_RB_RP_headtag", all_bool, ItemToken::is_RB_RP_headtag);
	add_predicate(IS_RBR_RBS_HEADTAG, "is_RBR_RBS_headtag", all_bool, ItemToken::is_RBR_RBS_headtag);
	add_predicate(IS_EX_PRP_HEADTAG, "is_EX_PRP_headtag", all_bool, ItemToken::is_EX_PRP_headtag);
	add_predicate(IS_DT_PRPP_HEADTAG, "is_DT_PRPP_headtag", all_bool, ItemToken::is_DT_PRPP_headtag);
	add_predicate(IS_VBP_MD_HEADTAG, "is_VBP_MD_headtag", all_bool, ItemToken::is_VBP_MD_headtag);
	add_predicate(IS_MD_TO_HEADTAG, "is_MD_TO_headtag", all_bool, ItemToken::is_MD_TO_headtag);
	add_predicate(IS_TO_IN_HEADTAG, "is_TO_IN_headtag", all_bool, ItemToken::is_TO_IN_headtag);
	add_predicate(IS_WDT_WP_HEADTAG, "is_WDT_WP_headtag", all_bool, ItemToken::is_WDT_WP_headtag);
	add_predicate(IS_HASH_DOLLAR_HEADTAG, "is_HASH_DOLLAR_headtag", all_bool, ItemToken::is_HASH_DOLLAR_headtag);
	add_predicate(IS_COMMA_COLON_HEADTAG, "is_COMMA_COLON_headtag", all_bool, ItemToken::is_COMMA_COLON_headtag);
}
*/
