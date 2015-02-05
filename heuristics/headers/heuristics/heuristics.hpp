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
/* Heuristics to sort signed/unsigned permutations                            */
/* ************************************************************************** */

#ifndef __HEURISTICS__
#define __HEURISTICS__

#include <problems/problems.hpp>
#include <permutation/permutation.hpp>

#define LIMIT_MULTIPLIER 15

#define LR                1
#define SMP               2
#define NB_PLUS_LR        3
#define NB_PLUS_STRIP     4
#define NB_PLUS_BESTSTRIP 5
#define NB_PLUS_LRSTRIP   6
#define NB_PLUS_SMP       7

class Heuristics {
public:

  static integer sort(const permutation_int intPi, const integer n, const bool sign,
		      const Problem &problem, const integer heuristic);

  static Inversions sort(const Permutation permutation, const Problem &problem,
			 const integer heuristic, integer &weight);
};

#endif // __HEURISTICS__
