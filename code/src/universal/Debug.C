/*  $Id: Debug.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Debug.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "universal/Debug.H"
#include "universal/parameter.H"
#include "universal/stats.H"

debugstream Debug::nullstream;
debugstream Debug::logstream;
bool Debug::log_to_cerr = false;
map<string, unsigned> Debug::warnings_used;
vector<string> Debug::warnings_used_list;

/// We create this Debug instance so that, upon program termination,
/// Debug::~Debug() will be called and the cumulative warnings output.
/// \todo Make Debug a singleton pattern and this its only instance.
Debug dgb;

debugstream::~debugstream() {
	this->close();
}

/// Open a logfile (and cerr) for output.
/// \todo Make the floating-point precision a parameter.
void debugstream::open(const char *logfile, ios_base::openmode mode) {
	use_cerr = true;
	use_file = true;
	file.open(logfile, mode);
	assert(file.is_open());
	assert(file.good());
	assert(cerr.good());

	// Set the precision
	file.precision(6);
	cerr.precision(6);

	// Disable buffering.
	file.setf(std::ios_base::unitbuf);
	cerr.setf(std::ios_base::unitbuf);
}

/// Open cerr (only) for log output.
/// \todo Make the floating-point precision a parameter.
void debugstream::open_cerr() {
	assert(!use_file);

	use_cerr = true;
	assert(cerr.good());

	// Set the precision
	cerr.precision(6);

	// Disable buffering.
	cerr.setf(std::ios_base::unitbuf);
}

/// Check if we can write to the stream.
bool debugstream::is_open(void) {
	if (use_file) {
		if (!file.is_open()) return false;
		if (!file.good()) return false;
	}
	if (use_cerr) {
		if (!cerr.good()) return false;
	}
	return true;
}

/// Close an open logfile.
void debugstream::close() {
	if (use_file) {
		file.close();
	}
	if (use_cerr) {
		use_cerr = false;
	}
}

Debug::Debug() {
	/// TODO: Enable these assertions
	//assert(Debug::nullstream.is_open());
	//assert(!Debug::logstream.is_open());
}

/// Output all the warnings (in order), and then close the Debug stream.
/// \todo Sort the warnings by number of occurrences.
Debug::~Debug() {
	if (!warnings_used.empty()) {
		Debug::log(0) << "==================================\n";
		Debug::log(0) << "==   WARNINGS USED              ==\n";
		Debug::log(0) << "==================================\n";
		for(vector<string>::const_iterator s = warnings_used_list.begin();
				s != warnings_used_list.end(); s++) {
			Debug::log(0) << warnings_used[*s] << " " << *s << "\n";
		}
	} else {
		Debug::log(0) << "==================================\n";
		Debug::log(0) << "No warnings :)\n";
	}
	Debug::close();
}

/// Open a log file for appending.
/// \param logfile The logfile to open.
/// \todo Make the floating-point precision a parameter.
/// \todo Make sure that we don't call Debug::open() twice!
void Debug::open(const char* logfile) {
	Debug::logstream.open(logfile, ofstream::app);
	assert(Debug::logstream.is_open());

	// Set the logstream precision
	Debug::logstream.precision(6);

	// Disable logstream buffering.
	Debug::logstream.setf(std::ios_base::unitbuf);

	Debug::log(0) << "\n\n\n\n";
	Debug::log(0) << "########################################################################################\n";
	Debug::log(0) << "Current time is now " << stats::current_time() << "\n";
	Debug::log(0) << "Appending to logfile: " << logfile << "\n";
	Debug::log(0) << "\n\n";
}
void Debug::open(const string logfile) {
	Debug::open(logfile.c_str());
}

/// Open cerr for logging.
/// \todo Make the floating-point precision a parameter.
/// \todo Make sure that we don't call Debug::open() twice!
void Debug::open() {
	Debug::log_to_cerr= true;

	Debug::logstream.open_cerr();

	Debug::log(0) << "\n\n\n\n";
	Debug::log(0) << "Current time is now " << stats::current_time() << "\n";
	Debug::log(0) << "Logging to cerr" << "\n";
}

/// Retrieve a debugstream.
/// \param The debuglevel of the message to be logged.
/// \return Debug::logstream iff debuglevel <= parameter::debuglevel()
/// A stream to /dev/null otherwise
/// \todo Split logstream s.t. output also goes to cerr.
debugstream& Debug::log(unsigned debuglevel) {
	assert(Debug::logstream.is_open() || Debug::log_to_cerr);
	if (debuglevel <= parameter::debuglevel()) {
		return Debug::logstream;
	} else {
		return Debug::nullstream;
	}
}

/// Close the log stream
void Debug::close() {
	Debug::log(0) << "\n\n";
	Debug::log(0) << "Ending logging.\n";
	Debug::log(0) << "Current time is now " << stats::current_time() << "\n";
	Debug::log(0) << "########################################################################################\n";
	Debug::log(0) << "\n\n\n\n";

	if (Debug::log_to_cerr) {
		Debug::log_to_cerr = false;
	} else {
		Debug::logstream.close();
	}
}

/// Will we logged at some debuglevel?
/// \param debuglevel Some debuglevel.
/// \return True iff a message will be logged at this debuglevel.
bool Debug::will_log(unsigned debuglevel) {
	if (debuglevel <= parameter::debuglevel()) return true;
	else return false;
}

/// Output a warning message.
/// Typical usage:
///	Debug::warning(__FILE__, __LINE__, "foo");
/// \note We will output each warning only once.
void Debug::warning(const char* file, int line, string message) {
	ostringstream o;
	o << "WARNING (" << file << ":" << line << "): " << message << "\n";
	string s = o.str();
	if (Debug::warnings_used.find(s) != Debug::warnings_used.end()) {
		Debug::warnings_used[s]++;
		return;
	}
	warnings_used[s] = 0;
	warnings_used_list.push_back(s);
	assert(warnings_used.size() == warnings_used_list.size());
	Debug::log(0) << s;
}
