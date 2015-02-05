/******************************************************************************/
/*                                                                            */
/*   This file is part of SWI-LS.                                             */
/*                                                                            */
/*   SWI-LS is free software: you can redistribute it and/or modify           */
/*   it under the terms of the GNU General Public License as published by     */
/*   the Free Software Foundation, either version 2 of the License, or        */
/*   any later version.                                                       */
/*                                                                            */
/*   SWI-LS is distributed in the hope that it will be useful,                */
/*   but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*   GNU General Public License for more details.                             */
/*                                                                            */
/*   You should have received a copy of the GNU General Public License        */
/*   along with SWI-LS.  If not, see <http://www.gnu.org/licenses/>.          */
/*                                                                            */
/******************************************************************************/

/* ************************************************************************** */
/* Piece of software which produces of a summary of the heuristic results     */
/* ************************************************************************** */

#include <set>
#include <vector>
#include <climits>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sys/stat.h>

typedef __int16_t integer;

#define READ_BUFFER_LENGTH 64000

#define ALL 1000

#define NHEURISTICS 7

/* ************************************************************************** */
// Struct to receive the command line parameters
struct Parameters {
  // Input file
  std::string file;
  // Permutation size
  integer n;
  // Signed/Unsigned permutation
  bool sign;
  // List of heuristics to be considered
  std::vector<bool> h;
};
/* ************************************************************************** */

/* ************************************************************************** */
// Prints program usage
void printUsage() {

  std::cerr << std::endl << "Usage: statistics <i> <n> <s> [h]" << std::endl << std::endl;

  std::cerr << "  <i>\tInput file." << std::endl;
  std::cerr << "  <n>\tPermutation size." << std::endl;
  std::cerr << "  <s>\t0 = unsigned permutations or 1 = signed permutations." << std::endl;
  std::cerr << "  <h>\tList of heuristic identifiers separated by comma." << std::endl;
  std::cerr << "     \tOptional parameter. Possible identifiers:" << std::endl;
  std::cerr << "\t\t1 - LR" << std::endl;
  std::cerr << "\t\t2 - SMP" << std::endl;
  std::cerr << "\t\t3 - NB+LR" << std::endl;
  std::cerr << "\t\t4 - NB+STRIP" << std::endl;
  std::cerr << "\t\t5 - NB+BESTSTRIP" << std::endl;
  std::cerr << "\t\t6 - NB+LRSTRIP" << std::endl;
  std::cerr << "\t\t7 - NB+SMP" << std::endl;
  std::cerr << std::endl << std::endl;

  std::cerr << " -----------------------------------------------------------------------" << std::endl;
  std::cerr << " |This program processes the output file produced by the program       |" << std::endl;
  std::cerr << " |processBinaryDatabase.                                               |" << std::endl;
  std::cerr << " -----------------------------------------------------------------------" << std::endl << std::endl;

  std::cerr << " -----------------------------------------------------------------------" << std::endl;
  std::cerr << " |The program will generate one line per heuristic plus one extra line |" << std::endl;
  std::cerr << " |representing the combination of all heuristics (selected ones). This |" << std::endl;
  std::cerr << " |extra heuristic is identified by the number 1000. Each line has the  |" << std::endl;
  std::cerr << " |following columns:                                                   |" << std::endl;
  std::cerr << " |  n    - Permutation size.                                           |" << std::endl;
  std::cerr << " |  s    - 0 (unsigned permutations) or 1 (signed permutations).       |" << std::endl;
  std::cerr << " |  h    - Heuristic identifier.                                       |" << std::endl;
  std::cerr << " |  r1   - % of permutations with ratio 1 (only valid solutions).      |" << std::endl;
  std::cerr << " |  avg  - Average ratio (only valid solutions).                       |" << std::endl;
  std::cerr << " |  max  - Maximum ratio (observed among all permutations.             |" << std::endl;
  std::cerr << " |  best - % of best solutions (considering all permutations).         |" << std::endl;
  std::cerr << " |  err  - % of permutations that got non-valid solutions (i.e. loop or|" << std::endl;
  std::cerr << " |         heuristic error) considering all permutations.              |" << std::endl;
  std::cerr << " |  excl - % of cases where the heuristic was the only one to get the  |" << std::endl;
  std::cerr << " |         best solution (not necessarily optimum).                    |" << std::endl;
  std::cerr << " |  excp - % of permutations (considering all permutations) where only |" << std::endl;
  std::cerr << " |         one heuristic had the best (not necessarily optimum) answer.|" << std::endl;
  std::cerr << " -----------------------------------------------------------------------" << std::endl << std::endl;

  exit(EXIT_FAILURE);
}
/* ************************************************************************** */

