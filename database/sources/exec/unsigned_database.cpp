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

/******************************************************************************/
/* Generates a database of signed permutations of size N                      */
/******************************************************************************/

#include <map>
#include <queue>
#include <climits>
#include <fstream>
#include <iostream>
#include <unordered_set>

#include <linear/unsigned.hpp>
#include <problem/problem.hpp>

#define BUFFER_SIZE 64000

struct Parameters {
  element n;
  bool binary;
  std::string file;
};

// Prints program usage.
void printUsage() {

  std::cerr << std::endl << "Usage: unsigned_database <n> <b> <o>" << std::endl << std::endl;
  std::cerr << "  <n>\tPermutation size in the interval [1," << N_MAX << "]" << std::endl;
  std::cerr << "  <b>\tOutput format: 0 - text or 1 - binary" << std::endl;
  std::cerr << "  <o>\tOutput file name" << std::endl << std::endl;

  std::cerr << " ------------------------------------------------------------------------" << std::endl;
  std::cerr << " |This program generates database files containing all possible unsigned|" << std::endl;
  std::cerr << " |permutations for the SWI-LS problem. In this problem, permutations can|" << std::endl;
  std::cerr << " |be sorted by a sequence composed by any type of inversions. The weight|" << std::endl;
  std::cerr << " |of the inversions are given by k + 1, where k is given by the absolute|" << std::endl;
  std::cerr << " |difference of the slice numbers of the inversion extremities.         |" << std::endl;
  std::cerr << " ------------------------------------------------------------------------" << std::endl << std::endl;

  std::cerr << " -------------------------------------------------------------------------" << std::endl;
  std::cerr << " |WARNING: This program may require huge amount of memory. For n=12, it  |" << std::endl;
  std::cerr << " |requires approximately 40GB of RAM.                                    |" << std::endl;
  std::cerr << " -------------------------------------------------------------------------" << std::endl << std::endl;

  exit(EXIT_FAILURE);
}

// Verifies the list of arguments
Parameters processArguments(int argc, char* argv[]) {

  if (argc != 4) printUsage();

  Parameters toReturn;
  toReturn.n = 0;
  toReturn.binary = true;
  toReturn.file = "data.out";

  bool error = false;

  try {
    toReturn.n = std::stoi(argv[1]);
    error = toReturn.n < 1 || toReturn.n > N_MAX;
  } catch (const std::exception& ia) {
    error = true;
  }
  if (error) {
    std::cerr << std::endl << "ERROR!!! Invalid permutation size.";
    printUsage();
  }

  toReturn.binary = std::string(argv[2]).compare("1") == 0;

  toReturn.file = std::string(argv[3]);

  return toReturn;
}

