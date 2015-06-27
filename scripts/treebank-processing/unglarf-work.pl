#!/usr/bin/perl -w
#
#   WRITEME
#
#   TODO: WRITEME Do something sane with multi-word nodes,
#   e.g. |according to| or |Roe & Farnham|
#
#   $Id: unglarf-work.pl 1657 2006-06-04 03:03:05Z turian $
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

while (<>) {
	# Find everything enclosed in pipes,
	# and remove pipes.
	# WRITEME: Do something sane with multi-word nodes,
	# e.g. |according to| or |Roe & Farnham|
	while (m/\|([^\|]*)\|/) {
		$txt = $1;

		$txt =~ s/ /_/g;		# REMOVEME!

		s/\|[^\|]*\|/$txt/;
	}
	die if m/\|/;

	# Remove all but the first two items in the node.
	# Essentially, this strips one or more node number IDs.
	# FIXME: Add sanity checks to make sure we remove the correct
	# number of number IDs, and no more.
	while (/[^\(\) ]+ [^\(\) ]+ [^\(\) ]+\)/) {
		s/([^\(\) ]+ [^\(\) ]+) [^\(\) ]+\)/$1)/;
	}

	# REMOVEME!
	s/CONJ-/CONJ/g;

	# \. -> .
	s/\\\././g;

	# / -> \/
	s|\/|\\\/|g;

#	# Fix multi-words to have underscore
#	#if (m/(\|[\(\)]* [\(\)]*\|)/) {
#	#if (m/(\|[\(\)\|]* [\(\)\|]*)\|/) {
#	if (m/(\|[\(\)\|]* [\(\)\|]*)/) {
#		$orig = $1;
#		($new = $orig) =~ s/ /_/g;
#		print "\n\n$orig -> $new\n\n\n";
#	}
#
#	s/([\(\)])/ $1 /g;

	print;
}
