/*  $Id: parameter.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file parameter.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class parameter
 *  \brief Parameters global to the system.
 *
 *  We can also read in the parameters at runtime.
 *  See ./parameters.example for an example parameters file, which also contains
 *  a description of the parameters file format.
 *
 *  \warning All functions herein (except the constructor and helpers)
 *  should start with "parameter::initialize()" to call the constructor.
 *
 *  \note This class is based upon the singleton design pattern.
 *
 *  \todo Have helper/wrapper methods for common local parameters
 *  \todo Allow positional option for parameters-file
 *  \todo Rewrite using C++ type methods.
 *  \todo Am I using static correctly?
 *  \todo Make the floating-point precision a parameter.
 *  \todo When reading in parameters, enable the following now-defunct
 *  feature:
 *  'Note that any floating-point variable can be assigned value
 *  "log(parameter)" or "parameter", which is computed based upon the current value of
 *  "parameter". If you change the value of "parameter", make sure that
 *  all references to it occur *after* the re-assignment of "parameter".'
 *
 */

#ifndef __parameter_H__
#define  __parameter_H__

#include "universal/globals.H"
#include "universal/vocabulary.H"

#include <string>
#include <vector>
#include <ext/hash_map>

#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
#endif /* DOXYGEN */

#include <boost/program_options.hpp>
namespace po = boost::program_options;

typedef enum {STRING_TY, BOOL_TY, UNSIGNED_TY, INT_TY, DOUBLE_TY, LOSS_TY} parameter_ty;

#ifndef DOXYGEN
// The following code posted by Daniel Kegel on
// libstdc++@@gcc.gnu.org (Thu, 06 May 2004)
///// MOVE THIS TO ANOTHER FILE!!! <--------------------------
namespace __gnu_cxx {
	template<> class hash<string> {
		public: size_t operator()(const string& s) const {
			return hash<const char*>()(s.c_str());
		}
	};
}
#endif

class parameter {
public:
	/// Read in the parameters.
	/// Parameters are read in from the command-line and,
	/// potentially, from a parameters-file specified on the
	/// command-line.
	/// \param local Parameters local to this particular program.
	/// \param argc Command-line argument count.
	/// \param argv Command-line argument vector.
	/// \return The variables map into which we read the parameters.
	/// \todo Check that all (required) local parameters are set herein.
	/// \todo Make argv a const!
	/// \todo Does it work to look at vm.count("parameters_file")
	/// before calling notify(vm)?
	/// \todo WRITEME: Version-output
	static const po::variables_map& read(
			const po::options_description& local,
			int argc, char *argv[]);

	/// Wrapper for reading parser parameters.
	/// \note The first two references (parameters) passed to this
	/// function are overwritten.
	/// \param treebank_file File from which to read the treebank to parse
	/// \param min_l1 Minimum l1 penalty for any classifier hypothesis. (default 0)
	/// \param argc Command-line argument count.
	/// \param argv Command-line argument vector.
	static void read_treebank_maybe_min_l1(string& treebank_file,
			double& min_l1, int argc, char *argv[]);

	/// Wrapper for reading training parameters.
	/// \note The first two references (parameters) passed to this
	/// function are overwritten.
	/// \param label Label to train (given numerically)
	/// \param treebank_file File from which to read the training treebank
	/// \param argc Command-line argument count.
	/// \param argv Command-line argument vector.
	static void read_label_and_treebank(Label& label,
			string& treebank_file, int argc, char *argv[]);


	/// Dump all parameters to a string.
	/// \todo Assert that the strings don't contain quotation marks.
	/// \todo Set proper floating point precision.
	static string to_string(string s="");

private:
	/// Private initialization.
	/// Add all parameters to our parameter list.
	static void initialize();

	/// Sanity check all variable values.
	/// \todo Add more sanity checks.
	static void sanity_check();

	/// Add a parameter to the list of valid parameters.
	/// \param name The name of the new parameter.
	/// \param ptr A pointer to the storage of the parameter.
	/// \param ty The type of value the parameter takes.
	/// \todo Make this static?
	static void add(string name, void* ptr, parameter_ty ty);

	/// Once a single parameter value has been retrieved, we lock
	/// all the parameters from any further changes.
	static bool write_locked;

	/// False until init() is called.
	static bool initialized;

	static hash_map<string, pair<void*, parameter_ty> > map;

	/// Names of the parameters, in order of insertion.
	static vector<string> names;

	/// Hidden options, will be allowed both on command line and
	/// in config file, but will not be shown to the user in 'help'.
	static po::options_description hidden;

