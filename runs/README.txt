Training README:

Here's a quick guide on how to train.
Please email the authors if you need any help.

Basically, the idea is that we create one subdirectory in 'runs/' for
each run (i.e. set of parameters/variables).
Each should have its own code/, data/, and scripts/ subdirectory.

Here's how the runs are created:

0. For the sake of explication, we refer to the package's directory as 'BASE':
	export BASE="/home/turian/turian-parser-20060604"
Make sure the code in $BASE/code is built, as described in $BASE/code/README.txt

1. Modify $BASE/scripts/run-management/create-run-directories.pl for different directories.

2. Enter directory $BASE/runs/

3. Run $BASE/scripts/run-management/create-run-directories.pl

4. Create $BASE/runs/*/data/ files, as described in $BASE/data/README.txt
(Note that $BASE/runs/*/ means each run directory created.)

5. Create $BASE/runs/*/workdata/parameters
NB the parameters used to create the "r2l (kitchen sink)" parser are provided
in $BASE/runs/parameters.r2l-kitchen-sink. You'll need to modify
VOCABULARY_FILENAME and LABEL_FILENAME therein to point to the appropriate
files in the $BASE/runs/*/data/ directory.

6. Train each hypothesis:
	cd $BASE/runs/*/workdata/workdata.full/
and, for LABEL in 1 .. 28, run:
	$BASE/code/bin/i686-pc-linux-gnu/train -p ../parameters -t ../../data/pathtreebank-15words.train.jmx -l LABEL

***

$Id: README.txt 1656 2006-06-04 02:59:40Z turian $
