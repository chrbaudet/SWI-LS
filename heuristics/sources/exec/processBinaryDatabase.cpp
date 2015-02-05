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

#include <set>
#include <fstream>
#include <sys/stat.h>

#include <problems/problems.hpp>
#include <heuristics/heuristics.hpp>
#include <permutation/permutation.hpp>

#define READ_BUFFER_LENGTH 64000

#define WRITE_BUFFER_LENGTH 64000

bool debug = false;

/* ************************************************************************** */
// Struct to receive the command line parameters
struct Parameters {
  // Database file
  std::string file;
  // Signed/Unsigned permutation
  bool sign;
  // Permutation size
  integer n;
  // Output file
  std::string outfile;
};
/* ************************************************************************** */

/* ************************************************************************** */
// Prints program usage
void printUsage() {

  std::cerr << std::endl << "Usage: processBinaryDatabase <i> <n> <s> <o>" << std::endl << std::endl;

  std::cerr << "  <i>\tDatabase input file (binary format)." << std::endl;
  std::cerr << "  <n>\tPermutation size." << std::endl;
  std::cerr << "  <s>\t0 = unsigned permutations or 1 = signed permutations." << std::endl;
  std::cerr << "  <o>\tOutput file (also in binary format)." << std::endl << std::endl;

  std::cerr << " -------------------------------------------------------------------" << std::endl;
  std::cerr << " |This program processes binary database files which contains all  |" << std::endl;
  std::cerr << " |possible permutations and their optimal solutions considering the|" << std::endl;
  std::cerr << " |problem SWI-LS. In this problem, permutations can be sorted by a |" << std::endl;
  std::cerr << " |sequence composed by any type of inversions. The weight of the   |" << std::endl;
  std::cerr << " |inversions are given by k + 1, where k is given by the absolute  |" << std::endl;
  std::cerr << " |difference of the slice numbers of the inversion extremities.    |" << std::endl;
  std::cerr << " -------------------------------------------------------------------" << std::endl << std::endl;

  std::cerr << " -------------------------------------------------------------------" << std::endl;
  std::cerr << " |Each permutation is processed by the following heuristics:       |" << std::endl;
  std::cerr << " |  1 - LR                                                         |" << std::endl;
  std::cerr << " |  2 - SMP                                                        |" << std::endl;
  std::cerr << " |  3 - NB+LR                                                      |" << std::endl;
  std::cerr << " |  4 - NB+STRIP                                                   |" << std::endl;
  std::cerr << " |  5 - NB+BESTSTRIP                                               |" << std::endl;
  std::cerr << " |  6 - NB+LRSTRIP                                                 |" << std::endl;
  std::cerr << " |  7 - NB+SMP                                                     |" << std::endl;
  std::cerr << " |                                                                 |" << std::endl;
  std::cerr << " |The output file lists the optimum value and the result of each   |" << std::endl;
  std::cerr << " |heuristic.                                                       |" << std::endl;
  std::cerr << " -------------------------------------------------------------------" << std::endl << std::endl;

  exit(EXIT_FAILURE);

}
/* ************************************************************************** */

/* ************************************************************************** */
// Verifies the list of arguments
Parameters processArguments(int argc, char* argv[]) {

  if (argc != 5) printUsage();

  bool error = false;

  Parameters toReturn;
  toReturn.file = "";
  toReturn.n = 0;
  toReturn.sign = true;
  toReturn.outfile = "";

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

  // Output file
  toReturn.outfile = std::string(argv[4]);

  return toReturn;
}
/* ************************************************************************** */

/* ************************************************************************** */
void processPermutation(permutation_int intPi, Parameters parameters,
			Problem problem, integer* buffer, __uint64_t &buffer_index) {

  Permutation pi = Permutation(intPi, parameters.n, parameters.sign);

  if (debug) std::cout << pi;

  for (integer h = 1; h < 8; ++h) {
    buffer[buffer_index] = Heuristics::sort(intPi, parameters.n,
					    parameters.sign,
					    problem, h);
    if (debug) std::cout << "\t" << buffer[buffer_index];
    buffer_index++;
  }

}
/* ************************************************************************** */

