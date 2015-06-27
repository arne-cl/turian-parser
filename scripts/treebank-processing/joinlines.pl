#!/usr/bin/perl -w
#
#   Read in Penn Treebank format, put one tree on each line of output, and
#   add spaces between tokens.
#
#   $Id: joinlines.pl 1657 2006-06-04 03:03:05Z turian $
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

sub printstr {
	(my $str) = @_;

	while ($str =~ m/\s\s/) {
		$str =~ s/\s\s+/ /g;
	}
	$str =~ s/^\s*\(\s*//;
	$str =~ s/\s*\)\s*$//;
	$str =~ s/^\s+//;
	$str =~ s/\s+$//;
	$str =~ s/\(\s+/\(/g;
	$str =~ s/\s+\)/\)/g;
	$str =~ s/^S1\s+//;
	print "(TOP $str)\n" if $str =~ m/\S/;
#	print "$str\n" if $str =~ m/\S/;
}

$str = "";
while (<>) {
	chop;
	s/([\(\)])/ $1 /g;
	if (m/^ \(/) {
		printstr($str);
		$str = $_;
	} else {
		$str .= " $_";
	}
}

printstr($str);
