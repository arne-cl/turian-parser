#!/usr/bin/perl -w
#
#######################################################################
#
#
#   create-gnuplot-files.pl
#
#   USAGE: ./create-gnuplot-files.pl dir1 [...]
#
#   Automagically Gnuplot splits vs. LFMS dat files,
#   and put them in ~/parsing/graphs/datnew/
#
#   Detailed procedure for each $parsedir in @ARGV:
#   1. Postprocess parse output
#   2. Compute PARSEVAL scores
#   3. Splitting off splits + LFMS
#   4. Copy '$parsedir/scores-splits-fms.dat' to a more descriptively
#   named file: '$parsedir/$base.$run.$nom.splits-fms.dat'
#   5. Move that file to ~/parsing/graphs/datnew/
#
#   TODO:
#   * Use variables.pm to ascertain + doublecheck the correct directories?
#
#   $Id: create-gnuplot-files.pl 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

use variables;

$wd = `pwd`;

die if not $wd =~ m|/home/turian/parsing/([^/]+)/runs/([^/\n]+)|;
$base = $1;
$run = $2;

print STDERR "\n";
print STDERR "BASE:\t$base\n";
print STDERR "RUN:\t$run\n\n";

die $! unless scalar @ARGV > 0;

foreach $parsedir (@ARGV) {
	die unless -d $parsedir;
	$nom = get_directory_name($parsedir);

	print STDERR "\n\nNOM:\t$nom\n";

	$splits_out = "$parsedir/$base.$run.$nom.splits-lfms.dat";

	die if -e $splits_out;

	die unless -x "$scriptdir/run-management/postprocess-all.pl";
	$cmd = "$scriptdir/run-management/postprocess-all.pl $parsedir";
	print STDERR "Postprocessing...\n";
	system("time $cmd 2> /dev/null")==0 or die "$!: Command '$cmd' failed!\n";

	die unless -x "$scriptdir/run-management/score/allfms.pl";
	$cmd = "$scriptdir/run-management/score/allfms.pl $parsedir > $parsedir/scores.dat";
	print STDERR "Computing PARSEVAL scores...\n";
	system("time $cmd 2> /dev/null")==0 or die "$!: Command '$cmd' failed!\n";

	die unless -x "$scriptdir/run-management/score/split-fms.pl";
	$cmd = "$scriptdir/run-management/score/split-fms.pl $parsedir/scores.dat > $parsedir/scores-splits-fms.dat";
	print STDERR "Splitting off splits + LFMS...\n";
	system("time $cmd 2> /dev/null")==0 or die "$!: Command '$cmd' failed!\n";

	$cmd = "cp $parsedir/scores-splits-fms.dat $splits_out";
	print STDERR "Copying '$parsedir/scores-splits-fms.dat' to '$splits_out'...\n";
	system("time $cmd 2> /dev/null")==0 or die "$!: Command '$cmd' failed!\n";

	$cmd = "mv $splits_out ~/parsing/graphs/datnew/";
	print STDERR "Moving '$splits_out' to ~/parsing/graphs/datnew/...\n";
	system("time $cmd 2> /dev/null")==0 or die "$!: Command '$cmd' failed!\n";
}
print STDERR "\nDone. Enjoy your gnuplotting! ;)\n"
