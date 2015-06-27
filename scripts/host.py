#######################################################################
#
#
#   host.py
#
#   Find the host: "linux", "sun", "euler08", "coop", or "chicken"
#
#   TODO
#	* Use the PATH environment variable to find command locations?
#
#
#   $Id: host.py 1649 2006-06-04 00:34:58Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

import os,os.path,popen2,string,sys

def run_cmd(cmd):
	(pin,pout) = popen2.popen2(cmd)
	s = string.strip(pin.read())
	pin.close()
	pout.close()
	return s


# TODO: Use the PATH environment variable to find command locations?
def get_cmd(cmd, prelocs=[]):
	locs = prelocs + ["/bin/", "/usr/bin/", "/usr/ucb/", "/usr/local/bin/"]
	for l in locs:
		s = os.path.join(l, cmd)
		if os.access(s, os.X_OK): return s
	sys.stderr.write("FATAL: %s not present anywhere in %s\n" % (cmd, `locs`))
	assert 0

def get_host():
	host = None
	
	(pin,pout) = popen2.popen2(get_cmd('uname'))
	nom = pin.readline()
	pin.close()
	pout.close()
	if nom == 'Linux\n':
		(pin,pout) = popen2.popen2(get_cmd('hostname'))
		nom = pin.readline()
		pin.close()
		pout.close()
		if nom[:5] == 'euler':
			host = "euler08"
		elif nom[-10:-1] == "clickroot":
			host = "chicken"
		elif len(nom) < 4:
			host = "chicken"
		else:
			host = "linux"
	elif nom == 'SunOS\n':
		host = "sun"
	else:
		sys.stderr.write("I don't know about this host-type: %s\n" % nom)
		assert 0
	
	assert host
	return host

def whoami():
	(pin,pout) = popen2.popen2(get_cmd('whoami'))
	nom = string.strip(pin.readline())
	pin.close()
	pout.close()
	return nom

# Return the result of 'config.guess', which may be cached as an environment variable.
def config_guess():
	if "CONFIG_GUESS" not in os.environ:
		# Find config.guess command and run it
		# TODO: For all found versions, chose the one with the latest timestamp
		cmd = get_cmd("config.guess", prelocs=string.split(". /usr/share/libtool/ /usr/share/automake-1.9/ /usr/local/share/automake-1.9/ /usr/share/automake-1.8/ /usr/local/share/automake-1.8/ /usr/share/automake-1.7/ /usr/local/share/automake-1.7/ /usr/share/automake-1.6/ /usr/local/share/automake-1.6/ /usr/share/automake-1.5/ /usr/local/share/automake-1.5/ /usr/share/automake-1.4/ /usr/local/share/automake-1.4"))

		(pin,pout) = popen2.popen2(cmd)
		s = string.strip(pin.readline())
		pin.close()
		pout.close()
		sys.stderr.write("`%s` gives %s\n" % (cmd, s))
		os.environ["CONFIG_GUESS"] = s

	return os.environ["CONFIG_GUESS"]

# Return the result of 'hostname', which may be cached as an environment variable.
# TODO: Combine with above function, don't duplicate code.
def hostname():
	if "HOSTNAME" not in os.environ:
		# Find config.guess command and run it
		# TODO: For all found versions, chose the one with the latest timestamp
		cmd = get_cmd("hostname")

		(pin,pout) = popen2.popen2(cmd)
		s = string.strip(pin.readline())
		pin.close()
		pout.close()
		sys.stderr.write("`%s` gives %s\n" % (cmd, s))
		os.environ["HOSTNAME"] = s

	return os.environ["HOSTNAME"]

# Return either 'euler', 'condor', or 'max'
def job_type():
	h = hostname()
	if h in ['euler01.cims.nyu.edu']: ty = 'euler'
	elif h in ['l1', 'l2', 'l3', 'l4', 'l5']: ty = 'max'
	elif h in ['c1.cs.nyu.edu', 'c2.cs.nyu.edu', 'c3.cs.nyu.edu', 'c4.cs.nyu.edu', 'c5.cs.nyu.edu']: ty = 'condor'
	else: assert 0
	sys.stderr.write("Job type:\t%s\n" % ty)
	return ty
