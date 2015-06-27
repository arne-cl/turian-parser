/*  $Id: Debug.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Debug.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*
 *  \class Debug
 *  \brief A simple debug/logging library.
 *
 *  Debug::log(n) is an ostream at debuglevel n.
 *  The higher the debuglevel, the less crucial is the debug output.
 *
 *  \todo Make this a singleton pattern, such that we can only have one
 *  instance of Debug.
 *  \todo Short-circuit any debug output that is at too high a debuglevel? i.e.\ such
 *  that no time is wasted string-converting the objects that would have been output.
 *  \todo Log to several logs simultaneously, one for each error-level?
 *  \todo Add Debug::increase_depth(unsigned debuglevel) and
 *  Debug::decrease_depth(unsigned debuglevel) to allow us to globally
 *  control indentation depth.
 *
 */

#ifndef __Debug_H__
#define  __Debug_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

/// Prefer Debug::warning() over TRACE.
#define TRACE \
	{ Debug::log(1) << "TRACE " << __FILE__ << ":" << __LINE__ << "\n"; }


/// A debug stream.
/// Either a NULL stream, a cerr stream, or a tee stream to cerr and a logfile (appended).
/// \warning Make sure that anything with an overloaded 'friend ostream operator<<'
/// also has an overloaded 'friend debugstream operator<<'.
/// \todo FIXME: This is dangerous? It silently fails (doesn't output
/// anything) if the appropriately typed operator<< is not overloaded!
/// \todo Inherit from ostream?
/// \todo Should the overloaded operator<<'s not be virtual?
//class debugstream : public ostream {
class debugstream : public ofstream {
public:
	/// Default (NULL) constructor.
	//debugstream(streambuf buf=streambuf()) : file(), use_file(false), use_cerr(false) { }
//	debugstream() : file(streambuf()), use_file(false), use_cerr(false) { }
	debugstream() : use_file(false), use_cerr(false) { }

	~debugstream();

	/// Open a logfile (and cerr) for output.
	/// \todo Make the floating-point precision a parameter.
	void open(const char *logfile, ios_base::openmode mode=ios::app);

	/// Open cerr (only) for log output.
	/// \todo Make the floating-point precision a parameter.
	void open_cerr();

	/// Check if we can write to the stream.
	bool is_open(void);

	/// Close an open logfile.
	void close();

	virtual debugstream& operator<<(char c) { if (use_file) file << c; if (use_cerr) cerr << c; return *this; }
	virtual debugstream& operator<<(unsigned char c) { return (*this) << (char)c; }
	virtual debugstream& operator<<(signed char c) { return (*this) << (char)c; }
	virtual debugstream& operator<<(const char *s) { if (use_file) file << s; if (use_cerr) cerr << s; return *this; }
	virtual debugstream& operator<<(const unsigned char *s) { return (*this) << (const char*)s; }
	virtual debugstream& operator<<(const string& s) { if (use_file) file << s; if (use_cerr) cerr << s; return *this; }
	virtual debugstream& operator<<(const signed char *s) { return (*this) << (const char*)s; }
	virtual debugstream& operator<<(const void *p) { if (use_file) file << p; if (use_cerr) cerr << p; return *this; }
	virtual debugstream& operator<<(int n) { if (use_file) file << n; if (use_cerr) cerr << n; return *this; }
	virtual debugstream& operator<<(unsigned int n) { if (use_file) file << n; if (use_cerr) cerr << n; return *this; }
	virtual debugstream& operator<<(long n) { if (use_file) file << n; if (use_cerr) cerr << n; return *this; }
	virtual debugstream& operator<<(unsigned long n) { if (use_file) file << n; if (use_cerr) cerr << n; return *this; }
#if defined(__GNUC__)
	__extension__ virtual debugstream& operator<<(long long n) { if (use_file) file << n; if (use_cerr) cerr << n; return *this; }
	__extension__ virtual debugstream& operator<<(unsigned long long n) { if (use_file) file << n; if (use_cerr) cerr << n; return *this; }
#endif
	virtual debugstream& operator<<(short n) {return operator<<((int)n);}
	virtual debugstream& operator<<(unsigned short n) {return operator<<((unsigned int)n);}
#if _G_HAVE_BOOL
	virtual debugstream& operator<<(bool b) { return operator<<((int)b); }
#endif
	virtual debugstream& operator<<(double n) { if (use_file) file << n; if (use_cerr) cerr << n; return *this; }
	virtual debugstream& operator<<(float n) { return operator<<((double)n); }
#if _G_HAVE_LONG_DOUBLE_IO
	virtual debugstream& operator<<(long double n) { if (use_file) file << n; if (use_cerr) cerr << n; return *this; }
#else
	virtual debugstream& operator<<(long double n) { return operator<<((double)n); }
#endif

private:
	ofstream file;
	bool use_file;
	bool use_cerr;
};


class Debug {
public:
	/// Protected for singleton method
	Debug();

	/// Output all the warnings (in order), and then close the Debug stream.
	/// \todo Sort the warnings by number of occurrences.
	~Debug();

	/// Open a log file for appending.
	/// \param logfile The logfile to open.
	/// \todo Make the floating-point precision a parameter.
	/// \todo Make sure that we don't call Debug::open() twice!
	static void open(const string logfile);
	static void open(const char* logfile);

	/// Open cerr for logging.
	/// \todo Make the floating-point precision a parameter.
	/// \todo Make sure that we don't call Debug::open() twice!
	static void open();

	/// Close the log stream
	static void close();

	/// Retrieve a debugstream.
	/// \param The debuglevel of the message to be logged.
	/// \return Debug::logstream iff debuglevel <= parameter::debuglevel()
	/// A stream to /dev/null otherwise
	/// \todo Split logstream s.t. output also goes to cerr.
	static debugstream& log(unsigned debuglevel);

	/// Will we logged at some debuglevel?
	/// \param debuglevel Some debuglevel.
	/// \return True iff a message will be logged at this debuglevel.
	static bool will_log(unsigned debuglevel);

	/// Output a warning message.
	/// Typical usage:
	///	Debug::warning(__FILE__, __LINE__, "foo");
	/// \note We will output each warning only once.
	static void warning(const char* file, int line, string message="");

private:
	static debugstream nullstream;
	static debugstream logstream;
	static bool log_to_cerr;

	static map<string, unsigned> warnings_used;
	static vector<string> warnings_used_list;
};

#endif /* __Debug_H__ */
