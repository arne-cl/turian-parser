#!/usr/bin/python
#
#!/usr/local/bin/python2.3
#
#   Generate condor submit file for parsing directories given in ARGV.
#
#   ./make-condor-parser-submit.py globals directory1 [directory2 ...]
#
#   $Id: make-condor-parser-submit.py 1657 2006-06-04 03:03:05Z turian $
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

import sys, os, popen2, random

from variables import *
from host import *
host = get_host()

if len(sys.argv) < 3:
	sys.stderr.write("Incorrect call.\n")
	sys.stderr.write("USAGE: ./make-condor-parser-submit.py parameters_file directory1 [directory2 ...]\n")
	assert(0)

develfile = parsefile["devel"]
trainfile = parsefile["train"]
globalfile = wrap(sys.argv[1])

assert(os.access(develfile, os.R_OK))
assert(os.access(trainfile, os.R_OK))
assert(os.access(globalfile, os.R_OK))

sys.stderr.write("Devel file: %s\n" % develfile)
sys.stderr.write("Train file: %s\n" % trainfile)
sys.stderr.write("Globals file is %s\n" % globalfile)

startlist = []
endlist  = []
fulllist = []

def uniq(l):
	hsh = {}
	for k in l: hsh[k] = 1
	return hsh.keys()

least_progress = {}
for dir in sys.argv[2:]:
	dir = wrap(dir)
	assert(os.path.isdir(dir))
	sys.stderr.write("Covering directory %s\n" % dir)

	parsedir = os.path.normpath(dir + "/parses/")
	if not os.path.isdir(parsedir):
		sys.stderr.write("Creating directory %s\n" % parsedir)
		os.mkdir(parsedir)
	assert(os.path.isdir(parsedir))
	sys.stderr.write("\n")

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
	sys.stderr.write("Least progress l1 penalty is %f for %s\n" % (least_progress[dir], dir))

	(cmdin, cmdout) = popen2.popen2("cat %s/checkpoint.label-* | perl -ne 's/.* //; print;' | sort -rg" % dir)
	iterations = uniq([float(l) for l in cmdin])
	cmdin.close()
	cmdout.close()
	#sys.stderr.write("Last iteration in %s is %d\n" % (dir, iterations[-1]))

	iterations.sort()
	endlist.append((dir, iterations[-1]))
	startlist.append((dir, iterations[0]))
	for i in iterations[1:-1]:
		fulllist.append((dir, i))
	sys.stderr.write("\n")

random.shuffle(fulllist)
fulllist = endlist + startlist + fulllist

assert(os.access(condor_parser_linux_cmd, os.X_OK))
assert(os.access(condor_parser_sun_cmd, os.X_OK))

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
	if i <= least_progress[dir] + 0.001: continue
	if jobcnt == -1 or jobcnt >= 20:
		if jobcnt != -1: f.close()
		jobcnt = 0
		filcnt += 1
		filname = "parse%d.submit" % filcnt
		assert(not os.access(filname, os.R_OK))
		f = open(filname, 'w')
		sys.stderr.write("Writing to %s...\n" % filname)

		f.write(txt % condor_parser_opsys_cmd)

	#itxt = "%.3d" % i
	itxt = "%g" % i
	if not os.access("%s/parses/parse-devel-%s.out" % (dir, itxt), os.R_OK):
#		assert not os.access("%s/parses/parse-train-%s.out" % (dir, itxt), os.R_OK)
		f.write("Initialdir\t= %s\n" % dir)
		#f.write("Arguments\t= -t %s -p %s --min_l1 %s  --need_END_token=false\n" % (develfile, globalfile, itxt))
		f.write("Arguments\t= -t %s -p %s --min_l1 %s\n" % (develfile, globalfile, itxt))
		f.write("Output\t= %s/parses/parse-devel-%s.out\n" % (dir, itxt))
		f.write("Error\t= %s/parses/parse-devel-%s.err\n" % (dir, itxt))
		f.write("Log\t= %s/parses/parse-devel-%s.log\n" % (dir, itxt))
		f.write("Queue\n\n")

		#f.write("Arguments\t= -t %s -p %s --min_l1 %s  --need_END_token=false\n" % (trainfile, globalfile, itxt))
		f.write("Arguments\t= -t %s -p %s --min_l1 %s\n" % (trainfile, globalfile, itxt))
		f.write("Output\t= %s/parses/parse-train-%s.out\n" % (dir, itxt))
		f.write("Error\t= %s/parses/parse-train-%s.err\n" % (dir, itxt))
		f.write("Log\t= %s/parses/parse-train-%s.log\n" % (dir, itxt))
		f.write("Queue\n\n")

		jobcnt += 2
#	else:
#		assert os.access("%s/parses/parse-train-%s.out" % (dir, itxt), os.R_OK)
f.close()