/* ************************************************************************** */
// Verifies the list of arguments
Parameters processArguments(int argc, char* argv[]) {

  if (argc != 4 && argc != 5) printUsage();

  bool error = false;

  Parameters toReturn;
  toReturn.file = "";
  toReturn.n    = 0;
  toReturn.sign = true;
  toReturn.h    = std::vector<bool>(NHEURISTICS);

  // File
  struct stat buffer;
  if (stat(argv[1], &buffer) != 0) {
    std::cerr << std::endl << "ERROR!!! Could not access file ";
    std::cerr << argv[1] << std::endl;
    printUsage();
  }
  toReturn.file = std::string(argv[1]);

  // Permutation size
  try {
    toReturn.n = std::stoi(argv[2]);
  } catch (const std::exception& ia) {
    error = true;
  }
  if (error) {
    std::cerr << std::endl << "ERROR!!! Invalid problem identifier." << std::endl;
    printUsage();
  }

  // Signed/Unsigned
  toReturn.sign = std::string(argv[3]).compare("1") == 0;

  // Heuristics to be considered
  for (integer index = 0; index < NHEURISTICS; ++index) {
    toReturn.h[index] = false;
  }


  integer nHeuristics = 0;
  if (argc == 5) {
    std::string aux = std::string(argv[4]);
    size_t index = 0;
    size_t length = aux.length();
    size_t comma = aux.find_first_of(",");
    try {
      if (comma != std::string::npos) {
	while (index < length) {
	  integer id = std::stoi(aux.substr(index, (comma - index)));
	  if (id < 1 || id > 7) {
	    error = true;
	    break;
	  }
	  toReturn.h[id - 1] = true;
	  index = comma + 1;
	  comma = aux.find_first_of(",", index);
	  if (comma == std::string::npos) comma = length;
	  nHeuristics++;
	}
      } else {
	integer id = std::stoi(aux.substr(0, comma));
	if (id < 1 || id > 7) {
	  error = true;
	}
	toReturn.h[id - 1] = true;
	nHeuristics++;
      }
    } catch (const std::exception& ia) {
      error = true;
    }
    if (error) {
      std::cerr << std::endl << "ERROR!!! Invalid heuristic identifier." << std::endl;
      printUsage();
    }
  }

  if (argc != 5 || nHeuristics == 0) {
    // Include all heuristics
    for (integer index = 0; index < NHEURISTICS; ++index) {
      toReturn.h[index] = true;
    }
  }

  return toReturn;
}
/* ************************************************************************** */

