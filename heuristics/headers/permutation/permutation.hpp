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
/* Class permutation permutation                                              */
/* ************************************************************************** */

#ifndef __PERMUTATION__
#define __PERMUTATION__

#include <vector>
#include <iostream>
#include <cinttypes>

////////////////////////////////////////////////////////////////////////////////
// For unsigned permutations
// Maximum size of an unsigned permutation
#define U_N_MAX 16
// Number of bit which are necessary to keep an unsigned integer of the
// permutation inside of an 64 bits variable.
#define U_N_BITS 4
// Auxiliar constants which are used to access the desired information
#define U_NUMBERS 15
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// For signed permutations
// Maximum size of a signed permutation
#define S_N_MAX 12
// Number of bit which are necessary to keep a signed integer of the
// permutation inside of an 64 bits variable.
#define S_N_BITS 5
// Auxiliar constants which are used to access the desired information
#define S_SIGN           16
#define S_NUMBERS        15
#define S_NUMBERSANDSIGN 31
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Type which defines an integer
typedef __int16_t integer;
// Type used to represent a permutation using only 64 bits.
typedef __uint64_t permutation_int;
// Type used to represent a permutation using a vector of integer numbers
typedef std::vector<integer> permutation_vector;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Returns the slice of the given position
static inline integer slice(integer pos, integer n) {
  return std::min((int)pos, (n - pos) + 1);
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Returns the symmetric position of the give position
// (other position in the same slice)
static inline integer symmetric(integer pos, integer n) {
  return n - pos + 1;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
class Permutation {

private:

  // Permutation size
  integer n;

  // Permutation
  permutation_vector permutation;

  // Inverse permutation
  permutation_vector inverse;

  // Flag: signed/unsigned permutation
  bool sign;

public:

  // Empty Constructor
  Permutation();

  // Constructor
  Permutation(Permutation const &other);

  // Constructor
  Permutation(const permutation_int intPi, const integer n, const bool sign);

  // Constructor
  Permutation(const permutation_vector vector, const bool sign);

  // Returns the position of the element e
  integer position(const integer e) const;

  // Returns the element that is at the position pos
  integer element_at(const integer pos) const;

  // Returns true if this permutation is the identity permutation
  bool isIdentity() const;

  // Returns true if this permutation is a signed permutation
  bool isSigned() const { return sign; }

  // Returns the number of breakpoints of the permutation
  integer size() const { return n; }

  // Returns the number of breakpoints of the permutation
  integer numberOfBreakpoints() const;

  // Returns the number of breakpoints of the permutation 
  // ignoring sign information
  integer numberOfBreakpointsUnsignedPermutation() const;

  // Returns the number of breakpoints of the permutation
  integer sliceMisplacedPairs() const;

  // Returns the final slice of the given element
  integer finalElementSlice(integer element) const {
    return slice(abs(element), n);
  }

  // Returns the current slice of the given element
  integer currentElementSlice(integer element) const {
    return slice(position(element), n);
  }

  // Returns an string representation of this permutation
  std::string toString() const;

  // Operator <<
  friend std::ostream& operator<<(std::ostream&, const Permutation&);

  // Apply a inversion to the permutation
  void applyInversion(integer i, integer j);

};

// Operator <<
std::ostream& operator<<(std::ostream&, const Permutation&);
////////////////////////////////////////////////////////////////////////////////

#endif // __PERMUTATION__
