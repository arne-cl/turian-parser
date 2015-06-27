#!/usr/bin/perl -w
#
#
#######################################################################
#
#
#   eda/run-tree-on-all-sentences.pl
#
#   USAGE: eda/run-tree-on-all-sentences.pl name treebank.txt
#
#   For each sentence in treebank.txt, run tree.pl and generate .txt drawing
#   of the tree.
#   We write sentence #N to sentence-N.name.txt
#
#   $Id: run-tree-on-all-sentences.pl 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

use variables;
$tree = "$scriptdir/eda/tree.pl";
die "'$tree' does not exist or is not executable!" unless -x $tree;

die "Incorrect call.\nUSAGE: eda/run-tree-on-all-sentences.pl name treebank.txt\n" unless scalar @ARGV == 2;

$nom = $ARGV[0];
$treefile = $ARGV[1];
open(TREEBANK, "<$treefile") or die "$!";

$cnt = 0;
while(<TREEBANK>) {
	$cnt++;

	$fil = sprintf("sentence-%.3d.%s.txt", $cnt, $nom);
	die "'$fil' already exists!" if -e $fil;
	print STDERR "Writing '$fil'...\n";
	open(TREE, "| $tree > $fil");
	print TREE;
	close TREE;
}
close TREEBANK;
