#!/usr/bin/python
#
#######################################################################
#
#
#   track-scores-while-parsing.py
#
#   USAGE: ./track-scores-while-parsing.py < parse.err
#
#   Read in output from parsing, in which PRC, RCL, and CBs are
#   being tracked, and try to make graph output.
#   [I don't know exactly what this does]
#
#   $Id: track-scores-while-parsing.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

#skew_value = 0.05
#skew_value = 0.1
skew_value = 0.15

import re, sys, random



def mean(values):
	sum = 0
	n = len(values)
	for x in values:
		sum += x
	return 1.*sum/n

def variance(values):
	avg = mean(values)
	var = 0
	for x in values:
		var += (x - avg) * (x - avg)
	return var

def stddev(values, bias_corrected=False):
	var = variance(values)
	n = len(values)
	if bias_corrected: return (1.*var/(n-1))**.5
	else: return (1.*var/n)**.5

def median(vtmp):
	values = vtmp		# Copy unnecessary?
	values.sort()
	if len(values) % 2 == 1:
		median = values[len(values)/2]
	else:
		median = (values[len(values)/2-1] + values[len(values)/2])/2.
	return median

def skew(y):
	y += random.random() * skew_value - skew_value / 2;
	return y


prc = {}
rcl = {}
tst = {}
gld = {}

prc_re = re.compile("^Sentence #([0-9]+), decision #([0-9]+): Correct decisions thus far\s*= ([0-9]+)$")
rcl_re = re.compile("^Sentence #([0-9]+), decision #([0-9]+): Correct decisions upper bound\s*= ([0-9]+)$")
fin_re = re.compile("^FINAL sentence #([0-9]+): Correct brackets = [0-9]+, Test brackets = ([0-9]+), Gold brackets = ([0-9]+)$")
for l in sys.stdin:
	m = prc_re.match(l)
	if m:
		s = int(m.group(1))
		d = int(m.group(2))
		y = int(m.group(3))
		if s not in prc: prc[s] = {}
		assert d not in prc[s]
		prc[s][d] = y

	m = rcl_re.match(l)
	if m:
		s = int(m.group(1))
		d = int(m.group(2))
		y = int(m.group(3))
		if s not in rcl: rcl[s] = {}
		assert d not in rcl[s]
		rcl[s][d] = y

	m = fin_re.match(l)
	if m:
		s = int(m.group(1))
		t = int(m.group(2))
		g = int(m.group(3))
		assert s not in tst
		assert s not in gld
		tst[s] = t
		gld[s] = g
		assert(len(prc[s])-1 == t)
		assert(len(rcl[s])-1 == t)


for s in prc:
	for i in range(tst[s]+1):
		if rcl[s][i] == gld[s]: continue
		x = 1.*i/tst[s]
		#y = 1.*prc[s][i]/tst[s]
		if i == 0: continue
		y = 1.*prc[s][i]/i
		print skew(x), skew(y)
	print


#for s in rcl:
#	for i in range(tst[s]+1):
#		x = 1.*i/tst[s]
#		y = 1.*rcl[s][i]/gld[s]
#		print skew(x), skew(y)
#	print

## fms
#for s in prc:
#	for i in range(tst[s]+1):
#		x = 1.*i/tst[s]
#		if i == 0: continue
#		p = 1.*prc[s][i]/i
#		r = 1.*rcl[s][i]/gld[s]
#		print p, r
#		f = 2*p*r/(p+r)
#		print skew(x), skew(f)
#	print
