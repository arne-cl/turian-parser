Parsing README:

Here's a quick guide on how to parse, in this case using the r2l kitchen sink parser.
Please email the authors if you need any help.

0. For the sake of explication, we refer to the package's directory as 'BASE':
	export BASE="/home/turian/turian-parser-20060604"
Make sure the code in $BASE/code is built, as described in $BASE/code/README.txt
We assume that $BASE/runs/r2l.kitchen-sink contains the parser's hypotheses.

1. Set up path variables for the various scripts:
	cd $BASE/
	./scripts/setup-paths.py
Note that this stores the location of $BASE, so the $BASE directory
should not be moved without subsequently re-running setup-paths.py

2. Link the parser's data files:
	ln -s $BASE/runs/r2l.kitchen-sink/vocabulary.txt $BASE/data/
	ln -s $BASE/runs/r2l.kitchen-sink/labels.txt $BASE/data/

3. Create some file containing JMX-style POS-tagged text:
	echo "Testing_NNP ,_, one_CD two_CD three_CD ._." > $BASE/data/in.txt

4. Convert these sentences to completely flat Penn Treebank-style trees:
	$BASE/scripts/treebank-processing/jmx2tree.py < $BASE/data/in.txt > $BASE/data/in-trees.txt

5. Preprocess the trees:
	$BASE/scripts/treebank-processing/preprocess.py < $BASE/data/in-trees.txt > $BASE/data/in-trees.pre.txt

6. Parse the trees from within the "full vocab" run directory:
	cd $BASE/runs/r2l.kitchen-sink/
	$BASE/code/bin/i686-pc-linux-gnu/parser-search-greedy-completion -p parameters -t $BASE/data/in-trees.pre.txt > $BASE/data/out-trees.txt

7. Postprocess the parser's output:
	$BASE/scripts/treebank-processing/postprocess-specific.py $BASE/data/in-trees.txt $BASE/data/in-trees.txt < $BASE/data/out-trees.txt > $BASE/data/out-trees.post.txt

***

$Id: README-parsing.txt 1656 2006-06-04 02:59:40Z turian $
