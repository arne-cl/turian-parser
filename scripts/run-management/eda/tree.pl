#!/usr/bin/perl -w
#
#
#######################################################################
#
#
#   eda/tree.pl
#
#   Run tree on each of the lines in <> to generate textual trees.
#
#   USAGE: eda/tree.pl < trees.txt
#
#
#   $Id: tree.pl 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

$tree_exe = "/home/turian/parsing/tools/tree/tree";
die unless -x $tree_exe;

open(TREE, "| $tree_exe") or die $!;
while(<>) {
	s/\%/\\%/g;
	s/\$/\\\$/g;
	print TREE "\\tree $_" if /\S/;
}
close TREE;
