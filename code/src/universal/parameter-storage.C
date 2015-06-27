/*  $Id: parameter-storage.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file parameter-storage.C
 *  \brief Storage of parameter%s member values.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "universal/parameter.H"

#include <string>
#include <cassert>
#include <cmath>

/// Before doing any training, should we cache all Example Feature vectors?
/// i.e. Should we convert all Example%s to SparseFullExample%s?
/// The cost is roughly 720 bytes * # examples.
bool parameter::_cache_example_feature_vectors_before_training = false;

/// Before building a decision tree, should we cache Feature vectors for all
/// Example%s in the sample?
/// \invariant cache_example_feature_vectors_before_decision_tree_building =>
/// downsample_examples_during_feature_selection
bool parameter::_cache_example_feature_vectors_before_decision_tree_building = false;


/// The minimum confidence for us to greedily perform a decision at some
/// state, without first evaluating the other candidates
double parameter::_minconf_for_greedy_decision = 1000;

/// Treat each state as having weight 1, and give half this weight
/// to the positive examples therein and half this weight to the negative
/// examples therein.
/// If false, all examples are equally weighted.
bool parameter::_share_example_weight_per_state = true;

string parameter::_vocabulary_filename = "/home/turian/parsing/FOOBAR/data/vocabulary.txt";
string parameter::_label_filename = "/home/turian/parsing/FOOBAR/data/labels.txt";

/// A *positive* decision tree leaf must have examples from at least
/// this many different sentences to be considered valid.
/// \warning This only applies during decision tree construction.
/// It may not hold leaf confidence-rating.
unsigned parameter::_min_sentences_per_feature = 5;

bool parameter::_use_item_features = true;
bool parameter::_use_length_features = true;
bool parameter::_use_quant_features = false;
bool parameter::_use_exists_features = true;
bool parameter::_use_exists_terminal_features = true;
bool parameter::_use_item_child_features = true;
bool parameter::_context_groups_for_exists_features = true;
bool parameter::_context_groups_for_exists_terminal_features = true;
bool parameter::_length_of_context_items_features = true;
bool parameter::_use_range_features = false;
bool parameter::_use_terminal_item_features = false;

/// Allow equality tests over lengths?
/// (otherwise, we just do 'greater than' tests)
bool parameter::_length_equal_to_feature = false;

/// Use posclass predicate
/// \invariant use_redundant_feature_predicates => use_posclass
bool parameter::_use_posclass = true;

/// Use binary predicates that haven't been tested by other discriminative
/// parser (e.g. is_basal_NP)
bool parameter::_use_esoteric_feature_predicates = false;

/// Use distributionally-clustered binary predicates
/// (e.g. is_S_SINV_SBARQ_FRAG).
bool parameter::_use_clustered_feature_predicates = true;

/// Use predicates that are aggregates of other predicates (typically,
/// super-classes of POS + label sets)
/// \invariant use_esoteric_feature_predicates => use_redundant_feature_predicates
/// \invariant use_clustered_feature_predicates => use_redundant_feature_predicates
bool parameter::_use_redundant_feature_predicates = true;


bool parameter::_use_context_features = true;

/// Don't use any features over terminals
/// \invariant use_exists_terminal_features => use_terminal_features
bool parameter::_use_terminal_features = true;

/// True iff we want to construct redundant features.
/// Useful only for reading in obsolete hypotheses.
bool parameter::_construct_redundant_features = false;

bool parameter::_binary_predicates_can_have_false_value = false;


/// Only allow parse decisions in a left-to-right fashion?
/// In other words, only allow an inference if there are no non-terminal
/// constituents to its right in the state?
/// \invariant !(parse_left_to_right and parse_right_to_left)
bool parameter::_parse_left_to_right = false;

/// Only allow parse decisions in a right-to-left fashion?
/// In other words, only allow an inference if there are no non-terminal
/// constituents to its left in the state?
/// \invariant !(parse_left_to_right and parse_right_to_left)
bool parameter::_parse_right_to_left = false;

/// When parsing l2r/r2l (or generating training examples),
/// consider all actions as legal even if they violate the l2r/r2l
/// ordering?
bool parameter::_consider_all_examples_despite_r2l_or_l2r = false;

/// True iff the treebank contains a fixed parse path.
/// If this is the case, then consistent parsing---typically used for
/// example generation---will always take the treebank path (and hence always
/// generate the same examples).
bool parameter::_treebank_has_parse_paths = true;

/// Should we downsample the example set during feature selection
/// (decision-tree building, but not weighting)?
bool parameter::_downsample_examples_during_feature_selection = false;

/// Maximum example size to use when downsampling examples for feature selection.
/// At most half this value will be positive examples, and at most half
/// will be negative.
/// The sampling method is based upon "Sampling to estimate
/// arbitrary subset sums" (Duffield et al.).
/// (decision-tree building, but not weighting)?
unsigned parameter::_downsample_examples_during_feature_selection_sample_size = 100000;



loss_ty parameter::_loss_type = LOGISTIC_LOSS;

/// Given that we have the gain measures for a node, only allow the node
/// to be split if the sum of the gains of the children is at least
/// min_gain_factor * the gain of the node.
double parameter::_min_gain_factor_for_split = 1.00;

/// If we are using the l1 penalty, should we vary the penalty term?
/// \warning The penalty may be too aggressive; If an earlier tree
/// split used the same feature, we nonetheless treat them as independent
/// in computing the magnitude of the confidence.
/// \invariant vary_l1_penalty => initial_l1_penalty_factor == final_l1_penalty_factor
bool parameter::_vary_l1_penalty = true;

/// Initial l1 penalty scaling factor
double parameter::_initial_l1_penalty_factor = 1e7;

/// Final l1 penalty scaling factor
double parameter::_final_l1_penalty_factor = 0;

/// When we reduce the l1 penalty, we reduce it to the maximum threshold,
/// downweighted by l1_penalty_factor_overscale
double parameter::_l1_penalty_factor_overscale = 1;

/// Normalize feature counts to mean 0, variance 1 before applying
/// the l1 penalty.
/// (from Riezler, ACL)
/// We only do this during the feature selection (tree building)
/// stage, not the confidence-rating stage.
/// If true, the gain of some node is scaled by sqrt(p * (1-p)),
/// where p, 0<=p<=1, is the weighted probability of an example
/// ending up in this node.
bool parameter::_normalize_feature_counts = false;

/// l1 penalty scaling factor
/// \warning This is not a parameter that can be loaded from a parameters
/// file.
double parameter::_l1_penalty_factor = parameter::_initial_l1_penalty_factor;

/// Beam search width, i.e. the maximum number of partial parse states we
/// keep around during parsing.
unsigned parameter::_beam_search_width = 100000;

/// Maximum number of partial parse states we pop from the beam during parsing.
unsigned parameter::_max_search_states_to_pop = 100000;

/// During search, minimum confidence for an action to be better than no action.
double parameter::_confidence_better_than_no_action = 0;

/// Constant epsilon, used for determine negligible differences
//double parameter::_small_epsilon = 1e-16;
double parameter::_small_epsilon = 1e-8;

unsigned parameter::_num_labels = 27;	///< The number of different labels

unsigned parameter::_precision = 12;	///< Precision of floating-point output

unsigned parameter::_max_iterations = 99999;	///< The maximum number of training iterations

/// Global debug level.
///  - 0: No debug messages, quiet mode
///  - 1: A few important debug messages, or very useful O(n) messages.
///  - 2: More debug messages, but not more than a few screens worth
///	(i.e. O(n), but with low constant term)
///  - 3: Get swamped
//unsigned parameter::_debuglevel = 3;	///< Global debug level
unsigned parameter::_debuglevel = 2;	///< Global debug level

/// Maximum length of any example span
unsigned parameter::_max_span_length = 5;

/// Maximum offset of any item in an ItemFeature
/// \todo WRITEME. Describe this better
unsigned parameter::_max_offset = 2;

/// Maximum offset of any *context* item in an ItemFeature
/// i.e. max_context_offset + 1 is the number of context items that can be examined
int parameter::_max_context_offset = 2;

/// Maximum offset of any terminal in a TerminalItemFeature
/// \todo WRITEME. Describe this better
unsigned parameter::_max_terminal_offset = 2;

/// Maximum offset of any *context* terminal in a TerminalItemFeature
/// i.e. max_context_offset + 1 is the number of context items that can be examined
int parameter::_max_terminal_context_offset = 2;

/// Maximum offset of any *child* item in an ItemFeature
/// \todo WRITEME. Describe this better
unsigned parameter::_max_child_offset = 0;
//unsigned parameter::_max_child_offset = 1;

/// Maximum (span) range in a RangeFeature
/// i.e. the maximum number of items selected over a range, inclusive.
/// \todo WRITEME. Describe this better
unsigned parameter::_max_range = 3;

/// Maximum (span) range in a RangeFeature over context
/// i.e. the maximum number of context items selected over a range, inclusive.
unsigned parameter::_max_context_range = 3;

/// Find the best leaf confidence, within this epsilon
//double parameter::_confidence_epsilon = 0.0000001;
double parameter::_confidence_epsilon = 0.0001;

// // Maximum depth of any decision tree node
unsigned parameter::_max_depth = 128;



/// Random seed
/// \todo Make this a long int?
unsigned parameter::_seed = 0;

/// Require an END token at the end of hypothesis files?
bool parameter::_need_END_token = true;

/// Disallow PRT label?
/// As per Magerman, Collins, &tc. convert non-terminal label PRT to
/// ADVP during preprocessing. If so, then the parser should explicitly disallow
/// PRT decisions, despite what (errant) hypotheses may say.
bool parameter::_convert_PRT_to_ADVP = true;

/// Raise punctuation nodes.
/// When punctuation nodes are raised, they are never the leftmost or
/// rightmost child of any node but the TOP (root) node.
bool parameter::_raise_punctuation = true;


/// Should we trace the paths taken by an example down a tree,
/// in computing its confidence?
/// \note Toggled internally.
bool parameter::_trace_confidence_paths = false;

/// For parallel update, should we accumulate example weights in leaves?
/// \note Toggled internally
bool parameter::_accumulate_weight_in_leaves = false;

/// Add all parameters to our parameter list.
void parameter::initialize() {
	// Don't do this more than once.
	if (initialized) return;
	assert(!write_locked);

	parameter::add("cache_example_feature_vectors_before_training", &_cache_example_feature_vectors_before_training, BOOL_TY);
	parameter::add("cache_example_feature_vectors_before_decision_tree_building", &_cache_example_feature_vectors_before_decision_tree_building, BOOL_TY);
	parameter::add("minconf_for_greedy_decision", &_minconf_for_greedy_decision, DOUBLE_TY);
	parameter::add("share_example_weight_per_state", &_share_example_weight_per_state, BOOL_TY);
	parameter::add("vocabulary_filename", &_vocabulary_filename, STRING_TY);
	parameter::add("label_filename", &_label_filename, STRING_TY);
	parameter::add("min_sentences_per_feature", &_min_sentences_per_feature, UNSIGNED_TY);
	parameter::add("use_item_features", &_use_item_features, BOOL_TY);
	parameter::add("use_length_features", &_use_length_features, BOOL_TY);
	parameter::add("use_quant_features", &_use_quant_features, BOOL_TY);
	parameter::add("use_exists_features", &_use_exists_features, BOOL_TY);
	parameter::add("use_exists_terminal_features", &_use_exists_terminal_features, BOOL_TY);
	parameter::add("use_item_child_features", &_use_item_child_features, BOOL_TY);
	parameter::add("context_groups_for_exists_features", &_context_groups_for_exists_features, BOOL_TY);
	parameter::add("context_groups_for_exists_terminal_features", &_context_groups_for_exists_terminal_features, BOOL_TY);
	parameter::add("length_of_context_items_features", &_length_of_context_items_features, BOOL_TY);
	parameter::add("use_range_features", &_use_range_features, BOOL_TY);
	parameter::add("use_terminal_item_features", &_use_terminal_item_features, BOOL_TY);
	parameter::add("length_equal_to_feature", &_length_equal_to_feature, BOOL_TY);
	parameter::add("use_posclass", &_use_posclass, BOOL_TY);
	parameter::add("use_esoteric_feature_predicates", &_use_esoteric_feature_predicates, BOOL_TY);
	parameter::add("use_clustered_feature_predicates", &_use_clustered_feature_predicates, BOOL_TY);
	parameter::add("use_redundant_feature_predicates", &_use_redundant_feature_predicates, BOOL_TY);
	parameter::add("use_context_features", &_use_context_features, BOOL_TY);
	parameter::add("use_terminal_features", &_use_terminal_features, BOOL_TY);
	parameter::add("construct_redundant_features", &_construct_redundant_features, BOOL_TY);
	parameter::add("binary_predicates_can_have_false_value", &_binary_predicates_can_have_false_value, BOOL_TY);
	parameter::add("parse_left_to_right", &_parse_left_to_right, BOOL_TY);
	parameter::add("parse_right_to_left", &_parse_right_to_left, BOOL_TY);
	parameter::add("consider_all_examples_despite_r2l_or_l2r", &_consider_all_examples_despite_r2l_or_l2r, BOOL_TY);
	parameter::add("treebank_has_parse_paths", &_treebank_has_parse_paths, BOOL_TY);
	parameter::add("downsample_examples_during_feature_selection", &_downsample_examples_during_feature_selection, BOOL_TY);
	parameter::add("downsample_examples_during_feature_selection_sample_size", &_downsample_examples_during_feature_selection_sample_size, UNSIGNED_TY);
	parameter::add("loss_type", &_loss_type, LOSS_TY);
	parameter::add("min_gain_factor_for_split", &_min_gain_factor_for_split, DOUBLE_TY);
	parameter::add("vary_l1_penalty", &_vary_l1_penalty, BOOL_TY);
	parameter::add("initial_l1_penalty_factor", &_initial_l1_penalty_factor, DOUBLE_TY);
	parameter::add("final_l1_penalty_factor", &_final_l1_penalty_factor, DOUBLE_TY);
	parameter::add("l1_penalty_factor_overscale", &_l1_penalty_factor_overscale, DOUBLE_TY);
	parameter::add("normalize_feature_counts", &_normalize_feature_counts, BOOL_TY);
	parameter::add("beam_search_width", &_beam_search_width, UNSIGNED_TY);
	parameter::add("max_search_states_to_pop", &_max_search_states_to_pop, UNSIGNED_TY);
	parameter::add("confidence_better_than_no_action", &_confidence_better_than_no_action, DOUBLE_TY);
	parameter::add("small_epsilon", &_small_epsilon, DOUBLE_TY);
	parameter::add("num_labels", &_num_labels, UNSIGNED_TY);
	parameter::add("precision", &_precision, UNSIGNED_TY);
	parameter::add("max_iterations", &_max_iterations, UNSIGNED_TY);
	parameter::add("debuglevel", &_debuglevel, UNSIGNED_TY);
	parameter::add("max_span_length", &_max_span_length, UNSIGNED_TY);
	parameter::add("max_offset", &_max_offset, UNSIGNED_TY);
	parameter::add("max_context_offset", &_max_context_offset, INT_TY);
	parameter::add("max_terminal_offset", &_max_terminal_offset, UNSIGNED_TY);
	parameter::add("max_terminal_context_offset", &_max_terminal_context_offset, INT_TY);
	parameter::add("max_child_offset", &_max_child_offset, UNSIGNED_TY);
	parameter::add("max_range", &_max_range, UNSIGNED_TY);
	parameter::add("max_context_range", &_max_context_range, UNSIGNED_TY);
	parameter::add("confidence_epsilon", &_confidence_epsilon, DOUBLE_TY);
	parameter::add("max_depth", &_max_depth, UNSIGNED_TY);
	parameter::add("seed", &_seed, UNSIGNED_TY);
	parameter::add("need_END_token", &_need_END_token, BOOL_TY);
	parameter::add("convert_PRT_to_ADVP", &_convert_PRT_to_ADVP, BOOL_TY);
	parameter::add("raise_punctuation", &_raise_punctuation, BOOL_TY);

	initialized = true;
	parameter::sanity_check();
}

/// Sanity check all variable values.
/// \todo Add more sanity checks.
void parameter::sanity_check() {
	assert(_min_sentences_per_feature >= 1);

	assert(!(_use_quant_features && _use_exists_features));

	/// \invariant use_redundant_feature_predicates => use_posclass
	assert(!_use_redundant_feature_predicates || _use_posclass);

	/// \invariant use_esoteric_feature_predicates => use_redundant_feature_predicates
	assert(!_use_esoteric_feature_predicates || _use_redundant_feature_predicates);

	/// \invariant use_clustered_feature_predicates => use_redundant_feature_predicates
	assert(!_use_clustered_feature_predicates || _use_redundant_feature_predicates);

	assert(_max_offset >= _max_child_offset);
	assert((int)_max_offset >= _max_context_offset);
	assert(_max_offset >= _max_terminal_offset);
	assert((int)_max_terminal_offset >= _max_terminal_context_offset);
	assert(_max_range >= _max_context_range);

	assert(_beam_search_width > 0);

	assert(_use_terminal_features || !_use_exists_terminal_features);

	assert(!(_parse_left_to_right && _parse_right_to_left));

	assert(_min_gain_factor_for_split >= 1);

	assert((_vary_l1_penalty && (_initial_l1_penalty_factor > _final_l1_penalty_factor)) ||
		(!_vary_l1_penalty && (_initial_l1_penalty_factor == _final_l1_penalty_factor)));

	assert(_l1_penalty_factor_overscale <= 1 && _l1_penalty_factor_overscale > 0);

	assert(_normalize_feature_counts == false);

	assert(!_cache_example_feature_vectors_before_decision_tree_building ||
			_downsample_examples_during_feature_selection);
}

/// \todo Throw exception, don't just exit.
void parameter::set_l1_penalty_factor(double d) {
	parameter::initialize();
	_l1_penalty_factor = d;
	
	assert(parameter::vary_l1_penalty());
	if (parameter::l1_penalty_factor() < parameter::final_l1_penalty_factor()) {
		ostringstream o;
		o << "l1_penalty_factor=" << _l1_penalty_factor << " < final_l1_penalty_factor="  <<  parameter::final_l1_penalty_factor() << "\n";
		throw o.str();
//		cerr << "ABORTING: " << o.str();
//		exit(0);
	}

	parameter::sanity_check();
}
