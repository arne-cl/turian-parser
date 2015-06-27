	HOW TO BUILD THE CODE-BASE

***

QUICKSTART GUIDE:

make	# Invoke with fingers crossed

If that does not work, tweak the variables at the top of the Makefile.
You may wish to prefix g++ with 'distcc' and 'make -jJOBS' to speed
compilation.

***

DEPENDENCIES:

Boost library		http://www.boost.org
			Specifically, we use only the headers and the
			program_options library.

Google Sparse Hash	http://goog-sparsehash.sourceforge.net/
			We've tested our code using version 0.3

Lex			Probably only flex is sufficient.

patch			[optional] Unfortunately, C++ scanners generated
			by flex need some special patching magic on
			certain systems (e.g. SunOS). If you are lucky
			enough to have be on one of these systems,
			you will have to patch EnsembleTokenizer.C and
			PennTokenizer.C manually. Included are patch
			files that we use.

***

FEEDBACK:

Questions? Comments? Suggestions?
	Bugs? Code patches? Feature requests?
	 
This is research software and we welcome all feedback.
Please let us know if you experience any difficulty with the build process,
even if you successfully resolve it.

Feedback should be emailed to Joseph Turian and I. Dan Melamed:
	Joseph Turian <turian at cs dot nyu dot edu>
	I. Dan Melamed <melamed at cs dot nyu dot edu>

***

$Id: README.txt 1652 2006-06-04 01:32:05Z turian $
