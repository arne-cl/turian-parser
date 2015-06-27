/*  $Id: stats.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file stats.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "universal/stats.H"
#include "universal/globals.H"

#include <ctime>

#include <sys/resource.h>
#include <unistd.h>

const int EST = -5; 			// Eastern Standard Time

unsigned stats::m_correct_actions = 0;
unsigned stats::m_total_actions = 0;
unsigned stats::m_steps = 0;

unsigned stats::m_sentence_count = 0;

time_t stats::m_start_time = 0;

stats::stats() {
	time(&m_start_time);
	assert(m_start_time);
}

unsigned stats::correct_actions() {
	return stats::m_correct_actions;
}

unsigned stats::total_actions() {
	return stats::m_total_actions;
}

void stats::correct_actions_add(unsigned i) {
	stats::m_correct_actions += i;
}

void stats::total_actions_add(unsigned i) {
	stats::m_total_actions += i;
}

const string stats::current_time() {
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	//timeinfo = localtime(&rawtime);
	rawtime += EST*3600;		// Eastern Standard Time
	timeinfo = gmtime(&rawtime);
	string s(asctime(timeinfo));

	// Strip trailing whitespace.
	while (s[s.length()-1] == '\n' || s[s.length()-1] == '\r') {
		s = s.substr(0, s.length()-1);
		assert(s.length() > 1);
	}
	return s;
}

const string stats::resource_usage() {
	rusage r;
	unsigned ret;
	ret = getrusage(RUSAGE_SELF, &r);
	assert(ret==0);
	ostringstream o;
	o << "Resource usage at " << stats::current_time() << ": ";

	o.precision(4);
	float ty = (r.ru_utime.tv_sec + r.ru_stime.tv_sec) + 1./1000000 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec);
	if (ty < 60) {
		o << ty << "s";
	} else {
		ty /= 60;
		if (ty < 60) {
			o << ty << "m";
		} else {
			ty /= 60;
			if (ty < 24) {
				o << ty << "h";
			} else {
				ty /= 24;
				o << ty << "d";
			}
		}
	}
	o << " user+sys";

	time_t rawtime;
	time(&rawtime);
	assert(m_start_time);
	time_t elapsed = rawtime - m_start_time;
	float user_sys_ty = (r.ru_utime.tv_sec + r.ru_stime.tv_sec) + 1./1000000 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec);
	o << " (" << 100.*user_sys_ty/elapsed << "% usage of " << elapsed << "s elapsed)";

//	o << "\tmaximum resident set size = " << r.ru_maxrss * getpagesize() / 1024. / 1024 << " MB\n";
//	o << "\tintegral shared memory size = " << r.ru_ixrss * getpagesize() / 1024. / 1024 << " MB\n";
//	o << "\tintegral unshared data size = " << r.ru_idrss * getpagesize() / 1024. / 1024 << " MB\n";
//	o << "\tintegral unshared stack size = " << r.ru_isrss * getpagesize() / 1024. / 1024 << " MB\n";
//	o << "\tpage reclaims = " << r.ru_minflt << "\n";
//	o << "\tpage faults = " << r.ru_majflt << "\n";

	ostringstream pidf;
	pidf << "/proc/" << getpid() << "/status";
	ifstream i(pidf.str().c_str());
	assert(i.good());
	string s;
	do {
		i >> s;
	} while(s != "VmSize:");
	unsigned vmsize;
	i >> vmsize;
	o.precision(4);
	o << ", " << vmsize/1024. << " MB vmsize ";

	if (r.ru_nswap) o << ", " << r.ru_nswap << " SWAPS\n";

	return o.str();
}

/// Get the user+sys time.
double stats::usersys_time() {
	rusage r;
	unsigned ret;
	ret = getrusage(RUSAGE_SELF, &r);
	assert(ret==0);
	return (r.ru_utime.tv_sec + r.ru_stime.tv_sec) + 1./1000000 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec);
}


unsigned stats::sentence_count() {
	return stats::m_sentence_count;
}

void stats::sentence_count_add(unsigned i) {
	stats::m_sentence_count += i;
}

unsigned stats::steps() {
	return stats::m_steps;
}

void stats::steps_add(unsigned i) {
	stats::m_steps += i;
}

void stats::set_steps(unsigned i) {
	m_steps = i;
}