/* ************************************************************************** */
// Do the real job
void process(const Parameters parameters) {

  permutation_int intPi;

  integer nHeuristics = 7;
  __uint64_t write_buffer_index = 0;
  __uint64_t write_buffer_length = (nHeuristics + 1) * WRITE_BUFFER_LENGTH;
  __uint64_t write_buffer_size = write_buffer_length * sizeof(integer);
  integer* write_buffer = new integer[write_buffer_length];

  __uint64_t nread = 0;
  integer bufferId = 0;
  __uint64_t  read_buffer_size = 0;
  __uint16_t* read_buffer_16 = NULL;
  __uint32_t* read_buffer_32 = NULL;
  __uint64_t* read_buffer_64 = NULL;

  if ((parameters.sign && parameters.n < 4) || (!parameters.sign && parameters.n < 5)) {
    read_buffer_16 = new __uint16_t[READ_BUFFER_LENGTH];
    read_buffer_size = READ_BUFFER_LENGTH * sizeof(__uint16_t);
    bufferId = 1;
  } else if ((parameters.sign && parameters.n < 7) || (!parameters.sign && parameters.n < 9)) {
    read_buffer_32 = new __uint32_t[READ_BUFFER_LENGTH];
    read_buffer_size = READ_BUFFER_LENGTH * sizeof(__uint32_t);
    bufferId = 2;
  } else {
    read_buffer_64 = new __uint64_t[READ_BUFFER_LENGTH];
    read_buffer_size = READ_BUFFER_LENGTH * sizeof(__uint64_t);
    bufferId = 3;
  }

  Problem problem = Problem(SWI_LS, parameters.n, parameters.sign);

  std::ofstream outfile;
  outfile.open(parameters.outfile, std::ios::out | std::ios::trunc | std::ios::ate | std::ios::binary);

  std::ifstream infile;
  infile.open(parameters.file, std::ios::in | std::ios::ate | std::ios::binary);
  infile.seekg (0, std::ios::beg);
  while (!infile.eof()) {

    switch(bufferId) {
    case 1:
      infile.read(reinterpret_cast<char *>(read_buffer_16), read_buffer_size);
      nread = infile.gcount() / sizeof(__uint16_t);
      for (__uint32_t i = 0; i < nread; i+=2) {
	intPi = read_buffer_16[i];
	write_buffer[write_buffer_index++] = read_buffer_16[i + 1];
	processPermutation(intPi, parameters, problem, write_buffer, write_buffer_index);
	if (debug) std::cout << "\t" << read_buffer_16[i + 1] << std::endl;
      }
      break;
    case 2:
      infile.read(reinterpret_cast<char *>(read_buffer_32), read_buffer_size);
      nread = infile.gcount() / sizeof(__uint32_t);
      for (__uint32_t i = 0; i < nread; i+=2) {
	intPi = read_buffer_32[i];
	write_buffer[write_buffer_index++] = read_buffer_32[i + 1];
	processPermutation(intPi, parameters, problem, write_buffer, write_buffer_index);
	if (debug) std::cout << "\t" << read_buffer_32[i + 1] << std::endl;
      }
      break;
    case 3:
      infile.read(reinterpret_cast<char *>(read_buffer_64), read_buffer_size);
      nread = infile.gcount() / sizeof(__uint64_t);
      for (__uint32_t i = 0; i < nread; i+=2) {
	intPi = read_buffer_64[i];
	write_buffer[write_buffer_index++] = read_buffer_64[i + 1];
	processPermutation(intPi, parameters, problem, write_buffer, write_buffer_index);
	if (debug) std::cout << "\t" << read_buffer_64[i + 1] << std::endl;
      }
      break;
    }
    if (write_buffer_index == write_buffer_length) {
      outfile.write(reinterpret_cast<const char *>(write_buffer), write_buffer_size);
      write_buffer_index = 0;
    }
  }
  infile.close();

  if (write_buffer_index != 0) {
    outfile.write(reinterpret_cast<const char *>(write_buffer), write_buffer_index * sizeof(integer));
  }

  outfile.close();

  if (read_buffer_16) delete read_buffer_16;
  if (read_buffer_32) delete read_buffer_32;
  if (read_buffer_64) delete read_buffer_64;
  delete write_buffer;
}
/* ************************************************************************** */

/* ************************************************************************** */
// Main program
int main (int argc, char* argv[]) {
  process(processArguments(argc, argv));
  return 0;
}
/* ************************************************************************** */
