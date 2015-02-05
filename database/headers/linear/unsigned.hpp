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
/* Operations for unsigned linear permutations                                */
/* ************************************************************************** */

#ifndef __LINEAR_UNSIGNED__
#define __LINEAR_UNSIGNED__

#include <cinttypes>
#include <iostream>
#include <vector>

// Maximum size of an unsigned permutation.
#define N_MAX 16

// Number of bit which are necessary to keep an element of the permutation
// inside of an 64 bits variable.
#define N_BITS 4

// Auxiliar constants which are used to access the desired information.
#define NUMBERS 15

// Type which defines an unsigned element.
typedef __int16_t element;

// Type used to represent a permutation using only 64 bits.
typedef __uint64_t permutation_int;

// Type used to represent a permutation using a vector of unsigned elements
typedef std::vector<element> permutation_vector;

// Converts a permutation in integer format to a permutation into vector format.
static inline void int_to_vector(const element n, const permutation_int intPi, permutation_vector &vectorPi) {
  permutation_int aux = intPi;
  for (element i = n - 1; i >= 0; --i) {
    vectorPi[i] = (aux & NUMBERS) + 1;
    aux = aux >> N_BITS;
  }
}

// Returns a permutation in integer format based on the given permutation (vector format).
static inline permutation_int vector_to_int(const element n, const permutation_vector pi) {
  permutation_int aux = 0;
  for (element i = 0; i < n; ++i) {
    aux = aux << N_BITS;
    element e = pi[i] - 1;
    aux = aux | e;
  }
  return aux;
}

// Fills the given permutation (vector format) with the identity permutation.
static void identityPermutation(const element n, permutation_vector &pi) {
  for (element i = 0; i < n; ++i) {
    pi[i] = i + 1;
  }
}

// Prints the given permutation (vector format).
static void print(const element n, const permutation_vector pi, std::ostream &ostream) {
  ostream << pi[0];
  for (element i = 1; i < n; ++i) {
    ostream << "," << pi[i];
  }
}

// Applies the given inversion into pi. The permutation sigma will receive the resulting permutation.
// IMPORTANT: To speed up, this function assumes that i <= j and that the extrimities of the
// inversion are inside of the permutation (in the interval [0, n - 1].
static void applyInversion(element i, element j, const permutation_vector pi, permutation_vector &sigma) {
  // First copy pi to sigma
  element aux;
  sigma.assign(pi.begin(), pi.end());
  // Then, apply the inversion
  while (i <= j) {
    aux = sigma[i];
    sigma[i] = sigma[j];
    sigma[j] = aux;
    ++i;
    --j;
  }
}

#endif
