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
/* Piece of software for processing a database of permutations                */
/* ************************************************************************** */

#include <problems/problems.hpp>
#include <heuristics/heuristics.hpp>
#include <permutation/permutation.hpp>

#define NHEURISTICS 7

/* ************************************************************************** */
// Struct to receive the command line parameters
struct Parameters {
  // Input permutation
  Permutation permutation;
  // List of heuristics to be considered
  std::vector<bool> h;
};
/* ************************************************************************** */

/* ************************************************************************** */
// Prints program usage
void printUsage() {

  std::cerr << std::endl << "Usage: processPermutation <s> <p> [h]" << std::endl << std::endl;

  std::cerr << "  <s>\t0 = unsigned permutations or 1 = signed permutations." << std::endl;
  std::cerr << "  <p>\tPermutation." << std::endl;
  std::cerr << "  [h]\tList of heuristic identifiers separated by comma." << std::endl;
  std::cerr << "     \tOptional parameter. Possible identifiers:" << std::endl;
  std::cerr << "\t\t1 - LR" << std::endl;
  std::cerr << "\t\t2 - SMP" << std::endl;
  std::cerr << "\t\t3 - NB+LR" << std::endl;
  std::cerr << "\t\t4 - NB+STRIP" << std::endl;
  std::cerr << "\t\t5 - NB+BESTSTRIP" << std::endl;
  std::cerr << "\t\t6 - NB+LRSTRIP" << std::endl;
  std::cerr << "\t\t7 - NB+SMP" << std::endl;

  std::cerr << " ----------------------------------------------------------------------" << std::endl;
  std::cerr << " |This program processes the given permutation accordingly with the   |" << std::endl;
  std::cerr << " |problem SWI-LS. In this problem, permutations can be sorted by a    |" << std::endl;
  std::cerr << " |sequence composed by any type of inversions. The weight of the      |" << std::endl;
  std::cerr << " |inversions are given by k + 1, where k is given by the absolute     |" << std::endl;
  std::cerr << " |difference of the slice numbers of the inversion extremities.       |" << std::endl;
  std::cerr << " ----------------------------------------------------------------------" << std::endl << std::endl;

  std::cerr << " ----------------------------------------------------------------------" << std::endl;
  std::cerr << " |The given permutation is processed by the selected heuristics       |" << std::endl;
  std::cerr << " |For each heuristic, the sequence of inversions and its total weight.|" << std::endl;
  std::cerr << " ----------------------------------------------------------------------" << std::endl << std::endl;

  exit(EXIT_FAILURE);

}
/* ************************************************************************** */

/* ************************************************************************** */
// Verifies the list of arguments
Parameters processArguments(int argc, char* argv[]) {

  if (argc != 3 && argc != 4) printUsage();

  bool error = false;

  Parameters toReturn;
  toReturn.h = std::vector<bool>(NHEURISTICS);

  // Signed/Unsigned
  bool sign = std::string(argv[1]).compare("1") == 0;

  // Permutation
  permutation_vector permutation;
  std::string aux = std::string(argv[2]);
  size_t index = 0;
  size_t length = aux.length();
  size_t comma = aux.find_first_of(",");
  try {
    if (comma != std::string::npos) {
      while (index < length) {
        integer element = std::stoi(aux.substr(index, (comma - index)));
	permutation.push_back(element);
        index = comma + 1;
        comma = aux.find_first_of(",", index);
        if (comma == std::string::npos) comma = length;
      }
    } else {
      integer element = std::stoi(aux.substr(0, comma));
      permutation.push_back(element);
    }
  } catch (const std::exception& ia) {
    error = true;
  }
  if (error) {
    std::cerr << std::endl << "ERROR!!! Could not parse the permutation string." << std::endl;
    printUsage();
  }

  toReturn.permutation = Permutation(permutation, sign);

  // Heuristics to be considered
  for (integer index = 0; index < NHEURISTICS; ++index) {
    toReturn.h[index] = false;
  }

  integer nHeuristics = 0;
  if (argc == 4) {
    std::string aux = std::string(argv[3]);
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

  if (argc != 4 || nHeuristics == 0) {
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

  Permutation pi = parameters.permutation;

  Problem problem = Problem(SWI_LS, pi.size(), pi.isSigned());

  std::cout << "------------------------------------------------------" << std::endl;

  for (integer h = 1; h <= 7; ++h) {
    if (parameters.h[h - 1]) {
      switch (h) {
      case LR:
	std::cout << "LR           : ";
	break;
      case SMP:
	std::cout << "SMP          : ";
	break;
      case NB_PLUS_LR:
	std::cout << "NB+LR        : ";
	break;
      case NB_PLUS_STRIP:
	std::cout << "NB+STRIP     : ";
	break;
      case NB_PLUS_BESTSTRIP:
	std::cout << "NB+BESTSTRIP : ";
	break;
      case NB_PLUS_LRSTRIP:
	std::cout << "NB+LRSTRIP   : ";
	break;
      case NB_PLUS_SMP:
	std::cout << "NB+SMP       : ";
	break;
      }

      integer weight = 0;
      Inversions inversions = Heuristics::sort(pi, problem, h, weight);
      if (weight < 0) {
	std::cout << "Loop or heuristic error." << std::endl;
      } else {
	std::cout << weight << std::endl;
      }

      int nInversions = inversions.size();
      if (nInversions > 0) {
	std::cout << inversions[0];
	for (integer index = 1; index < nInversions; ++index) {
	  std::cout << std::endl << inversions[index];
	}
      }

      std::cout << std::endl << "------------------------------------------------------" << std::endl;
    }
  }

}
/* ************************************************************************** */

/* ************************************************************************** */
// Main program
int main (int argc, char* argv[]) {
  process(processArguments(argc, argv));
  return 0;
}
/* ************************************************************************** */
