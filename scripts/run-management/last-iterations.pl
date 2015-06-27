#!/usr/bin/perl -w
#
#
########################################################################
#
#
#   last-iterations.pl
#
#   USAGE: ./last-iterations.pl [min_dloss] [*]
#
#   Find the lowest min dloss for each hypothesis.
#   If the min_dloss parameter is given, then all hypotheses must have
#   min_dloss larger than this parameter.
#
#   If there is no parameter, the hypotheses will be output on
#   a single line.
#   If there is a parameter, the hypotheses will be output
#   one per line, sorted by min_dloss.
#
#   $Id: last-iterations.pl 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

if ($ARGV[0] =~ m/^[0-9e\.\-\+]+$/) {
	$min_dloss = $ARGV[0];
	$long = 1 if $ARGV[1];
	die if $ARGV[2];
} else {
	$min_dloss = 0;
	$long = 1 if $ARGV[0];
	die if $ARGV[1];
}

%dloss = ();
%min_file = ();
foreach $l (split(/[\r\n]+/, `ls checkpoint.label-*`)) {
	if ($l =~ m/checkpoint.label-(.*)/) {
		$lbl = $1;
		$dloss{$lbl} = 1e30;
		open(FILE, "< checkpoint.label-$lbl") or die $!;
		while(<FILE>) {
			if (/(hypothesis\S*) (\S+)/) {
#			if (/(hypothesis\S*) dloss (\S+)/) {
				print STDERR "WARNING: dloss $2 is very near mindloss param $min_dloss\n" if ($min_dloss > $2 and $min_dloss*0.9999 <= $2);
				if ($2 < $dloss{$lbl} and $min_dloss <= $2) {
					$dloss{$lbl} = $2;
					$min_file{$lbl} = $1;
				}
			} else {
				die $!;
			}
		}
	} else {
		die $!;
	}
}

open(SORT, '| sort +2 -g -t \-') if $long;
foreach $file (values %min_file) {
	print " $file" if not $long;
	print SORT "$file\n" if $long;
}
close SORT if $long;