	/// The variables map into which we read parameters.
	static po::variables_map vm;

public:
/*
#define return_reference(name, ty)	\
	static const ty& name() {	\
	#ifdef SANITY_CHECKS		\
		parameter::initialize();	\
		write_locked = true;	\
	#endif				\
		return _name;		\
	}
	*/
	static const bool& cache_example_feature_vectors_before_training() { parameter::initialize(); return _cache_example_feature_vectors_before_training; }
	static const bool& cache_example_feature_vectors_before_decision_tree_building() { parameter::initialize(); return _cache_example_feature_vectors_before_decision_tree_building; }
	static const double& minconf_for_greedy_decision() { parameter::initialize(); return _minconf_for_greedy_decision; }
	static bool share_example_weight_per_state() { parameter::initialize(); return _share_example_weight_per_state; }
	static const string& vocabulary_filename() { parameter::initialize(); return _vocabulary_filename; }
	static const string& label_filename() { parameter::initialize(); return _label_filename; }
	static unsigned min_sentences_per_feature() { parameter::initialize(); return _min_sentences_per_feature; }
	static bool use_item_features() { parameter::initialize();  return _use_item_features; }
	static bool use_length_features() { parameter::initialize();  return _use_length_features; }
	static bool use_quant_features() { parameter::initialize();  return _use_quant_features; }
	static bool use_exists_features() { parameter::initialize();  return _use_exists_features; }
	static bool use_exists_terminal_features() { parameter::initialize();  return _use_exists_terminal_features; }
	static bool use_item_child_features() { parameter::initialize();  return _use_item_child_features; }
	static bool context_groups_for_exists_features() { parameter::initialize(); return _context_groups_for_exists_features; }
	static bool context_groups_for_exists_terminal_features() { parameter::initialize(); return _context_groups_for_exists_terminal_features; }
	static bool length_of_context_items_features() { parameter::initialize(); return _length_of_context_items_features; }
	static bool use_range_features() { parameter::initialize(); return _use_range_features; }
	static bool use_terminal_item_features() { parameter::initialize(); return _use_terminal_item_features; }
	static bool length_equal_to_feature() { parameter::initialize(); return _length_equal_to_feature; }
	static bool use_posclass() { parameter::initialize(); return _use_posclass; }
	static bool use_esoteric_feature_predicates() { parameter::initialize(); return _use_esoteric_feature_predicates; }
	static bool use_clustered_feature_predicates() { parameter::initialize(); return _use_clustered_feature_predicates; }
	static bool use_redundant_feature_predicates() { parameter::initialize(); return _use_redundant_feature_predicates; }
	static bool use_context_features() { parameter::initialize(); return _use_context_features; }
	static bool use_terminal_features() { parameter::initialize(); return _use_terminal_features; }
	static bool construct_redundant_features() { parameter::initialize(); return _construct_redundant_features; }
	static bool binary_predicates_can_have_false_value() { parameter::initialize(); return _binary_predicates_can_have_false_value; }
	static bool parse_left_to_right() { parameter::initialize(); return _parse_left_to_right; }
	static bool parse_right_to_left() { parameter::initialize(); return _parse_right_to_left; }
	static bool consider_all_examples_despite_r2l_or_l2r() { parameter::initialize(); return _consider_all_examples_despite_r2l_or_l2r; }
	static bool treebank_has_parse_paths() { parameter::initialize(); return _treebank_has_parse_paths; }
	static bool downsample_examples_during_feature_selection() { parameter::initialize(); return _downsample_examples_during_feature_selection; }
	static unsigned downsample_examples_during_feature_selection_sample_size() { parameter::initialize(); return _downsample_examples_during_feature_selection_sample_size; }
	static const loss_ty& loss_type() { parameter::initialize(); return _loss_type; }
	static const double& min_gain_factor_for_split() { parameter::initialize(); return _min_gain_factor_for_split; }
	static bool vary_l1_penalty() { parameter::initialize(); return _vary_l1_penalty; }
	static const double& initial_l1_penalty_factor() { parameter::initialize(); return _initial_l1_penalty_factor; }
	static const double& final_l1_penalty_factor() { parameter::initialize(); return _final_l1_penalty_factor; }
	static const double& l1_penalty_factor() { parameter::initialize(); return _l1_penalty_factor; }
	static const double& l1_penalty_factor_overscale() { parameter::initialize(); return _l1_penalty_factor_overscale; }
	static const bool& normalize_feature_counts() { parameter::initialize(); return _normalize_feature_counts; }
	static const unsigned& beam_search_width() { parameter::initialize(); return _beam_search_width; }
	static const unsigned& max_search_states_to_pop() { parameter::initialize(); return _max_search_states_to_pop; }
	static const double& confidence_better_than_no_action() { parameter::initialize(); return _confidence_better_than_no_action; }
	static const double& small_epsilon() { parameter::initialize(); return _small_epsilon; }
	static const unsigned& num_labels() { parameter::initialize(); return _num_labels; }
	static const unsigned& precision() { parameter::initialize(); return _precision; }
	static const unsigned& max_iterations() { parameter::initialize(); return _max_iterations; }
	static const unsigned& debuglevel() { parameter::initialize(); return _debuglevel; }
	static const unsigned& max_span_length() { parameter::initialize(); return _max_span_length; }
	static const unsigned& max_offset() { parameter::initialize(); return _max_offset; }
	static const int& max_context_offset() { parameter::initialize(); return _max_context_offset; }
	static const unsigned& max_terminal_offset() { parameter::initialize(); return _max_terminal_offset; }
	static const int& max_terminal_context_offset() { parameter::initialize(); return _max_terminal_context_offset; }
	static const unsigned& max_child_offset() { parameter::initialize(); return _max_child_offset; }
	static const unsigned& max_range() { parameter::initialize(); return _max_range; }
	static const unsigned& max_context_range() { parameter::initialize(); return _max_context_range; }
	static const double& confidence_epsilon() { parameter::initialize(); return _confidence_epsilon; }
	static const unsigned& max_depth() { parameter::initialize(); return _max_depth; }
	static const unsigned& seed() { parameter::initialize(); return _seed; }
	static bool need_END_token() { parameter::initialize(); return _need_END_token; }
	static bool convert_PRT_to_ADVP() { parameter::initialize(); return _convert_PRT_to_ADVP; }
	static bool raise_punctuation() { parameter::initialize(); return _raise_punctuation; }
	static bool trace_confidence_paths() { parameter::initialize(); return _trace_confidence_paths; }
	static bool accumulate_weight_in_leaves() { parameter::initialize(); return _accumulate_weight_in_leaves; }

