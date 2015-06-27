/*  $Id: features-help.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file features-help.H
 *  \brief Helper functions for feature code.
 *
 *  Also includes enumerated values used by features.
 *  WRITEME
 *
 *  \todo Make these functions into a class.
 *  \todo Make the enumerated value types into classes, to ensure type
 *  safety and add helper functions like to_string().
 *  \todo Compose some of these enumerated values into fewer, removing
 *  redundant ones.
 *
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#ifndef __features_help_H__
#define  __features_help_H__

#include "learn/features/Features.H"

#include "universal/globals.H"		// For LIST

#include <cstddef>
#include <list>				// REMOVEME
#include <ext/hash_set>
#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
#endif /* DOXYGEN */

class ItemToken;
class ItemTokens;
class IntermediateExample;

/// \todo Rename to which_group_ty or which_items_ty?
typedef enum { LCONTEXT, RCONTEXT, SPAN } which_span_ty;

/// \todo Rename to span_where_ty or items_where_ty?
typedef enum { START_ITEM, HEAD_ITEM, END_ITEM } span_where_ty;

/*
/// for RangeFeatures
/// \todo Allow RangeFeatures both to include head and to not include head in some range wrt head item.
typedef enum { SPAN_START_ITEM, SPAN_HEAD_ITEM, SPAN_END_ITEM, LCONTEXT_START_ITEM, RCONTEXT_START_ITEM } which_item_ty;
/// \todo Add LEFT_OR_RIGHT_OF direction
typedef enum { LEFT_OF, RIGHT_OF } direction_ty;
*/

/// for RangeFeatures
/// \todo Add LEFT_AND_RIGHT_OF_SPAN_HEAD_RANGE
/// \todo Allow RangeFeatures both to include head and to not include head in some range wrt head item. (i.e. not just exclusive of span head, inclusive too)
typedef enum { SPAN_START_RANGE, SPAN_END_RANGE, LEFT_OF_SPAN_HEAD_RANGE, RIGHT_OF_SPAN_HEAD_RANGE, LCONTEXT_RANGE, RCONTEXT_RANGE } range_group_ty;

/// Types of item feature queries.
///	- IS_TERMINAL_PREDICATE:	Is this a terminal item?
///	- LABEL_PREDICATE:		The label of this item.
///	- HEADLABEL_PREDICATE:		The headlabel of this item.
///	- HEADTAG_PREDICATE:		The headtag of this item.
///	- HEADTAGCLASS_PREDICATE:	The headtagclass of this item.
///	- HEADWORD_PREDICATE:		The headword of this item.
/// \todo Describe others.
typedef enum { IS_TERMINAL_PREDICATE, LABEL_PREDICATE, HEADLABEL_PREDICATE, HEADTAG_PREDICATE, HEADTAGCLASS_PREDICATE, HEADWORD_PREDICATE, EXISTS_PREDICATE, IS_VERBAL_HEADTAG_PREDICATE, IS_VERBAL_LABEL_PREDICATE, DOMINATES_SOME_VERB_PREDICATE, ARE_ALL_CHILDREN_TERMINALS_PREDICATE, ONLY_ONE_CHILD_PREDICATE, IS_RIGHT_RECURSIVE_NP_PREDICATE, IS_POSSESSIVE_NP_KLEIN_MANNING_PREDICATE, IS_POSSESSIVE_NP_MACINTYRE_PREDICATE, IS_GAPPED_S_PREDICATE, IS_BASAL_NP_PREDICATE, IS_COORDINATED_PHRASE_PREDICATE, IS_CONTENT_WORD, IS_NOUN, IS_NOUN_PHRASE,
IS_WH_LABEL, IS_V_LABEL, IS_NN_LABEL, IS_JJ_JJS_LABEL, IS_JJ_JJR_LABEL, IS_RB_RP_LABEL, IS_RBR_RBS_LABEL, IS_PP_ADVP_LABEL, IS_PP_SBAR_LABEL, IS_SBAR_ADJP_LABEL, IS_SBAR_ADJP_UCP_LABEL, IS_EX_PRP_QP_LABEL, IS_DT_PRPP_LABEL, IS_VBP_MD_LABEL, IS_MD_TO_LABEL, IS_TO_IN_LABEL, IS_WDT_WP_LABEL, IS_HASH_DOLLAR_LABEL, IS_S_SINV_SBARQ_FRAG_LABEL, IS_S_SINV_SBARQ_FRAG_SQ_X_LABEL, IS_S_SINV_SBARQ_FRAG_SQ_X_INTJ_LABEL, IS_COMMA_COLON_LABEL,
IS_V_HEADTAG, IS_NN_HEADTAG, IS_JJ_JJS_HEADTAG, IS_JJ_JJR_HEADTAG, IS_RB_RP_HEADTAG, IS_RBR_RBS_HEADTAG, IS_EX_PRP_HEADTAG, IS_DT_PRPP_HEADTAG, IS_VBP_MD_HEADTAG, IS_MD_TO_HEADTAG, IS_TO_IN_HEADTAG, IS_WDT_WP_HEADTAG, IS_HASH_DOLLAR_HEADTAG, IS_COMMA_COLON_HEADTAG } item_predicate_ty;