/* ************************************************************************** */
// Do the real job
void process(const Parameters parameters) {

  __uint32_t nread = 0;
  __uint64_t read_buffer_length = (NHEURISTICS + 1) * READ_BUFFER_LENGTH;
  __uint64_t read_buffer_size   = read_buffer_length * sizeof(integer);
  integer* read_buffer = new integer[read_buffer_length];

  __int64_t minimum       = 0;
  __int64_t nPermutations = 0;
  __int64_t nExclusive    = 0;
  __int64_t* best       = new __int64_t[NHEURISTICS + 1];
  __int64_t* error      = new __int64_t[NHEURISTICS + 1];
  __int64_t* ratio1     = new __int64_t[NHEURISTICS + 2];
  __int64_t* exclusive  = new __int64_t[NHEURISTICS + 2];
  double* sumRatio      = new double[NHEURISTICS + 1];
  double* maxRatio      = new double[NHEURISTICS + 1];

  integer nHeuristics = 0;
  for (__uint32_t i = 0; i < NHEURISTICS + 1; ++i) {
    best[i] = 0;
    error[i] = 0;
    ratio1[i] = 0;
    sumRatio[i] = 0;
    maxRatio[i] = 0;
    exclusive[i] = 0;
    nHeuristics += parameters.h[i];
  }

  std::ifstream infile;
  infile.open(parameters.file, std::ios::in | std::ios::ate | std::ios::binary);
  infile.seekg (0, std::ios::beg);
  while (!infile.eof()) {

    infile.read(reinterpret_cast<char *>(read_buffer), read_buffer_size);
    nread = infile.gcount() / sizeof(integer);

    for (__uint32_t i = 0; i < nread; i += (NHEURISTICS + 1)) {

      nPermutations++;

      integer optimum = read_buffer[i];
      minimum = INT_MAX;

      for (__uint32_t j = 0; j < NHEURISTICS; ++j) {
	if (parameters.h[j]) {
	  integer result = read_buffer[i + j + 1];
	  if (result != -1 && result < minimum) minimum = result;
	  if (optimum == result) {
	    ratio1[j] = ratio1[j] + 1;
	    sumRatio[j] = sumRatio[j] + 1;
	    if (maxRatio[j] == 0) maxRatio[j] = 1;
	  } else {
	    if (optimum == 0 || result == -1) {
	      error[j] = error[j] + 1;
	    } else {
	      double ratio = (double) result / (double) optimum;
	      if (ratio > maxRatio[j]) {
		maxRatio[j] = ratio;
	      }
	      sumRatio[j] = sumRatio[j] + ratio;
	    }
	  }
	}
      } // for (__uint32_t j = 0; j < NHEURISTICS; ++j) {...}

      for (__uint32_t j = 0; j < NHEURISTICS; ++j) {
	if (parameters.h[j] && read_buffer[i + j + 1] == minimum) {
	  best[j] = best[j] + 1;
	}
      } // for (__uint32_t j = 0; j < NHEURISTICS; ++j) {...}

      if (minimum == INT_MAX) {
	error[NHEURISTICS] = error[NHEURISTICS] + 1;
      } else {

	for (__uint32_t j = 0; j < NHEURISTICS; ++j) {
	  if (parameters.h[j]) {
	    integer result1 = read_buffer[i + j + 1];
	    if (result1 == minimum) {
	      integer count = 0;
	      for (__uint32_t k = 0; k < NHEURISTICS; ++k) {
		if (k != j && parameters.h[k]) {
		  integer result2 = read_buffer[i + k + 1];
		  if (result1 < result2) ++count;
		}
	      }
	      if (count == nHeuristics - 1) {
		exclusive[j] = exclusive[j] + 1;
		++nExclusive;
	      }
	    }
	  }
	} // for (__uint32_t j = 0; j < NHEURISTICS; ++j) {...}

	if (minimum == optimum) {
	  ratio1[NHEURISTICS] = ratio1[NHEURISTICS] + 1;
	  if (maxRatio[NHEURISTICS] == 0) maxRatio[NHEURISTICS] = 1;
	  sumRatio[NHEURISTICS] = sumRatio[NHEURISTICS] + 1;
	} else {
	  double ratio = (double) minimum / (double) optimum;
	  if (ratio > maxRatio[NHEURISTICS]) {
	    maxRatio[NHEURISTICS] = ratio;
	  }
	  sumRatio[NHEURISTICS] = sumRatio[NHEURISTICS] + ratio;
	} // if (minimum == optimum) {...} else {...}

	best[NHEURISTICS] = best[NHEURISTICS] + 1;
      } // if (minimum == INT_MAX) {...} else {...}

    } // for (__uint32_t i = 0; i < nread; i += nColumns) {...}

  } // while (!infile.eof()) {...}
  infile.close();

  if (nPermutations > 0) {

    for (__uint32_t i = 0; i <= NHEURISTICS; ++i) {

      if ((i < NHEURISTICS && parameters.h[i]) || i == NHEURISTICS) {

	__uint64_t nValid = nPermutations - error[i];

	if (i != NHEURISTICS)
	  std::cout << parameters.n << "\t" << parameters.sign << "\t" << (i + 1) << "\t";
	else
	  std::cout << parameters.n << "\t" << parameters.sign << "\t" << ALL << "\t";

	if (nValid > 0) {
	  double ratio1P  = (ratio1[i] * 100.0) / nValid;
	  double avgRatio = sumRatio[i] / nValid;
	  double bestP    = (best[i] * 100.0) / nPermutations;
	  double errorP   = (error[i] * 100.0) / nPermutations;
	  std::cout << std::fixed << std::setprecision(3) << ratio1P << "\t" << avgRatio << "\t" << maxRatio[i] << "\t" << bestP << "\t" << errorP;
	  if (nExclusive > 0 && i != NHEURISTICS) {
	    double exclusiveP = (exclusive[i] * 100.0) / nExclusive;
	    double percentage = (nExclusive * 100.0) / nPermutations;
	    std::cout << "\t" << std::fixed << std::setprecision(3) << exclusiveP << "\t" << percentage << std::endl;
	  } else {
	    std::cout << "\tNA\tNA" << std::endl;
	  }
	} else {
	  std::cout << "NA\tNA\tNA\tNA\t100.000\tNA\tNA" << std::endl;
	}

      } // if ((i < NHEURISTICS && parameters.h[i]) || i == NHEURISTICS) {...}

    } // for (__uint32_t i = 0; i <= NHEURISTICS; ++i) {...}

  } // if (nPermutations > 0) {...}

  delete read_buffer;
  delete ratio1;
  delete sumRatio;
  delete maxRatio;
  delete error;
}
/* ************************************************************************** */

/* ************************************************************************** */
// Main program
int main (int argc, char* argv[]) {
  process(processArguments(argc, argv));
  return 0;
}
/* ************************************************************************** */
