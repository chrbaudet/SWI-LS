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

#include <problem/problem.hpp>

// Generates the list of inversions that are compatible with the
// SWI-LS problem.
// In this problem, all inversions are allowed and their weight is
// given by k + 1, where k is the difference between the slice of
// the elements on each extremity of the inversion.
inversion_list swi_ls(int n, bool signedPermutation) {

  position delta = 1;
  if (signedPermutation) delta = 0;

  inversion_list list = inversion_list();

  for (position i = 0; i < n; ++i)
    for (position j = i + delta; j < n; ++j) {
      int si = std::min(i + 1, n - i);
      int sj = std::min(j + 1, n - j);
      list.push_back(inversion_info(i, j, abs(si - sj) + 1));
    }

  return list;
}

inversion_list getPossibleInversions(int problem, int n, bool signedPermutation) {
  switch(problem) {
  case SWI_LS:
    return swi_ls(n, signedPermutation);
  default:
    std::cerr << "ABORT!!! Unknown problem." << std::endl;
    exit(EXIT_FAILURE);
  }
}
