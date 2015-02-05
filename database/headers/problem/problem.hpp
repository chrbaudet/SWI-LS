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

#ifndef __PROBLEM_INVERSIONS_AND_WEIGHTS__
#define __PROBLEM_INVERSIONS_AND_WEIGHTS__

#include <vector>
#include <iostream>

#define SWI_LS 1

// Type used to represent positions on a permutation.
typedef __int16_t  position;

// Type used to represent a inversion weight.
typedef __int16_t  weight;

// Class inversion_info: It keeps the positions of the extremities of the
// inversion interval and its associated weight.
class inversion_info {
public:
  position i;
  position j;
  weight w;
  inversion_info(position I, position J, weight W) : i(I), j(J), w(W) {}
};

// Type used to represent a list of inversions.
typedef std::vector<inversion_info> inversion_list;
// Type (iterator for the list of inversions).
typedef std::vector<inversion_info>::iterator inversion_list_it;


// Return a vector of inversions accordingly with the given problem.
inversion_list getPossibleInversions(int problem, int n, bool signedPermutation);

#endif // __PROBLEM_INVERSIONS_AND_WEIGHTS__
