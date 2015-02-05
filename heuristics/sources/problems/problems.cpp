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
/* Generate inversions and weights for specific problems                       */
/* ************************************************************************** */

#include <algorithm>

#include <problems/problems.hpp>

std::string Inversion::toString() const {
  return "[" + std::to_string((const long long int)i) + "," + std::to_string((const long long int)j) + "] weight=" + std::to_string((const long long int)w);
}

std::ostream& operator<<(std::ostream& strm, const Inversion& object) {
  return strm << object.toString();
}

Problem::Problem(integer ID, integer N, bool S) {
  id = ID;
  n = N;
  sign = S;
  switch(id) {
  case SWI_LS:
    swi_ls();
    break;
  default:
    std::cerr << "ERROR!!! Unknown problem identifier." << std::endl << std::endl;
    exit(EXIT_FAILURE);
  }
}

// Generates the list of inversions that are compatible with the
// SWI-LS problem.
// In this problem, all inversions are allowed and their weight is
// given by k + 1, where k is the difference between the slice of
// the elements on each extremity of the inversion.
void Problem::swi_ls() {

  integer delta = 1;
  if (sign) delta = 0;

  inversions = Inversions();
  for (integer i = 0; i < n; ++i)
    for (integer j = i + delta; j < n; ++j)
      inversions.push_back(Inversion(i + 1, j + 1, getInversionWeight(i + 1, j + 1)));

}

integer Problem::getInversionWeight(integer i, integer j) const {
  switch(id) {
  case SWI_LS:
    return abs(slice(i, n) - slice(j, n)) + 1;
  }
  std::cerr << "ERROR!!! Unknown problem identifier." << std::endl << std::endl;
  exit(EXIT_FAILURE);
}

Inversions Problem::inversionsToCorrectPosition(const integer pos, Permutation const & pi,
					      integer &totalCost) const {

  // To correct a position pos, we have to bring the element e = pos) to the right position

  // Auxiliary variables
  integer i, j;

  // Vector to keep the inversions
  Inversions toReturn;

  // Get the current position of the element e = pos
  integer current_pos = pi.position(pos);

  // Get the right value of the element (maybe it has negative sign)
  integer element = pi.element_at(current_pos);

  // Inversion weight
  integer weight;

  // Initially the total cost is zero
  totalCost = 0;

  if (current_pos == pos) {

    // The element is already in the right position
    if (pi.isSigned() && element < 0) {
      // However, an unitary inversion is needed.
      weight = getInversionWeight(pos, pos);
      toReturn.push_back(Inversion(pos, pos, weight));
      totalCost = weight;
    } // if (pi.isSigned() && element < 0) { ... }

  } else {

    // The element is somewhere else

    switch (id) {

    case SWI_LS:
      // Just send the element direct to its place.
      if (current_pos < pos) { i = current_pos; j = pos; }
      else { i = pos; j = current_pos; }
      weight = getInversionWeight(i, j);
      toReturn.push_back(Inversion(i, j, weight));
      totalCost = weight;
      if (pi.isSigned() && element > 0) {
	// An additional unitary inversion is needed.
	weight = getInversionWeight(pos, pos);
	toReturn.push_back(Inversion(pos, pos, weight));
	totalCost += weight;
      }
      break;

    default:
      std::cerr << "ERROR!!! Unknown problem identifier." << std::endl << std::endl;
      exit(EXIT_FAILURE);

    } // switch (id) { ... }

  } // if (current_pos == pos) { ... } else { ... }

  return toReturn;
}
