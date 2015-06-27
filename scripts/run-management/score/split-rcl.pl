#!/usr/bin/perl -wn
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

open(SORT, "| grep -v L[A-Z][A-Z][A-Z] | sort -g") or die $!;
while(<>) {
	if (m/^(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s*$/) {
		print SORT "$2 $4\n";
	} else {
		die "$!: $_";
	}
}
close SORT;
