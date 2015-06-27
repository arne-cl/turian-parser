
Scripts used for getting results and performing EDA.

####################################################################

Contents of this directory:

README.txt
	This file ;*)

fms.pl 
	Compute the F-measure of PARSEVAL output.

guided-parser.py
	Perform *guided* (i.e. with the oracle) parsing of text, and
	report the number of incorrect (state) decisions.

	Correct decisions do not compound, since a correct decision is
	always performed.
	Incorrect decisions *can* compound, since an incorrect decision
	may be the highest confidence decision for several states in a
	row.

guided-preparser.py
	Perform *guided* (i.e. with the oracle) parsing of text, *but
	stop parsing each sentence when we wish to perform an incorrect
	decision*.
	Report the number of incorrect (state) decisions.

	Correct decisions do not compound, since a correct decision is
	always performed.
	Incorrect decisions do not compound, since an incorrect
	decision causes the parser to stop parsing this sentence.

percent-incorrect.pl
	Finds the percent of lines that contain:
		correct=0
	out of the number of lines that contain:
		correct=[01]

state-error.pl
	TESTME: I'M NOT SURE IF THIS PROGRAM WORKS!

	Given an (unshuffled) set of preparsed examples, determine the
	percent of states in which we would commit an error (i.e. the
	example/label pair assigned highest confidence is incorrect).

***

$Id: README.txt 1287 2005-07-26 08:15:13Z turian $