/// All predicates for terminal items.
/// These must be a subset of the item_predicate_ty predicates.
typedef item_predicate_ty terminal_predicate_ty;

typedef enum { ITEM_COUNT, TERMINAL_COUNT } length_count_ty;
typedef enum { GREATER_THAN, EQUAL_TO } length_cmp_ty;

typedef enum { THERE_EXISTS, NOT_ALL } quant_ty;
typedef enum { ALL_ITEMS, ALL_LCONTEXT_ITEMS, ALL_RCONTEXT_ITEMS, ALL_SPAN_ITEMS, NONLEFTMOST_SPAN_ITEMS, NONRIGHTMOST_SPAN_ITEMS, NONHEAD_SPAN_ITEMS, LEFT_OF_HEAD_SPAN_ITEMS, RIGHT_OF_HEAD_SPAN_ITEMS, HEAD_OR_LEFT_OF_HEAD_SPAN_ITEMS, HEAD_OR_RIGHT_OF_HEAD_SPAN_ITEMS } item_group_ty;

size_t feature_values_hash(int x1);
size_t feature_values_hash(int x1, int x2);
size_t feature_values_hash(int x1, int x2, int x3);
size_t feature_values_hash(int x1, int x2, int x3, int x4);
size_t feature_values_hash(int x1, int x2, int x3, int x4, int x5);
size_t feature_values_hash(int x1, int x2, int x3, int x4, int x5, int x6);
size_t feature_values_hash(int x1, int x2, int x3, int x4, int x5, int x6, int x7);

void sanity_check_which_span_ty(which_span_ty x);
void sanity_check_span_where_ty(span_where_ty x);
void sanity_check_which_span_where_ty(which_span_ty which, span_where_ty where);
void sanity_check_item_predicate_ty(item_predicate_ty x);
void sanity_check_terminal_predicate_ty(terminal_predicate_ty x);
void sanity_check_length_count_ty(length_count_ty x);
void sanity_check_length_cmp_ty(length_cmp_ty x);
void sanity_check_quant_ty(quant_ty x);
void sanity_check_item_group_ty(item_group_ty x);

void sanity_check_range_group_ty(range_group_ty x);

void sanity_check_predicate_and_value(item_predicate_ty predicate, unsigned value);
void sanity_check_where_offset(span_where_ty where, int offset);
void sanity_check_terminal_where_offset(span_where_ty where, int offset);
void sanity_check_child_where_offset(span_where_ty where, int offset);

/// Sanity check that the features in a list are unique.
/// \param flist The feature list to be uniq'ed.
/// \todo Remove the specific version of this function, keep only the template version.
template<class T> void sanity_check_uniq(const LIST<T>& flist);
template<class T> void sanity_check_uniq(const deque<T>& flist);
template<class T> void sanity_check_uniq(const list<T>& flist);

bool is_binary_predicate(item_predicate_ty predicate);

