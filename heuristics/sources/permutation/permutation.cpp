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
/* Class permutation                                                          */
/* ************************************************************************** */

#include <unordered_set>

#include <permutation/permutation.hpp>

Permutation::Permutation() {
  n = 0;
  sign = false;
}

Permutation::Permutation(Permutation const &other) {
  // Set the permutation size
  n = other.size();

  // Set the flag of signed/unsigned permutation
  sign = other.isSigned();

  // Create the vectors (permutation and inverse)
  permutation        = permutation_vector(n + 2);
  permutation[0]     = 0;
  permutation[n + 1] = n + 1;
  inverse            = permutation_vector(n + 2);
  inverse[0]         = 0;
  inverse[n + 1]     = n + 1;

  // Copy the elements to their right position
  for (integer i = 1; i <= n; ++i) {
    integer element = other.element_at(i);
    permutation[i] = element;
    inverse[abs(element)] = i;
  }

}


Permutation::Permutation(const permutation_int intPi, const integer N, const bool S) {

  // Set the permutation size
  n = N;

  // Set the flag of signed/unsigned permutation
  sign = S;

  // Create the vectors (permutation and inverse)
  permutation        = permutation_vector(n + 2);
  permutation[0]     = 0;
  permutation[n + 1] = n + 1;
  inverse            = permutation_vector(n + 2);
  inverse[0]         = 0;
  inverse[n + 1]     = n + 1;

  if (sign) {
    permutation_int aux = intPi;
    for (integer i = n - 1; i >= 0; --i) {
      integer last = (aux & S_NUMBERS) + 1;
      integer sign = aux & S_SIGN;
      if (sign) permutation[i + 1] = -last;
      else permutation[i + 1] = last;
      inverse[last] = i + 1;
      aux = aux >> S_N_BITS;
    }
  } else {
    permutation_int aux = intPi;
    for (integer i = n - 1; i >= 0; --i) {
      integer last = (aux & U_NUMBERS) + 1;
      permutation[i + 1] = last;
      inverse[last] = i + 1;
      aux = aux >> U_N_BITS;
    }
  }

  // Validate the permutation
  std::unordered_set<integer> elements = std::unordered_set<integer>();
  for (integer i = 1; i <= n; ++i) {
    elements.insert(abs(permutation[i]));
  }

  for (integer i = 1; i <= n; ++i) {
    if (elements.find(i) == elements.end()) {
      std::cerr << std::endl << "ERROR!!! Invalid permutation!" << std::endl << std::endl;
      exit(EXIT_FAILURE);
    }
  }

}

Permutation::Permutation(const permutation_vector vector, const bool S) {

  // Set the permutation size
  n = vector.size();

  // Set the flag of signed/unsigned permutation
  sign = S;

  // Create the vectors (permutation and inverse)
  permutation        = permutation_vector(n + 2);
  permutation[0]     = 0;
  permutation[n + 1] = n + 1;
  inverse            = permutation_vector(n + 2);
  inverse[0]         = 0;
  inverse[n + 1]     = n + 1;

  integer index = 1;
  std::unordered_set<integer> elements = std::unordered_set<integer>();
  for (auto it = vector.begin(); it != vector.end(); ++it) {
    integer element = *it;
    if ((!sign && element < 0) || abs(element) > n || element == 0) {
      std::cerr << std::endl << "ERROR!!! Invalid permutation!" << std::endl << std::endl;
      exit(EXIT_FAILURE);
    }
    permutation[index] = element;
    inverse[abs(element)] = index;
    elements.insert(abs(element));
    ++index;
  }

  // Validate the permutation
  for (integer i = 1; i <= n; ++i) {
    if (elements.find(i) == elements.end()) {
      std::cerr << std::endl << "ERROR!!! Invalid permutation!" << std::endl << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

// Warning !!! For performance purposes, we are not checking the boundaries of the vector.
integer Permutation::position(const integer e) const {
  return inverse[abs(e)];
}


// Warning !!! For performance purposes, we are not checking the boundaries of the vector.
integer Permutation::element_at(const integer pos) const {
  return permutation[pos];
}


void Permutation::applyInversion(const integer i, const integer j) {
  integer aux;
  integer b = i;
  integer e = j;
  if (sign) {
    while (b <= e) {
      aux = permutation[b];
      permutation[b] = -permutation[e];
      permutation[e] = -aux;
      inverse[abs(permutation[b])] = b;
      inverse[abs(permutation[e])] = e;
      ++b;
      --e;
    }
  } else {
    while (b <= e) {
      aux = permutation[b];
      permutation[b] = permutation[e];
      permutation[e] = aux;
      inverse[permutation[b]] = b;
      inverse[permutation[e]] = e;
      ++b;
      --e;
    }
  }
}


bool Permutation::isIdentity() const {
  for (integer i = 1; i <= n; ++i) {
    if (permutation[i] != i) return false;
  }
  return true;
}


integer Permutation::numberOfBreakpoints() const {
  integer breakpoints = 0;
  integer last = n + 1;
  integer current = 0;
  if (sign) {
    for (integer i = n; i >= 0; --i) {
      current = permutation[i];
      if (last - current != 1) ++breakpoints;
      last = current;
    }
  } else {
    for (integer i = n; i >= 0; --i) {
      current = permutation[i];
      if (abs(last - current) != 1) ++breakpoints;
      last = current;
    }
  }
  return breakpoints;
}

integer Permutation::numberOfBreakpointsUnsignedPermutation() const {
  integer breakpoints = 0;
  integer last = n + 1;
  integer current = 0;
  for (integer i = n; i >= 0; --i) {
    current = abs(permutation[i]);
    if (abs(last - current) != 1) ++breakpoints;
    last = current;
  }
  return breakpoints;
}


std::string Permutation::toString() const {
  std::string toReturn = std::to_string((const long long int)permutation[1]);
  for (integer i = 2; i <= n; ++i) {
    toReturn = toReturn.append(",");
    toReturn = toReturn.append(std::to_string((const long long int)permutation[i]));
  }
  return toReturn;
}


std::ostream& operator<<(std::ostream& strm, const Permutation& object) {
  return strm << object.toString();
}


integer Permutation::sliceMisplacedPairs() const {

  integer slice_misplaced_pairs = 0;

  for (integer i = 1; i <= n; ++i) {
    integer element = abs(permutation[i]);
    integer final_slice = slice(element, n);
    integer local_slice = slice(i, n);
    integer left = 1;
    integer right = n;
    while (left < local_slice) {
      // Dealing with left
      integer left_element = abs(permutation[left]);
      integer final_left_slice = slice(left_element, n);
      if (final_left_slice > final_slice)
	slice_misplaced_pairs++;
      // Dealing with right
      integer right_element = abs(permutation[right]);
      integer final_right_slice = slice(right_element, n);
      if (final_right_slice > final_slice)
	slice_misplaced_pairs++;
      left++;
      right--;
    }
  }

  return slice_misplaced_pairs;
}
