#!/usr/bin/perl -w
#
#
#######################################################################
#
#
#   create-run-directories.pl
#
#   USAGE: ./create-run-directories.pl
#   [in the 'runs/' directory]
#
#   Setup up directories for different runs.
#   NOTE: Edit the variable @dirs to specify which directories should
#   be created!
#
#   $Id: create-run-directories.pl 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

# EDIT THE BELOW
@dirs = ("r2l", "l2r", "bottom-up", "r2l-kitchen-sink");

die "Remember to final in directories to create!\n" unless scalar @dirs;

system("mkdir condor") if not -d "condor";

foreach $d (@dirs) {
	die if -e $d;
	system("mkdir $d");
	system("cp -Rd ../scripts/ $d");
	system("cd $d ; mkdir -p workdata/workdata-15words/logs");
	system("cd $d ; mkdir data");
	system("cd $d/workdata/ ; ln -s ../scripts/variables.pm");
	system("cd $d/workdata/workdata.full/ ; ln -s ../../scripts/variables.pm");
	system("cd $d/ ; ln -s ../../code/");
	system("cd $d/ ; ln -s ../condor/ condorbin");
	system("cd $d/ ; ./scripts/setup-paths.py");
	print STDERR "\n\n\n";
}

#system("mv ../scripts ../scripts.old");
#system("mv ../data ../data.old");
