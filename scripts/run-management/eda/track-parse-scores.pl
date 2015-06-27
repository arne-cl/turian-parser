#!/usr/bin/perl -w
#
#
#######################################################################
#
#
#   eda/track-parse-scores.pl
#
#   USAGE: eda/track-parse-scores.pl < parse.err
#
#   Generate plots, for each sentence, tracking its PARSEVAL scores
#   during parsing.
#
#   We read a parser stderr log from <>, extract the scores,
#   and generate (in the current directory) sentence-scores.gp and
#   dat/sentence-*.dat
#
#   $Id: track-parse-scores.pl 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################


system("mkdir dat/");
die unless -d "dat/";

$gpfil = "sentence-scores.gp";
die if -e $gpfil;
open (GP, "> $gpfil");

print GP '
set xtics 0,1
set ytics 0,1
show xtics
show ytics
                                                                                                
set nomxtics
set nomytics
                                                                                                
set key top left
#set key outside

set xlabel "# decisions"
set ylabel "count"

set terminal postscript
#set size 0.85, 0.85
';

%counts = ();
while(<>) {
	if (/Sentence #([0-9]+), decision #([0-9]+):\s+(\S.*\S)\s+= ([0-9]+)/i) {
		$sentence = int($1);
		$step = int($2);
		$str = $3;
		$count = int($4);

		if ($str =~ m/Correct decisions thus far/i) {
			$ty = "PRC";
		} elsif ($str =~ m/Correct decisions upper bound/i) {
			$ty = "RCL";
		} elsif ($str =~ m/Crossing brackets thus far/i) {
			$ty = "CBs";
		}

		die unless $ty eq "PRC" or $ty eq "RCL" or $ty eq "CBs";
		$counts{$sentence}{$ty}{$step} = $count;
	}
}

foreach $sentence (sort {$a <=> $b } keys %counts) {
	$sstr = sprintf("sentence-%.3d", $sentence);

	$steps = -1;
	foreach $ty (keys %{$counts{$sentence}}) {
		$fil = "dat/$sstr.$ty.dat";
		die "$fil already exists!\n" if -e $fil;
		open(F, "> $fil");
		print STDERR "Writing to $fil...\n";
		foreach $step (sort {$a <=> $b } keys %{$counts{$sentence}{$ty}}) {
			print F "$step $counts{$sentence}{$ty}{$step}\n";
#			print "$ty $sentence $step => $counts{$sentence}{$ty}{$step}\n";
			$steps = $step if $steps < $step;
		}
		close(F);
	}

	print GP "\n";
	print GP "set output '$sstr.ps'\n";
	print GP "set title \"Sentence #$sentence, PARSEVAL scores (optimistic)\"\n";
	print GP "plot [0:$steps] [0:$steps] \\\n";
	print GP "\t'dat/$sstr.RCL.dat' title \"PRC bound\" with lp, \\\n";
	print GP "\t'dat/$sstr.PRC.dat' title \"PRC so far\" with lp, \\\n";
	print GP "\t'dat/$sstr.CBs.dat' title \"CBs so far\" with lp\n\n";
}
close(GP);