const string which_span_string(which_span_ty which);
const string where_offset_string(span_where_ty where, int offset);
const string predicate_value_string(item_predicate_ty predicate, unsigned value);
const string item_group_string(item_group_ty group);
const string range_string(range_group_ty group, unsigned range);



const LIST<which_span_ty>& all_which_span();
const LIST<span_where_ty>& all_span_where();
const LIST<span_where_ty>& all_span_where(which_span_ty which);
const LIST<item_predicate_ty>& all_item_predicates();
const LIST<terminal_predicate_ty>& all_terminal_predicates();
const LIST<item_group_ty>& all_item_groups();
const LIST<range_group_ty>& all_range_groups();

const LIST<int>& all_where_offsets(span_where_ty where);
const LIST<int>& all_child_where_offsets(span_where_ty where);

unsigned max_group_range(range_group_ty group);

/// Find all possible values for some predicate.
/// \param predicate The predicate which values we want.
/// \return A list of all possible values of this predicate.
/// \todo Do we really want special case handling for boolean predicates?
/// Reason in favor of special case handling for boolean predicates:
///	- We don't consider redundant features. Only one suffices.
/// Reason against special case handling for boolean predicates:
///	- Cleaner, simpler code.
const LIST<unsigned>& all_predicate_values(item_predicate_ty predicate);

/// Retrieve the maximum possible value for some predicate.
/// \todo This is really ugly. Outsource the work to methods in vocabulary.C posclass.C.
unsigned max_predicate_value(item_predicate_ty predicate);

/// Determine if a predicate holds on an item.
/// \param predicate The predicate type to be checked.
/// \param value The desired value of the predicate.
/// \param item The item to check the predicate value.
/// \return True iff the example has the desired predicate value.
bool predicate_holds(item_predicate_ty predicate, unsigned value, const ItemToken& item);

unsigned get_predicate_value(item_predicate_ty predicate, const ItemToken& item);

const ItemToken* get_which_where_offset_item(which_span_ty which, span_where_ty where, int offset, const IntermediateExample& e);
const ItemToken* get_child_where_offset_item(span_where_ty where, int offset, const ItemToken& item);
const ItemToken* get_range_group_offset_item(range_group_ty group, unsigned offset, const IntermediateExample& e);
const ItemToken* get_which_where_offset_terminal(which_span_ty which, span_where_ty where, int offset, const IntermediateExample& e);

/// Get the items in a certain group from an example.
/// \param group The group of items to retrieve.
/// \param e The example from which to retrieve the items.
/// \param items The list into which the items are retrieved.
void get_group_items(item_group_ty group, const IntermediateExample& e, ItemTokens& items);

/// Get terminals dominated by the items in a certain group from an example.
/// \param group The group of items which terminals to retrieve.
/// \param e The example from which to retrieve the items+terminals.
/// \param terminals The list into which the terminals are retrieved.
void get_group_terminals(item_group_ty group, const IntermediateExample& e, ItemTokens& terminals);

/*
/// Uniq the values of a feature list.
/// \param flist The feature list to be uniq'ed.
/// \todo Faster just to work with hash_sets directly?
void uniq_FeaturePtrs(FeaturePtrs& flist);
*/

/// Sanity check that the features in a list are unique.
/// \param flist The feature list to be uniq'ed.
template<class T> void sanity_check_uniq(const LIST<T>& flist) {
	hash_set<T, hash<T>, equal_to<T> > fset;
        fset.insert(flist.begin(), flist.end());
	assert(flist.size() == fset.size());
}
template<class T> void sanity_check_uniq(const deque<T>& flist) {
	hash_set<T, hash<T>, equal_to<T> > fset;
        fset.insert(flist.begin(), flist.end());
	assert(flist.size() == fset.size());
}
template<class T> void sanity_check_uniq(const list<T>& flist) {
	hash_set<T, hash<T>, equal_to<T> > fset;
        fset.insert(flist.begin(), flist.end());
	assert(flist.size() == fset.size());
}

const LIST<unsigned>& all_bool();

#endif /* __features_help_H__ */
