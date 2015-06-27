#!/usr/bin/perl
#
#   state-error.pl
#
#   TESTME: I'M NOT SURE IF THIS PROGRAM WORKS!
#
#   Given an (unshuffled) set of preparsed examples, determine the
#   percent of states in which we would commit an error (i.e. the
#   example/label pair assigned highest confidence is incorrect).
#
#
#   $Id: state-error.pl 1657 2006-06-04 03:03:05Z turian $
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

$labels = 27;

$workdir = "/s1/export/home/turian/parsing/runsystem/workdata";
$cmd = "/s1/export/home/turian/parsing/runsystem/boosting/classifier2 $workdir/items $workdir/examples.original-nostate $workdir/weights.initial $workdir/mh-009";
$examples = "$workdir/examples.original-nolong";

open(EIN, "< $examples");
open(CIN, "$cmd |");

$laststate = -1;
$maxcorrect = 0;
$maxlabel = 0;
$maxconf = -999999.;
$maxexample = "";
while(<EIN>) {
	if (/state=([0-9]+) .* correct=([0-9]+) \| (.*) \|/) {
		$state = $1;
		$correct = $2;
		$example = $3;
		if ($laststate != $state and $laststate != -1) {
			print "correct=$maxcorrect conf=$maxconf label=$maxlabel state=$laststate example='$maxexample'\n";
			$maxcorrect = 0;
			$maxconf = -999999.;
		}
		$laststate = $state;

		$l = <CIN>;
		if ($l =~ m/([0-9]+)\s+\|\s*(.*\S)\s*\|/) {
			die unless $correct == $1;
			die unless $example == $2;
		} else {
			die $!;
		}
		for ($i = 1; $i <= $labels; $i++) {
			$l = <CIN>;
			if ($l =~ m/predict_label=([0-9]+) confidence=([\-0-9\.eE]+)/) {
				die unless $i eq $1;
				# FIXME: Tiebreak equality case
				if ($2 > $maxconf) {
					$maxlabel = $1;
					$maxconf = $2;
					$maxexample = $example;
					if ($1 == $correct) {
						$maxcorrect = 1;
					} else {
						$maxcorrect = 0;
					}
				}
			}
		}
	} else {
		die $!;
	}
}

close EIN;
close CIN;
