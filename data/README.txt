Here is how data/ should be created.

Note that scripts/variables.py and scripts/variables.pm should be
created before doing this preprocessing. In particular, make sure that
the location of JMX (MXPOST) is correctly given in scripts/variables.py

Also, we assume that files:
	ptb-section02-21.joined
and
	ptb-section22.joined
have been created by cat'ing the appropriate sections of the PTB.

####

mkdir orig

ln -s ../scripts/variables.pm	# EDITME FIRST!!!

# Skip long sentences
../scripts/treebank-processing/skip-long-sentences.pl 15 ptb-section02-21.joined > orig/treebank-15words.train
../scripts/treebank-processing/skip-long-sentences.pl 15 ptb-section22.joined > orig/treebank-15words.devel

# Create the vocabulary
../scripts/treebank-processing/clean-treebank.py < orig/treebank-15words.train | ../scripts/treebank-processing/regularize.py  | ../scripts/treebank-processing/create-vocabulary.py

# Create JMX files for training.
cat orig/treebank-15words.train | ../scripts/treebank-processing/clean-treebank.py | ../scripts/treebank-processing/regularize.py | ../scripts/treebank-processing/jmxtag.py tagger.project | ../scripts/treebank-processing/preprocess.py > treebank-15words.train.jmx

# Make static example set (fixed parse paths) for all training files.
../code/bin/i686-pc-linux-gnu/make-treepaths-random -p ../workdata/parameters -t treebank-15words.train.jmx > pathtreebank-15words.train.jmx

../scripts/treebank-processing/clean-treebank.py < orig/treebank-15words.devel > ../data/treebank-parse-postprocess.15-words.devel.gld
../scripts/treebank-processing/regularize.py < ../data/treebank-parse-postprocess.15-words.devel.gld | ../scripts/treebank-processing/jmxtag.py tagger.project > ../data/treebank-parse-postprocess.15-words.devel.jmx
../scripts/treebank-processing/preprocess.py < ../data/treebank-parse-postprocess.15-words.devel.jmx > ../data/treebank-parse.15-words.devel.jmx

