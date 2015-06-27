#!/usr/bin/perl -w
#
#######################################################################
#
#
#   remove-incomplete-parse-files.pl
#
#   USAGE: ./remove-incomplete-parse-output.pl [-f] dir1 [...]
#
#   Removes parse-*.[out|err|log] files if no postprocessed version
#   exists.
#   Only removes the files if -f is given.
#
#   $Id: remove-incomplete-parse-files.pl 1335 2005-09-20 01:08:26Z turian $
#
#
#######################################################################

die unless scalar @ARGV > 0;
if ($ARGV[0] eq "-f") {
	$remove = 1;
	die unless scalar @ARGV > 1;
	shift @ARGV;
}

foreach $parsedir (@ARGV) {
	die unless -d $parsedir;
	foreach (split(/[\r\n]+/, `ls $parsedir/ | grep 'parse-*'`)) {
		next if m/.*[0-9]$/;
		
		$fil = $_;
		#($postfil = $fil) =~ s/\.[^0-9]+$//;
		($postfil = $fil) =~ s/\.[^\.0-9]+$//;
#		print STDERR "Checking for $postfil\n";
		next if -e "$parsedir/$postfil";

		if ($remove) {
			print STDERR "REMOVING: $parsedir/$fil\n";
			system("rm $parsedir/$fil");
		} else {
			print STDERR "Not really removing: $parsedir/$fil\n";
		}
	}
}
