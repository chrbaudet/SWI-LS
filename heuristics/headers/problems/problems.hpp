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
/* Generate inversions and weights for specific problems                      */
/* ************************************************************************** */

#ifndef __PROBLEMS__
#define __PROBLEMS__

#include <vector>
#include <climits>

#include <permutation/permutation.hpp>

#define SWI_LS 1

class Inversion {
public:

  // Position i
  integer i;

  // Position j
  integer j;

  // Inversion weight
  integer w;

  // Constructor
  Inversion() {i=0; j=0; w=0;}

  Inversion(integer I, integer J, integer W) : i(I), j(J), w(W) {}

  // Returns an string representation of this permutation
  std::string toString() const;

  // Operator <<
  friend std::ostream& operator<<(std::ostream&, const Inversion&);
};

// Operator <<
std::ostream& operator<<(std::ostream&, const Inversion&);


// Type used to represent a list of inversions
typedef std::vector<Inversion> Inversions;

// Type (iterator for the list of inversions)
typedef std::vector<Inversion>::iterator InversionsIt;


class Problem {

private:

  // Problem identifier
  integer id;

  // Permutation size
  integer n;

  // Signed/Unsigned flag
  bool sign;

  // List of inversions
  Inversions inversions;


  // Generates the list of inversions for the problem SWI_LS
  void swi_ls();

public:

  // Constructor
  Problem(integer ID, integer N, bool S);

  // Returns the problem id
  integer getId() const { return id; }

  // Returns the list of possible inversions (accordingly to the problem)
  Inversions getInversions() const { return inversions; }

  // Returns the weight of a given inversion (accordingly to the problem)
  integer getInversionWeight(integer i, integer j) const;

  // Returns the sequence of inversions to correct the given position
  Inversions inversionsToCorrectPosition(const integer pos, Permutation const & pi, integer &totalCost) const;

};


#endif // __PROBLEMS__
