#!/usr/bin/python
#
#!/usr/local/bin/python2.3
#
#   Generate condor submit file for parsing directories given in ARGV.
#
#   make-condor-parser-submit.py parameters_file directory name1 [name2 ...]
#   	name? = "15-words.devel", for example
#
#   $Id: make-condor-parsersearch-submit.py 1505 2006-02-14 01:24:55Z turian $
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

import sys, os, popen2, random

#jobs_per_file = 10
jobs_per_file = 100
max_l1 = 10	# Don't do any parsing with an l1 above this value

from variables import *
from host import *
host = get_host()

if len(sys.argv) < 4:
	sys.stderr.write("Incorrect call.\n")
	sys.stderr.write("USAGE: make-condor-parser-submit.py parameters_file directory name1 [name2 ...]\n")
	assert(0)

globalfile = wrap(sys.argv[1])
assert(os.access(globalfile, os.R_OK))

dir = wrap(sys.argv[2])
assert(os.path.isdir(dir))

names = sys.argv[3:]
parsefiles = {}
for n in names:
	assert n not in parsefiles
	parsefiles[n] = parsefile(n)
	assert(os.access(parsefiles[n], os.R_OK))

sys.stderr.write("Globals file is %s\n" % globalfile)
sys.stderr.write("Working directory is %s\n" % dir)
sys.stderr.write("Parse files:\n\t%s\n" % string.join(parsefiles.values(), "\n\t"))

startlist = []
endlist  = []
fulllist = []

def uniq(l):
	hsh = {}
	for k in l: hsh[k] = 1
	return hsh.keys()

least_progress = {}

cmd = "cd %s ; %s a | grep '1e+06'" % (dir, os.path.join(scripts_dir, "run-management/last-iterations.pl"))
(cmdin, cmdout) = popen2.popen2(cmd)
for l in cmdin:
	sys.stderr.write("IGNORING %s" % l)
cmdin.close()
cmdout.close()

cmd = "cd %s ; %s a | grep -v '1e+06' | tail -1 | perl -ne 's/.*l1_penalty-//; print;'" % (dir, os.path.join(scripts_dir, "run-management/last-iterations.pl"))
(cmdin, cmdout) = popen2.popen2(cmd)
least_progress[dir] = float(string.split(cmdin.readline())[0])
cmdin.close()
cmdout.close()
sys.stderr.write("Least progress l1 penalty is %g for %s\n" % (least_progress[dir], dir))

(cmdin, cmdout) = popen2.popen2("cat %s/checkpoint.label-* | perl -ne 's/.* //; print;' | sort -rg" % dir)
iterations = uniq([float(l) for l in cmdin])
iterations = [f for f in iterations if f > least_progress[dir]*1.01]		# Remove iterations with too low l1
cmdin.close()
cmdout.close()
#sys.stderr.write("Last iteration in %s is %d\n" % (dir, iterations[-1]))

iterations.sort()
#endlist.append((dir, iterations[-1]))
startlist.append((dir, iterations[0]))
for i in iterations[1:-1]:
	fulllist.append((dir, i))
sys.stderr.write("\n")

random.shuffle(fulllist)
#fulllist = endlist + startlist + fulllist
fulllist = startlist + fulllist


txt = "#!/bin/bash\n\
#PBS -l nodes=1:compute:ppn=1,walltime=24:00:00\n\
#PBS -N %s\n\
ARCH=$(uname -m)\n\
MPICH=/usr/local/mpich/mpichgm-1.2.6..14b/gm/$ARCH/smp/ibmcmp64/ssh\n\
GOTOROOT=/usr/local/goto\n\
IBMCMP=/opt/ibmcmp\n\
PATH=$MPICH/bin:$PATH\n\
LD_LIBRARY_PATH=/opt/gm/lib64:/home/jpt230/utils/lib\n\
#ulimit -s unlimited\n\
\n"


jobcnt = -1
filcnt = 0
for (dir, i) in fulllist:
	if i > max_l1: continue
	assert i > least_progress[dir]*1.01

	for n in names:
		parsedir = wrap(os.path.join(dir, "parse-search-greedy-completion.beam-1M.max_states_popped-100K.%s" % n))
		if not os.path.isdir(parsedir):
			sys.stderr.write("Creating directory %s\n" % parsedir)
			os.mkdir(parsedir)
		assert(os.path.isdir(parsedir))

		if not os.access(os.path.join(parsedir, "NAME"), os.R_OK):
			nfil = open(os.path.join(parsedir, "NAME"), "wt")
			nfil.write("%s\n" % n)
			nfil.close()
		nfil = open(os.path.join(parsedir, "NAME"), "rt")
		n2 = nfil.readlines()
		nfil.close()
		assert n == string.strip(string.join(n2))

		if jobcnt == -1 or jobcnt >= jobs_per_file:
			if jobcnt != -1: f.close()
			jobcnt = 0
			filcnt += 1
			filname = "parse%d.submit" % filcnt
			assert(not os.access(filname, os.R_OK))
			f = open(filname, 'w')
			sys.stderr.write("Writing to %s...\n" % filname)
	
			f.write(txt % ("p%d" % filcnt))
	
		#itxt = "%.3d" % i
		itxt = "%g" % i
		if not os.access("%s/parse-%s.out" % (parsedir, itxt), os.R_OK):
			f.write("cd %s\n" % dir)
			f.write("LD_LIBRARY_PATH=/home/jpt230/utils/lib/ ~/parsing/system/code/build/PPC64/parser-search-greedy-completion -t %s -p %s --min_l1 %s --debuglevel 3 --beam_search_width 1000000 --max_search_states_to_pop 100000 > %s/parse-%s.out 2> %s/parse-%s.err\n\n" % (parsefiles[n], globalfile, itxt, parsedir, itxt, parsedir, itxt))
			jobcnt += 1
f.close()
