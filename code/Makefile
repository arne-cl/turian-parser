# TODO
#	* Other targets
#	* Sandbox
#	* Make a single rule for both target executables
#	* Put all object files in a flat directory
#	* Create 'opt' and 'debug' build subdirectories
# $Id: Makefile 1653 2006-06-04 02:05:37Z turian $

# Or you can set this to the code directory, e.g., "/home/turian/turian-parser-20060604/code"
CODE = .	

# Set the following three variables to match your compiler settings.
CMP = /usr
# Compiling
CC	=   $(CMP)/bin/g++
#CC	=   distcc $(CMP)/bin/g++
# Linking
LCC	=   $(CMP)/bin/g++
#LCC	=   distcc $(CMP)/bin/g++
AR	=   $(CMP)/bin/ar
RANLIB	=   $(CMP)/bin/ranlib
FLEX	=   $(CMP)/bin/flex

# set the following to point to boost, if it does not live in $(CMP)/include
# BOOST_INCLUDES = -I $(CMP)/include

# Installation of the boost libraries can result in library files with
# or without the -gcc ending on their name.  Choose the variant on
# your machine.
BOOST_LIBS =  -lboost_program_options -lboost_date_time -lboost_iostreams
#BOOST_LIBS =  -lboost_program_options-gcc -lboost_date_time -lboost_iostreams-gcc

# CC options used when compiling and linking
EXTRA_CC_ARGS = -Wall -pedantic -g -pipe -O3

###############################################################################
#
#  You probably don't need to change anything past this point.
#

# correct for extra slashes at the end of pathnames
CODEDIR=$(patsubst %/, %, $(CODE))
BIN_CODE = $(CODEDIR)/bin
OBJ_CODE = $(CODEDIR)/obj
BIN = $(BIN_CODE)/$(CONFIG_ARCH)
OBJ = $(OBJ_CODE)/$(CONFIG_ARCH)
SRC = $(CODEDIR)/src

CONFIG_ARCH := $(shell $(CODEDIR)/config.guess)

# Library target for this build
LIB = $(BIN)/libparse.a
LIB_OBJS = \
		$(OBJ)/learn/Checkpoint.o \
		$(OBJ)/learn/Classifier.o \
		$(OBJ)/learn/Cost.o \
		$(OBJ)/learn/Weights.o \
		$(OBJ)/learn/decision-trees/BuildNode.o \
		$(OBJ)/learn/decision-trees/BuildTree.o \
		$(OBJ)/learn/decision-trees/Ensemble-tune.o \
		$(OBJ)/learn/decision-trees/Ensemble.o \
		$(OBJ)/learn/decision-trees/EnsembleTokenizer.o \
		$(OBJ)/learn/decision-trees/Node.o \
		$(OBJ)/learn/decision-trees/Tree.o \
		$(OBJ)/learn/decision-trees/Treelex.o \
		$(OBJ)/learn/examples/AbstractExample.o \
		$(OBJ)/learn/examples/Example.o \
		$(OBJ)/learn/examples/Examples.o \
		$(OBJ)/learn/examples/FullExample.o \
		$(OBJ)/learn/examples/Generator.o \
		$(OBJ)/learn/examples/IntermediateExample.o \
		$(OBJ)/learn/examples/Sampler.o \
		$(OBJ)/learn/features/ExistsFeatures.o \
		$(OBJ)/learn/features/ExistsTerminalFeatures.o \
		$(OBJ)/learn/features/Features.o \
		$(OBJ)/learn/features/GroupIterator.o \
		$(OBJ)/learn/features/ItemChildFeatures.o \
		$(OBJ)/learn/features/ItemFeatures.o \
		$(OBJ)/learn/features/LengthFeatures.o \
		$(OBJ)/learn/features/QuantFeatures.o \
		$(OBJ)/learn/features/RangeFeatures.o \
		$(OBJ)/learn/features/TerminalItemFeatures.o \
		$(OBJ)/learn/features/features-help.o \
		$(OBJ)/learn/loss.o \
		$(OBJ)/parse/Chart.o \
		$(OBJ)/parse/ItemToken.o \
		$(OBJ)/parse/ParseState.o \
		$(OBJ)/parse/Parser-help.o \
		$(OBJ)/parse/Parser-search.o \
		$(OBJ)/parse/Parser.o \
		$(OBJ)/parse/PennTokenizer.o \
		$(OBJ)/parse/SearchQueue.o \
		$(OBJ)/parse/Span.o \
		$(OBJ)/parse/SpanChart.o \
		$(OBJ)/parse/SpanItem.o \
		$(OBJ)/parse/State.o \
		$(OBJ)/parse/Treebank.o \
		$(OBJ)/parse/Treebanklex.o \
		$(OBJ)/parse/head.o \
		$(OBJ)/universal/Debug.o \
		$(OBJ)/universal/Double.o \
		$(OBJ)/universal/Random.o \
		$(OBJ)/universal/parameter-storage.o \
		$(OBJ)/universal/parameter.o \
		$(OBJ)/universal/posclass.o \
		$(OBJ)/universal/stats.o \
		$(OBJ)/universal/vocabulary-freq.o \
		$(OBJ)/universal/vocabulary.o

