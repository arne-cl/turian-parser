#!/usr/bin/perl -w
#
#   Read in Penn Treebank I (Brown corpus) format, put one tree on each
#   line of output, and add spaces between tokens.
#
#   $Id: joinlines-brown.pl 1657 2006-06-04 03:03:05Z turian $
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

sub printstr {
	(my $str) = @_;

	# Convert (T foo) to foo
	# (handles a weird special case in cl/cl09.par)
	$str =~ s/\(\s*T\s+([^\(\)\s]+)\s*\)/ $1 /g;

#	# Remove empty elements
#	$str =~ s/\s\*\s/ /g;
#	$str =~ s/\sT\s/ /g;
#	$str =~ s/\s0\s/ /g;
#	$str =~ s/\sNIL\s/ /g;
	# Replace empty elements with -NONE-
	$str =~ s/\s\*\s/ -NONE- /g;
	$str =~ s/\sT\s/ -NONE- /g;
	$str =~ s/\s0\s/ -NONE- /g;
	$str =~ s/\sNIL\s/ -NONE- /g;

	# Remove trace numbers
	$str =~ s/([A-Za-z])-[0-9]+\s/$1 /g;

#	# Remove pseudo-attach constits
#	$str =~ s/\(\s*[A-Za-z]+\s+\*pseudo-attach\*\s*\)/ /g;
	# Convert pseudo-attach to -NONE-
	$str =~ s/\*pseudo-attach\*/-NONE-/g;

	# Bracket all words with POS tag NNP
	$str =~ s/([^\s\(\)]+)\s+([^\s\(\)]+)\s+/$1 (NNP $2) /g;
	$str =~ s/\)\s+([^\s\(\)]+)\s+/) (NNP $1) /g;

	# Make sure we tagged initial words
	$str =~ s/^\s*\(\s*([A-Za-z][a-z]+)\s/( (NNP $1) /;

	# Make sure we tagged all initial punctuation
	$str =~ s/\(\s*([`'\*\-\.\,\%\$\!\?\:\;\#][^\s\(\)]*)\s/( ( NNP $1 ) /g;

	# Convert bracket abbreviations to Penn Treebank II format
	$str =~ s/\*([LR][A-Z]B)\*/-$1-/g;

	# Replace -NONE- tag with -NONE-
	$str =~ s/\(\s*NNP\s+-NONE-\s*\)/( -NONE- -NONE- )/g;

	# Remove all outermost parenthesis
	while ($str =~ m/^\s*\((.*)\)\s*$/) {
		$str =~ s/^\s*\((.*)\)\s*$/$1/;
	}

	while ($str =~ m/\s\s/) {
		$str =~ s/\s\s+/ /g;
	}
#	$str =~ s/^\s*\(\s*//;
#	$str =~ s/\s*\)\s*$//;
	$str =~ s/^\s+//;
	$str =~ s/\s+$//;
	$str =~ s/\(\s+/\(/g;
	$str =~ s/\s+\)/\)/g;
	print "(TOP ($str))\n" if $str =~ m/\S/;
#	print "($str)\n" if $str =~ m/\S/;
}

$str = "";
while (<>) {
	s/\*x\*.*//;	# Remove comment lines, *x**x**x*
	s/(\s*END_OF_TEXT_UNIT\s*)//g;

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
