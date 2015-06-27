#!/usr/bin/perl -w
#
#
########################################################################
#
#
#   numsplits-last-iterations.pl
#
#   USAGE: ./numsplits-last-iterations.pl [min_dloss]
#
#   Find the number of splits total in the last hypothesis for each
#   label classifier.
#   If the min_dloss parameter is given, then for each label we choose
#   that hypothesis with lowest dloss that is nonetheless at least as
#   large as min_dloss.
#
#   $Id: numsplits-last-iterations.pl 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

use variables;

if (scalar @ARGV == 0) {
	$min_dloss = 0;
} else {
	if ($ARGV[0] =~ m/^[0-9e\.\-\+]+$/) {
		$min_dloss = $ARGV[0];
	} else {
		die "Incorrect call.\nUSAGE: ./numsplits-last-iterations.pl [min_dloss]\n";
	}
}

$last_hypotheses = `$script_last_iterations $min_dloss`;
$count = int(`cat $last_hypotheses | grep split | wc -l`);
die unless $count;
print "$count\n";
