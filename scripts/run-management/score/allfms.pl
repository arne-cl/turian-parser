#!/usr/bin/perl -w
#
#######################################################################
#
#
#   allfml.pl
#
#   USAGE: ./allfms.pl dir
#
#   For each file parse-[0-9\.e\-\+]* in dir, determine
#   the PARSEVAL FMS of this parsefile.
# 
#   $Id: allfms.pl 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

use variables;

die unless scalar @ARGV == 1;
$parsedir = $ARGV[0];

die unless -d $parsedir;
$nom = get_directory_name($parsedir);
$goldfile = postprocess_gold($nom);

print STDERR "Evaluating $parsedir using gold file $goldfile\n";

#open(SORT, "| sort -rg");
open(SORT, "| sort +1 -n");

foreach (split(/[\r\n]+/, `ls $parsedir/ | grep 'parse-.*[0-9]\$'`)) {
	$fil = "$parsedir/$_";

	if ($fil =~ m/parse-([0-9\.e\-\+]+)$/) {
		$step = $1;
	} else { die "$!: $fil"; }

	$fmsfil = "$parsedir/$_.fms";
	if (-e $fmsfil) {
		# ...we've already computed this dloss's scores
		$str = `cat $fmsfil`;
		die $! unless $str =~ m/([0-9\.e\-\+]+)\s+([0-9+)\s+([0-9\.e\-\+]+)\s+([0-9\.e\-\+]+)\s+([0-9\.e\-\+]+)/ and $1 == $step;
		print SORT $str;
		next;
	}

	$splits = int(`$script_numsplits_last_iterations $step`);
	$out = `~/parsing/tools/eval/EVALB20050908/evalb -p ~/parsing/tools/eval/EVALB20050908/COLLINS.prm $goldfile $fil | $script_fms | tail -1`;
	#$out =~ s/,.*//;
	$out =~ s/,/ /g;
	$out =~ s/[\r\n]\s*$//s;

	#$str = "$step $out";
	#$str = "$splits $out";
	$str = "$step $splits $out";

	if (not $str =~ m/([0-9\.e\-\+]+)\s+([0-9+)\s+([0-9\.e\-\+]+)\s+([0-9\.e\-\+]+)\s+([0-9\.e\-\+]+)/) {
		print STDERR "Problem with $fil: $str!\nSkipping...\n";
		next;
	}

	print SORT "$str\n";
	system("echo $str > $fmsfil");
}
close(SORT);
