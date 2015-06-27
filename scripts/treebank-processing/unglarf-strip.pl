#!/usr/bin/perl -w
#
#   WRITEME
#
#   TODO: WRITEME Do something sane with multi-word nodes,
#   e.g. |according to| or |Roe & Farnham|
#
#   $Id: unglarf-strip.pl 1657 2006-06-04 03:03:05Z turian $
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

while (<>) {
	# Remove comments
	s/;;.*//;

	# Remove identification nodes
	s/\(TREE-NUM[^\)]*\)//g;
	s/\(FILE-NAME[^\)]*\)//g;

	print;
}
