#!/usr/bin/perl -w
#
#######################################################################
#
#
#   skip-long-sentences.pl
#
#   USAGE: ./skip-long-sentences.pl maxwords treefile.orig > treefile
#
#   Read in trees, and skip (output a blank line for) those with more
#   words than maxwords.
#   Determines sentence length using EVALB.
#
#   $Id: skip-long-sentences.pl 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

use variables;

die unless scalar @ARGV == 2;

$maxwords = int($ARGV[0]);
$fil = $ARGV[1];

%tokeep = ();

open(IN, "/home/turian/parsing/tools/eval/EVALB/evalb -p /home/turian/parsing/tools/eval/EVALB/COLLINS.prm $fil $fil |") or die $!;
$work = 0;
$i = 0;
while(<IN>) {
	if (/^====================/) {
		if (not $work) {
			$work = 1;
		} else {
			last;
		}
	} elsif ($work) {
		## Extract PARSEVAL "Words" column, i.e. number of terminal nodes *excluding* punctuation nodes
		#if (/^\s*([0-9]+)\s+[0-9]+\s+[0-9]+\s+[0-9\.]+\s+[0-9\.]+\s+[0-9]+\s+[0-9]+\s+[0-9]+\s+[0-9]+\s+([0-9]+)\s+/) {

		# Extract PARSEVAL "Length" column, i.e. number of terminal nodes *including* punctuation nodes
		if (/^\s*([0-9]+)\s+([0-9]+)\s+[0-9]+\s+[0-9\.]+\s+[0-9\.]+\s+[0-9]+\s+[0-9]+\s+[0-9]+\s+[0-9]+\s+[0-9]+\s+/) {
			$i++;
			die unless $1 == $i;
			$tokeep{$1} = 1 if $2 <= $maxwords;
		} else {
			die "$!: $_\n";
		}
	}
}
close IN;

open(FIL, "< $fil") or die $!;
$j = 0;
while(<FIL>) {
	$j++;
	print if $tokeep{$j};
}
close FIL;

die unless $i == $j;