// Does the real job.
void process(const Parameters parameters) {

  // Auxiliary variables
  inversion_list_it it;
  int currentDistance;
  int newDistance;
  int oldDistance;
  int minDistance;


  // Auxiliary permutations
  permutation_int intPi;
  permutation_int intSigma;

  // Create two permutation vectors of size n
  permutation_vector vectorPi    = permutation_vector(parameters.n);
  permutation_vector vectorSigma = permutation_vector(parameters.n);

  // Fill pi with the identity permutation
  identityPermutation(parameters.n, vectorPi);

  // Get the identity permutation on int format
  intPi = vector_to_int(parameters.n, vectorPi);

  // Get the list of inversions considering the given problem:
  inversion_list list = getPossibleInversions(SWI_LS, parameters.n, true);


  // Time to run dijkstra
  std::unordered_set<permutation_int> visited;

  std::map<permutation_int,int> distances = std::map<permutation_int,int>();
  std::map<permutation_int,int>::iterator map_it;

  std::queue<permutation_int> queue = std::queue<permutation_int>();

  distances[intPi] = 0;
  queue.push(intPi);

  int buffer_size = 0;
  int buffer_index = 0;
  __uint16_t* buffer16 = NULL;
  __uint32_t* buffer32 = NULL;
  __uint64_t* buffer64 = NULL;

  std::ofstream outfile;

  if (parameters.binary) {
    outfile.open(parameters.file, std::ios::out | std::ios::trunc | std::ios::ate | std::ios::binary);
    if (parameters.n < 5) {
      buffer16 = new __uint16_t[BUFFER_SIZE];
      buffer_size = BUFFER_SIZE * sizeof(__uint16_t);
    } else if (parameters.n < 9) {
      buffer32 = new __uint32_t[BUFFER_SIZE];
      buffer_size = BUFFER_SIZE * sizeof(__uint32_t);
    } else {
      buffer64 = new __uint64_t[BUFFER_SIZE];
      buffer_size = BUFFER_SIZE * sizeof(__uint64_t);
    }
  } else {
    outfile.open(parameters.file, std::ios::out | std::ios::trunc | std::ios::ate);
  }

  while (queue.size() > 0) {

    // Pop the first element of the queue
    intPi = queue.front();
    queue.pop();

    // Get the distance and remove it from the map
    map_it = distances.find(intPi);
    currentDistance = (*map_it).second;
    distances.erase(map_it);

    // Transform the int into a vector
    int_to_vector(parameters.n, intPi, vectorPi);

    // Print the permutation and the distance
    if (parameters.binary) {
      if (parameters.n < 5) {
	buffer16[buffer_index++] = intPi;
	buffer16[buffer_index++] = currentDistance;
	if (buffer_index == BUFFER_SIZE) {
	  outfile.write(reinterpret_cast<const char *>(buffer16), buffer_size);
	  buffer_index = 0;
	}
      } else if (parameters.n < 9) {
	buffer32[buffer_index++] = intPi;
	buffer32[buffer_index++] = currentDistance;
	if (buffer_index == BUFFER_SIZE) {
	  outfile.write(reinterpret_cast<const char *>(buffer32), buffer_size);
	  buffer_index = 0;
	}
      } else {
	buffer64[buffer_index++] = intPi;
	buffer64[buffer_index++] = currentDistance;
	if (buffer_index == BUFFER_SIZE) {
	  outfile.write(reinterpret_cast<const char *>(buffer64), buffer_size);
	  buffer_index = 0;
	}
      }
    } else {
      print(parameters.n, vectorPi, outfile);
      outfile << " " << currentDistance << std::endl;
    }

    // Insert the permutation into the visited set
    visited.insert(intPi);

    // Try all inversions over the permutation
    for (it = list.begin(); it != list.end(); ++it) {
      inversion_info info = *it;
      applyInversion(info.i, info.j, vectorPi, vectorSigma);
      intSigma = vector_to_int(parameters.n, vectorSigma);

      if (visited.find(intSigma) == visited.end()) {
	// This permutation is not marked as visited
	newDistance = currentDistance + info.w;
	// Verify if it is in the map
	map_it = distances.find(intSigma);
	if (map_it == distances.end()) {
	  // First time this permutation appears
	  distances[intSigma] = newDistance;
	} else {
	  // We compare the old distance
	  oldDistance = (*map_it).second;
	  if (oldDistance > newDistance)
	    distances[intSigma] = newDistance;
	}
      }
    }

    // The queue is empty, look for the next group of permutations
    if (queue.size() == 0) {
      minDistance = INT_MAX;
      for (map_it = distances.begin(); map_it != distances.end(); ++map_it) {
	if (minDistance > (*map_it).second) {
	  // Empty the queue
	  std::queue<permutation_int> empty;
	  std::swap(queue, empty);
	  // Add the permutation and updated the distance
	  queue.push((*map_it).first);
	  minDistance = (*map_it).second;
	} else if (minDistance == (*map_it).second) {
	  // Just add the element
	  queue.push((*map_it).first);
	}
      }
    }

  }

  if (parameters.binary && buffer_index > 0) {
    if (parameters.n < 5) {
      outfile.write(reinterpret_cast<const char *>(buffer16), buffer_index * sizeof(__uint16_t));
    } else if (parameters.n < 9) {
      outfile.write(reinterpret_cast<const char *>(buffer32), buffer_index * sizeof(__uint32_t));
    } else {
      outfile.write(reinterpret_cast<const char *>(buffer64), buffer_index * sizeof(__uint64_t));
    }
  }

  outfile.flush();
  outfile.close();

  if (buffer16) delete buffer16;
  if (buffer32) delete buffer32;
  if (buffer64) delete buffer64;

}

// Main program
int main (int argc, char* argv[]) {
  process(processArguments(argc, argv));
  return 0;
}
