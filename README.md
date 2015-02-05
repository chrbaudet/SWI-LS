# SWI-LS
Sorting by Weighted Inversions Considering Length and Symmetry

This package implements heuristics for solving the SWI-LS problem.

-------------------------------------------------------------------------------- 
GENERAL INFORMATION:

The directory database contains software for generating databases for testing
purpose.

The directory heuristics contains software for processing databases or individual permutations.

-------------------------------------------------------------------------------- 
COMPILING THE CODE:

To compile the code, go to the desired directory (database or heuristics) and
execute make:

make

To clean-up, type:

make clean

If you have problems to compile, edit Makefile and change the line:
STDLIB=c++11
by
STDLIB=c++0x
If it does not work, please, contact us.
