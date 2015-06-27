#!/usr/bin/python
#
#!/usr/local/bin/python2.3
#
#   Generate condor submit file for parsing directories given in ARGV.
#
#   make-condor-parser-submit.py parameters_file directory name1 [name2 ...]
#   	name? = "15-words.devel", for example
#
#   $Id: make-condor-parsersearch-submit.py 1657 2006-06-04 03:03:05Z turian $
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

import sys, os, popen2, random

#jobs_per_file = 10
jobs_per_file = 4
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

print condor_parser_search_greedy_completion_linux_cmd
assert(os.access(condor_parser_search_greedy_completion_linux_cmd, os.X_OK))
#assert(os.access(condor_parser_search_greedy_completion_sun_cmd, os.X_OK))

if host != "coop" and host != "chicken":
	txt = " \n\
NiceUser		= true\n\
#Priority		= -19\n\
Executable		= %s\n\
Universe		= Vanilla\n\
Notification		= Error\n\
Requirements		= ((Arch == \"SUN4u\" && OpSys == \"SOLARIS29\") || (Arch == \"INTEL\" && OpSys == \"LINUX\")) && UidDomain == \"cs.nyu.edu\" && FileSystemDomain == \"cs.nyu.edu\" && Machine != \"int1.cims.nyu.edu\" && Machine != \"int2.cims.nyu.edu\" && Machine != \"int3.cims.nyu.edu\" && Machine != \"int4.cims.nyu.edu\" && Machine != \"int5.cims.nyu.edu\"\n\
ShouldTransferFiles	= No\n\
Copy_to_spool           = False\n\
Image_Size		= 128 Meg\n\n\n"
else:
	txt = " \n\
NiceUser		= true\n\
#Priority		= -19\n\
Executable		= %s\n\
Universe		= Vanilla\n\
Notification		= Error\n\
Requirements		= (Arch == \"INTEL\" && OpSys == \"LINUX\") && UidDomain == \"clickroot\" && FileSystemDomain == \"clickroot\"\n\
ShouldTransferFiles	= No\n\
Copy_to_spool           = False\n\
Image_Size		= 128 Meg\n\n\n"

jobcnt = -1
filcnt = 0
for (dir, i) in fulllist:
	if i > max_l1: continue
	assert i > least_progress[dir]*1.01

	for n in names:
		#parsedir = os.path.normpath(os.path.join(dir, "parse-search-greedy-completion.beam-1000.max_states_popped-1000000.%s" % n))
		parsedir = wrap(os.path.join(dir, "parse-search-greedy-completion.beam-1000.max_states_popped-1000000.%s" % n))
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
	
			f.write(txt % condor_parser_search_greedy_completion_opsys_cmd)
	
		#itxt = "%.3d" % i
		itxt = "%g" % i
		if not os.access("%s/parse-%s.out" % (parsedir, itxt), os.R_OK):
			f.write("Initialdir\t= %s\n" % dir)
			f.write("Arguments\t= -t %s -p %s --min_l1 %s --debuglevel 3 --beam_search_width 1000 --max_search_states_to_pop 1000000\n" % (parsefiles[n], globalfile, itxt))
			f.write("Output\t= %s/parse-%s.out\n" % (parsedir, itxt))
			f.write("Error\t= %s/parse-%s.err\n" % (parsedir, itxt))
			f.write("Log\t= %s/parse-%s.log\n" % (parsedir, itxt))
			f.write("Queue\n\n")
			jobcnt += 1
f.close()
