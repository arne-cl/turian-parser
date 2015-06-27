#!/usr/bin/perl -w
#
#######################################################################
#
#
#   postprocess-all.pl
#
#   USAGE: ./postprocess-all.pl dir1 [...]
#
#   Invoke $scriptdir/postprocess.py on each as-yet-unprocessed
#   parse-*.out file in the directories specified in the command-line
#   arguments.
#
#   $Id: postprocess-all.pl 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

use variables;

$chop = "/home/turian/common/scripts/chop-columns.pl";
die $! unless -x $chop;

die unless scalar @ARGV >= 1;

# FIXME: Move to variables.pm
$exe = "$scriptdir/treebank-processing/postprocess.py";
die unless -x $exe;

foreach $parsedir (@ARGV) {
	die unless -d $parsedir;
	$nom = get_directory_name($parsedir);
	$ppjmx = postprocess_jmx($nom);
	$len = int(`wc -l $ppjmx |  $chop 1`);
	foreach (split(/[\r\n]+/, `ls $parsedir/ | grep 'parse-.*.out'`)) {
		$fil = "$parsedir/$_";
		die unless $fil =~ m/parse-[0-9\.e\-\+]+.out/;

		die $! unless -r $fil;
		$l = int(`wc -l $fil |  $chop 1`);
		die "HUH???" if $l > $len;
		if ($l < $len) {
			print STDERR "\nSkipping $fil ($l lines, instead of $len)\n";
			next;
		}

		($outfil = $fil) =~ s/\.out$//;
		die $! unless $outfil ne $fil;
		if (-e $outfil) {
			die $! unless $len == int(`wc -l $outfil | $chop 1`);
			next;
		}		

		print STDERR "\nPostprocessing to $outfil\n";
		$cmd = "$exe $nom < $fil > $outfil 2> /dev/null || (rm $outfil && echo \"PROBLEM with $outfil\")";
#		print "$cmd\n";
		system($cmd);
	}
}