	// Only a few variables are mutable after locking.
	static void set_treebank_has_parse_paths(bool b) { parameter::initialize(); _treebank_has_parse_paths = b; parameter::sanity_check(); }
	static void set_l1_penalty_factor(double d);
	static void set_trace_confidence_paths(bool b) { parameter::initialize(); _trace_confidence_paths = b ; parameter::sanity_check(); }
	static void set_accumulate_weight_in_leaves(bool b) { parameter::initialize(); _accumulate_weight_in_leaves = b ; parameter::sanity_check(); }

private:
	static bool _cache_example_feature_vectors_before_training;
	static bool _cache_example_feature_vectors_before_decision_tree_building;
	static double _minconf_for_greedy_decision;
	static bool _share_example_weight_per_state;
	static string _vocabulary_filename;
	static string _label_filename;
	static unsigned _min_sentences_per_feature;
	static bool _use_item_features;
	static bool _use_length_features;
	static bool _use_quant_features;
	static bool _use_exists_features;
	static bool _use_exists_terminal_features;
	static bool _use_item_child_features;
	static bool _context_groups_for_exists_features;
	static bool _context_groups_for_exists_terminal_features;
	static bool _length_of_context_items_features;
	static bool _use_range_features;
	static bool _use_terminal_item_features;
	static bool _length_equal_to_feature;
	static bool _use_posclass;
	static bool _use_esoteric_feature_predicates;
	static bool _use_clustered_feature_predicates;
	static bool _use_redundant_feature_predicates;
	static bool _use_context_features;
	static bool _use_terminal_features;
	static bool _construct_redundant_features;
	static bool _binary_predicates_can_have_false_value;
	static bool _parse_left_to_right;
	static bool _parse_right_to_left;
	static bool _consider_all_examples_despite_r2l_or_l2r;
	static bool _treebank_has_parse_paths;
	static bool _always_shuffle_examples;
	static bool _downsample_examples_during_feature_selection;
	static unsigned _downsample_examples_during_feature_selection_sample_size;
	static unsigned _beam_search_width;
	static unsigned _max_search_states_to_pop;
	static double _confidence_better_than_no_action;
	static loss_ty _loss_type;
	static double _min_gain_factor_for_split;
	static bool _vary_l1_penalty;
	static double _initial_l1_penalty_factor;
	static double _final_l1_penalty_factor;
	static double _l1_penalty_factor_overscale;
	static double _l1_penalty_factor;
	static bool _normalize_feature_counts;
	static double _small_epsilon;
	static unsigned _num_labels;
	static unsigned _precision;
	static unsigned _max_iterations;
	static unsigned _debuglevel;
	static unsigned _max_span_length;
	static unsigned _max_offset;
	static int _max_context_offset;
	static unsigned _max_terminal_offset;
	static int _max_terminal_context_offset;
	static unsigned _max_child_offset;
	static unsigned _max_range;
	static unsigned _max_context_range;
	static double _confidence_epsilon;
	static unsigned _max_depth;
	static unsigned _seed;
	static bool _need_END_token;
	static bool _convert_PRT_to_ADVP;
	static bool _raise_punctuation;
	static bool _trace_confidence_paths;
	static bool _accumulate_weight_in_leaves;
};

#endif /* __parameter_H__ */
