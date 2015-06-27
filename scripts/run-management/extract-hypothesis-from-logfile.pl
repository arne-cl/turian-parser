#!/usr/bin/perl -w
#
#   Extract an incomplete weak hypothesis from the stderr logfile of a
#   crashed run.
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

die $! unless scalar @ARGV == 1;
open(F, "grep -A 2 Updated $ARGV[0] |") or die $!;
open(SORT, "| sort +1 -n") or die $!;
$lastf = 0;
while(<F>) {
	next if /^--$/;
	if (/Updated node: (Node [0-9]+ depth=[0-9]+)\s.*\s(split on.*)/) {
		print SORT $1;
		$lastf = $2;
	} elsif (/Positive child: Node ([0-9]+)/) {
		print SORT " +child=$1";
	} elsif (/Negative child: Node ([0-9]+)/) {
		die unless $lastf;
		print SORT " -child=$1 $lastf\n";
		$lastf = 0;
	} else {
		die $!;
	}
}
close(F);
close(SORT);
