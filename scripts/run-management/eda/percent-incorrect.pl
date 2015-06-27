#!/usr/bin/perl
#
#   percent-incorrect.pl
#
#
#   Finds the percent of lines that contain:
#   correct=0
#   out of the number of lines that contain:
#   correct=[01]
#
#   $Id: percent-incorrect.pl 1657 2006-06-04 03:03:05Z turian $
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################
 
$tot = 0;
$cor = 0;
while(<>) {
	next if not /correct\s*=\s*[01]/;
	$tot++;
	next if /correct\s*=\s*1/;
	die unless /correct\s*=\s*0/;
	$cor++;
}
