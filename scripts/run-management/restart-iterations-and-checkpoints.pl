#!/usr/bin/perl -w
#
#
########################################################################
#
#
#   restart-iterations-and-checkpoints.pl
#
#   USAGE: ./restart-iterations-and-checkpoints.pl newdirectory
#
#   For all hypotheses in the current directory,
#   copy them to a new directory and restart them as if the dloss
#   were 1e+06.
#
#   $Id: restart-iterations-and-checkpoints.pl 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

die unless scalar @ARGV == 1;
$newdir = $ARGV[0];
print STDERR "NB: $newdir already exists...\n" if -e $newdir;

%hyps = ();
foreach $l (split(/[\r\n]+/, `ls hypothesis.label-*`)) {
	if ($l =~ m/hypothesis.label-(.*).min_dloss-.*/) {
		$lbl = $1;
		die "More than one hypothesis for $lbl!\n" if $hyps{$lbl};
		$hyps{$lbl} = $l;
	} else {
		die $!;
	}
}

foreach $lbl (keys %hyps) {
	$in = $hyps{$lbl};
	$out = "$newdir/hypothesis.label-$lbl.min_dloss-1e+06";
	$chk = "$newdir/checkpoint.label-$lbl";
	die $! if not -e $in;
	die $! if -e $out;
	die $! if -e $chk;
}

foreach $lbl (keys %hyps) {
	$in = $hyps{$lbl};
	$out = "$newdir/hypothesis.label-$lbl.min_dloss-1e+06";
	$chk = "$newdir/checkpoint.label-$lbl";
	system("cp $in $out");
	system("echo 'hypothesis.label-$lbl.min_dloss-1e+06 1e+06' > $chk");
	die $! if not -e $in;
	die $! if not -e $out;
	die $! if not -e $chk;
}

print STDERR "Done restarting hypotheses to directory $newdir\n";