CC_ARGS = $(EXTRA_CC_ARGS) -DBOOST_SP_DISABLE_THREADS $(BOOST_INCLUDES) -I$(SRC)
LIBS = $(BOOST_LIBS)

ALL: dir $(LIB) $(BIN)/train $(BIN)/parser-search-greedy-completion $(BIN)/make-treepaths-random

$(LIB): \
		$(LIB_OBJS)
	$(AR) r $@ $^
	$(RANLIB) $@

$(BIN)/train: \
		$(OBJ)/train.o \
		$(LIB)
	@echo
	$(LCC) $(CC_ARGS) $< $(LIBS) -o $(BIN)/train $(LIB)

$(BIN)/parser-search-greedy-completion: \
		$(OBJ)/parser-search-greedy-completion.o \
		$(LIB)
	@echo
	$(LCC) $(CC_ARGS) $< $(LIBS) -o $(BIN)/parser-search-greedy-completion $(LIB)

$(BIN)/make-treepaths-random: \
		$(OBJ)/make-treepaths-random.o \
		$(LIB)
	@echo
	$(LCC) $(CC_ARGS) $< $(LIBS) -o $(BIN)/make-treepaths-random $(LIB)

dir: $(BIN) $(OBJ) \
		$(OBJ)/learn/decision-trees/ \
		$(OBJ)/learn/examples/ \
		$(OBJ)/learn/features/ \
		$(OBJ)/parse/ \
		$(OBJ)/universal/
	@echo

$(OBJ): 
	-mkdir -p $(OBJ)

$(BIN):
	-mkdir -p $(BIN)

$(OBJ)/learn/decision-trees:
	-mkdir -p $(OBJ)/learn/decision-trees

$(OBJ)/learn/examples:
	-mkdir -p $(OBJ)/learn/examples

$(OBJ)/learn/features:
	-mkdir -p $(OBJ)/learn/features

$(OBJ)/parse:
	-mkdir -p $(OBJ)/parse

$(OBJ)/universal:
	-mkdir -p $(OBJ)/universal

$(OBJ)/learn/decision-trees/EnsembleTokenizer.o: $(OBJ)/learn/decision-trees/EnsembleTokenizer.C
	$(CC) $(CC_ARGS) -c -o $@ $<

$(OBJ)/learn/decision-trees/EnsembleTokenizer.C: $(SRC)/learn/decision-trees/EnsembleTokenizer.ll
	$(FLEX) -Pensemble -8 -Cfar -+ -t $< > $@

$(OBJ)/parse/PennTokenizer.o: $(OBJ)/parse/PennTokenizer.C
	$(CC) $(CC_ARGS) -c -o $@ $<

$(OBJ)/parse/PennTokenizer.C: $(SRC)/parse/PennTokenizer.ll
	$(FLEX) -Ppenn -8 -Cfar -+ -t $< > $@

# object files
$(OBJ)/%.o: $(SRC)/%.C
	$(CC) $(CC_ARGS) -c -o $@ $<

clean:
	rm -Rf $(BIN_CODE) $(OBJ_CODE)
