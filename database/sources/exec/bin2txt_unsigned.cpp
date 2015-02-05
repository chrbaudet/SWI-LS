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
/* Converts a binary database to a text database (unsigned)                   */
/******************************************************************************/

#include <climits>
#include <fstream>
#include <iostream>

#include <linear/unsigned.hpp>

#define BUFFER_SIZE 64000

struct Parameters {
  element n;
  std::string file;
};

// Prints program usage.
void printUsage() {

  std::cerr << std::endl << "Usage: unsigned_bin2txt <n> <i>" << std::endl << std::endl;
  std::cerr << "  <n>\tPermutation size in the interval [1," << N_MAX << "]" << std::endl;
  std::cerr << "  <i>\tInput file name" << std::endl << std::endl;

  std::cerr << " -----------------------------------------------------------------------" << std::endl;
  std::cerr << " |This program converts a binary database of unsigned permutations in a|" << std::endl;
  std::cerr << " |text database.                                                       |" << std::endl;
  std::cerr << " -----------------------------------------------------------------------" << std::endl << std::endl;

  exit(EXIT_FAILURE);
}

// Verifies the list of arguments.
Parameters processArguments(int argc, char* argv[]) {

  if (argc != 3) printUsage();

  Parameters toReturn;
  toReturn.n = 0;
  toReturn.file = "data.in";

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

  toReturn.file = std::string(argv[2]);

  return toReturn;
}

// Does the real job
void process(const Parameters parameters) {

  // Auxiliary permutation
  permutation_vector vectorPi = permutation_vector(parameters.n);

  int buffer_size = 0;
  __uint16_t* buffer16 = NULL;
  __uint32_t* buffer32 = NULL;
  __uint64_t* buffer64 = NULL;

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

  std::ifstream infile;
  infile.open(parameters.file, std::ios::in | std::ios::ate | std::ios::binary);
  infile.seekg (0, std::ios::beg);
  while (!infile.eof()) {
    if (parameters.n < 5) {
      infile.read(reinterpret_cast<char *>(buffer16), buffer_size);
      int n = infile.gcount() / sizeof(__uint16_t);
      for (int i = 0; i < n; i+=2) {
	int_to_vector(parameters.n, buffer16[i], vectorPi);
	print(parameters.n, vectorPi, std::cout);
	std::cout << " " << buffer16[i + 1] << std::endl;
      }
    } else if (parameters.n < 9) {
      infile.read(reinterpret_cast<char *>(buffer32), buffer_size);
      int n = infile.gcount() / sizeof(__uint32_t);
      for (int i = 0; i < n; i+=2) {
	int_to_vector(parameters.n, buffer32[i], vectorPi);
	print(parameters.n, vectorPi, std::cout);
	std::cout << " " << buffer32[i + 1] << std::endl;
      }
    } else {
      infile.read(reinterpret_cast<char *>(buffer64), buffer_size);
      int n = infile.gcount() / sizeof(__uint64_t);
      for (int i = 0; i < n; i+=2) {
	int_to_vector(parameters.n, buffer64[i], vectorPi);
	print(parameters.n, vectorPi, std::cout);
	std::cout << " " << buffer64[i + 1] << std::endl;
      }
    }
  }
  infile.close();

  if (buffer16) delete buffer16;
  if (buffer32) delete buffer32;
  if (buffer64) delete buffer64;

}

// Main program
int main (int argc, char* argv[]) {
  process(processArguments(argc, argv));
  return 0;
}
