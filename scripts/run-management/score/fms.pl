#!/usr/bin/perl
#
#   fms.pl
#
#   Compute the F-measure of PARSEVAL output.
#
#   $Id: fms.pl 1657 2006-06-04 03:03:05Z turian $
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

$r = 0;
$p = 0;
while(<>) {
	$r = $1 if (/Bracketing Recall\s*=\s*([0-9\.]+)/);
	$p = $1 if (/Bracketing Precision\s*=\s*([0-9\.]+)/);
}

print "LFMS,LR,LP\n";
print sprintf("%.2f,%.2f,%.2f\n", 2*$r*$p/($r+$p),$r,$p);
