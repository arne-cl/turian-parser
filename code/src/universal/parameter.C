/*  $Id: parameter.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file parameter.C
 *  \brief Implementation of parameter methods.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "universal/parameter.H"
#include "universal/Debug.H"

#include <sstream>


bool parameter::initialized = false;
bool parameter::write_locked = false;
hash_map<string, pair<void*, parameter_ty> > parameter::map = hash_map<string, pair<void*, parameter_ty> >();
vector<string> parameter::names = vector<string>();
po::options_description parameter::hidden("Hidden options");
po::variables_map parameter::vm;


/// Dump all globals to a string.
/// \todo Assert that the strings don't contain quotation marks.
/// \todo Set proper floating point precision.
string parameter::to_string(string s) {
	static parameter p;

	parameter::sanity_check();

	ostringstream o;
	vector<string>::iterator n;
	for (n = p.names.begin(); n != p.names.end(); n++) {
		assert(p.map.find(*n) != p.map.end());
		hash_map<string, pair<void*, parameter_ty> >::iterator i = p.map.find(*n);
		assert(*n == i->first);
		o << s << *n << " = ";
		switch (i->second.second) {
			case STRING_TY:
				/// \todo Assert that the strings don't contain quotation marks.
				o << "'" << *((string*)i->second.first) << "'";
				break;
			case BOOL_TY:
				if (*((bool*)i->second.first)) {
					o << "true";
				} else {
					o << "false";
				}
				break;
			case INT_TY:
				o << *((int*)i->second.first);
				break;
			case UNSIGNED_TY:
				o << *((unsigned*)i->second.first);
				break;
			case DOUBLE_TY:
				o << *((double*)i->second.first);
				break;
			case LOSS_TY:
				if (*((loss_ty*)i->second.first) == EXPONENTIAL_LOSS) {
					o << "EXPONENTIAL_LOSS";
				} else if (*((loss_ty*)i->second.first) == LOGISTIC_LOSS) {
					o << "LOGISTIC_LOSS";
				} else {
					cerr << "FATAL: Unknown loss " << *((loss_ty*)i->second.first) << "\n";
					assert(0);
				}
				break;
			default:
				assert(0);
		}
		if (!parameter::vm.count(*n)) o << " [default]";
		o << "\n";
	}
	return o.str();
}

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
const po::variables_map& parameter::read(
		const po::options_description& local,
		int argc, char *argv[]) {
	parameter::initialize();
	assert(!parameter::write_locked);

	// Add the parameters-file to the local parameter set.
	string parameters_file;
	po::options_description newlocal = local;
	newlocal.add_options()
		("parameters_file,p", po::value<string>(&parameters_file),
		 "file from which to read parameters [optional]")
		;

	// Declare a group of options that will be
	// allowed only on command line
	po::options_description generic("Generic options");
	generic.add_options()
		("version,v", "print version string")
		("help", "produce help message")
		;

	po::options_description cmdline_options;
	cmdline_options.add(generic).add(newlocal).add(hidden);

	po::options_description config_file_options;
	config_file_options.add(newlocal).add(hidden);

	po::options_description visible("Allowed options");
	visible.add(generic).add(newlocal);

	store(po::command_line_parser(argc, argv).options(cmdline_options).run(),
			parameter::vm);

	/// \todo Does it work to look at parameter::vm.count("parameters_file")
	/// before calling notify(parameter::vm)?
	if (parameter::vm.count("parameters_file")) {
		string parameters_file = parameter::vm["parameters_file"].as<string>();
		// We haven't opened the debug stream yet
		cerr << "Reading parameters file: " << parameters_file << "\n";

		ifstream ifs(parameters_file.c_str());
		store(parse_config_file(ifs, config_file_options), parameter::vm);
	}
	notify(parameter::vm);

	if (parameter::vm.count("help")) {
		cerr << visible << "\n";
		cerr.flush();
		exit(0);
	}

	if (parameter::vm.count("version")) {
		/// \todo WRITEME: Version-output
		assert(0);
	}

	parameter::sanity_check();
	parameter::write_locked = true;

	return parameter::vm;
}

/// Wrapper for reading parser parameters.
/// \note The first two references (parameters) passed to this
/// function are overwritten.
/// \param treebank_file File from which to read the treebank to parse
/// \param min_l1 Minimum l1 penalty for any classifier hypothesis. (default 0)
/// \param argc Command-line argument count.
/// \param argv Command-line argument vector.
void parameter::read_treebank_maybe_min_l1(string& treebank_file,
		double& min_l1, int argc, char *argv[]) {
	min_l1 = 0;

	Debug::open();

	po::options_description local("Parameters");
	local.add_options()
		("treebank_file,t", po::value<string>(&treebank_file),
		 "file from which to read the treebank to parse")
		("min_l1", po::value<double>(&min_l1),
		 "minimum l1 penalty for any classifier hypothesis [optional]")
		;

	const po::variables_map& vm = parameter::read(local, argc, argv);

	// Make sure all required parameters were set
	// NB This happens automatically if ->default_value() is used.
	assert(vm.count("treebank_file"));

	assert(vm["treebank_file"].as<string>() == treebank_file);

	parameter::set_treebank_has_parse_paths(false);

	Debug::log(1) << "\n";
	Debug::log(1) << "####################################\n";
	Debug::log(1) << "# Global parameters:\n";
	Debug::log(1) << parameter::to_string("\t");
	Debug::log(1) << "\n\n";

	if (vm.count("min_l1")) {
		assert(min_l1>= 0);
		Debug::log(1) << "Parsing with min l1 = " << min_l1 << "\n";
	} else {
		Debug::log(1) << "Parsing with indefinite number of iterations\n";
	}

	srand48(parameter::seed());

	// Read in the word list
	read_words(parameter::vocabulary_filename());

	// Read in the label list
	read_labels(parameter::label_filename());
}


/// Wrapper for reading training parameters.
/// \note The first two references (parameters) passed to this
/// function are overwritten.
/// \param label Label to train (given numerically)
/// \param treebank_file File from which to read the training treebank
/// \param argc Command-line argument count.
/// \param argv Command-line argument vector.
void parameter::read_label_and_treebank(Label& label,
		string& treebank_file, int argc, char *argv[]) {
	po::options_description local("Parameters");
	local.add_options()
		("label,l", po::value<Label>(&label),
		 "label to train (given numerically)")
		("treebank_file,t", po::value<string>(&treebank_file),
		 "file from which to read the training treebank")
		;

	const po::variables_map& vm = parameter::read(local, argc, argv);

	// Make sure all required parameters were set
	// NB This happens automatically if ->default_value() is used.
	assert(vm.count("label"));
	assert(vm.count("treebank_file"));

	assert(vm["label"].as<Label>() == label);
	assert(vm["treebank_file"].as<string>() == treebank_file);

	srand48(parameter::seed());

	// Read in the word list
	read_words(parameter::vocabulary_filename());

	// Read in the label list
	read_labels(parameter::label_filename());

	assert(is_constituent_label(label));

	Debug::open("log.label-" + label_string(label));
	Debug::log(1) << "\n";
	Debug::log(1) << "####################################\n";
	Debug::log(1) << "# Global parameters:\n";
	Debug::log(1) << parameter::to_string("\t");
	Debug::log(1) << "\n\n";
	Debug::log(1) << "\nTraining " << label_string(label) << " (#" << label << ") classifier.\n";
}



/// Overload the validate function for conversion of loss_ty.
/// \param v Storage for the value, either empty of an instance of loss_ty.
/// \param values The list of strings found in the next occurrence of the option.
/// \param target_type The two remaining parameters are needed "to
/// workaround the lack of partial template specialization and partial
/// function template ordering on some compilers"
/// \note Code based upon section 4.5 of the boost manual (pg. 76)
void validate(boost::any& v,
		const vector<string>& values, loss_ty* target_type, int) {
	// Make sure no previous assignment to 'v' was made.
	po::validators::check_first_occurrence(v);

	// Extract the first string from 'values'.
	// If there is more than one string, it's an error,
	// and an exception will be thrown.
	const string& s = po::validators::get_single_string(values);

	loss_ty l;
	if (s == "EXPONENTIAL_LOSS") {
		l = EXPONENTIAL_LOSS;
	} else if (s == "LOGISTIC_LOSS") {
		l = LOGISTIC_LOSS;
	} else {
		assert(0);
	}

	v = boost::any(l);
}

void parameter::add(string name, void* location, parameter_ty ty) {
	assert (!initialized);
	assert(map.find(name) == map.end());
	map.insert(make_pair(name, make_pair(location, ty)));
	names.push_back(name);

	switch (ty) {
		case STRING_TY:
			hidden.add_options()(name.c_str(), po::value<string>((string*)location)); break;
		case BOOL_TY:
			hidden.add_options()(name.c_str(), po::value<bool>((bool*)location)); break;
		case INT_TY:
			hidden.add_options()(name.c_str(), po::value<int>((int*)location)); break;
		case UNSIGNED_TY:
			hidden.add_options()(name.c_str(), po::value<unsigned>((unsigned*)location)); break;
		case DOUBLE_TY:
			hidden.add_options()(name.c_str(), po::value<double>((double*)location)); break;
		case LOSS_TY:
			hidden.add_options()(name.c_str(), po::value<loss_ty>((loss_ty*)location)); break;
		default:
			assert(0);
	}
}
