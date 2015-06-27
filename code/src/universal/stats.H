/*  $Id: stats.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file stats.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class stats
 *  \brief Statistics maintained by the system.
 *
 *  \warning All functions herein (except the constructor and helpers)
 *  should start with "stats::stats()" to call the constructor.
 *
 *  \note This class is based upon the singleton design pattern. [meaning: WRITEME]
 *
 *  \todo Add a initialized boolean variable for *each* statistic.
 *  \todo Add #sentence, #features
 *
 */

#ifndef __stats_H__
#define  __stats_H__

#include "universal/globals.H"
#include "universal/Double.H"

#include <sys/time.h>

class stats {
public:
	stats();
	static unsigned correct_actions();
	static unsigned total_actions();
	static unsigned steps();

	static void correct_actions_add(unsigned i);
	static void total_actions_add(unsigned i);
	static void steps_add(unsigned i=1);
	static void set_steps(unsigned i);

	static unsigned sentence_count();
	static void sentence_count_add(unsigned i=1);

	static void start_timing();
	/// \todo Use this function in more places.
	static const string current_time();
	static const string resource_usage();

	/// Get the user+sys time.
	static double usersys_time();

private:
	static unsigned m_correct_actions;
	static unsigned m_total_actions;
	static unsigned m_steps;

	static unsigned m_sentence_count;

	static time_t m_start_time;
};

#endif /* __stats_H__ */
