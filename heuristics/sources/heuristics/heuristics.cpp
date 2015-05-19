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

#include <vector>
#include <climits>

#include <heuristics/heuristics.hpp>

void defineRightAndLeft(Permutation &pi, integer &right, integer &left) {
  integer n = pi.size();
  // Update right
  right = 1;
  while (right <= n) {
    if (pi.element_at(right) != right) break;
    ++right;
  }
  //Update left
  left = n;
  while (left > 1) {
    if (pi.element_at(left) != left) break;
    --left;
  }
}

void defineStrips(Permutation &pi, integer &right, integer &left, std::vector<integer> &strips) {
  defineRightAndLeft(pi, right, left);
  strips.push_back(right);
  integer index = right;
  while (index < left) {
    if (pi.element_at(index + 1) - pi.element_at(index) != 1) {
      strips.push_back(index + 1);
    }
    ++index;
  }
  strips.push_back(left + 1);
}

// Adjusts the slices of the permutation
// (auxiliary function for heuristics SMP and NB+SMP)
Inversion adjustSlices(Permutation &pi, const Problem &problem) {
  integer n = pi.size();
  integer i = 1;
  integer j = n;
  // First, symmetric inversions
  while (i <= j) {
    integer position = pi.position(i);
    if (i != position)
      return Inversion(i, j, problem.getInversionWeight(i, j));
    i++;
    j--;
  }
  // Unitary inversions
  if (pi.isSigned())
    for (i = 1; i <= n; ++i) {
      if (pi.element_at(i) < 0)
	return Inversion(i, i, problem.getInversionWeight(i, i));
    }
  // In theory, it should never arrive here
  return Inversion();
}



// Heuristic LR ////////////////////////////////////////////////////////////////
Inversion lr(Permutation &pi, const Problem &problem) {

  integer n = pi.size();

  // Identify the first slice that is misplaced
  integer right = 1;
  integer left  = n;
  while (right < left) {
    if (pi.element_at(right) != right ||
	pi.element_at(left) != left) {
      break;
    }
    right++;
    left--;
  }

  integer costRight = 0;
  integer costLeft  = 0;

  Inversions inversionsRight = problem.inversionsToCorrectPosition(right, pi, costRight);
  Inversions inversionsLeft  = problem.inversionsToCorrectPosition(left, pi, costLeft);

  if (costRight != 0 && costLeft == 0) {
    // Left is already at the right place (move right)
    return inversionsRight[0];
  }

  if (costRight == 0 && costLeft != 0) {
    // Right is already at the right place (move left)
    return inversionsLeft[0];
  }

  if (costRight < costLeft) {
    // Move right is cheaper
    return inversionsRight[0];
  }

  if (costRight > costLeft) {
    // Move left is cheaper
    return inversionsLeft[0];
  }

  if (inversionsRight.size() <= inversionsLeft.size()) {
    // Right uses less inversions. Move right
    return inversionsRight[0];
  }

  // Left uses less inversions. Move left
  return inversionsLeft[0];
}



// Heuristic SMP ///////////////////////////////////////////////////////////////
Inversion smp(Permutation &pi, const Problem &problem) {

  integer n = pi.size();
  Inversions inversions = problem.getInversions();

  float best = 0;
  Inversion bestInversion;

  float piScore = pi.sliceMisplacedPairs();
  for (InversionsIt it = inversions.begin(); it != inversions.end(); ++it) {
    Inversion r = *it;
    Permutation sigma = Permutation(pi);
    sigma.applyInversion(r.i, r.j);
    float sigmaScore = sigma.sliceMisplacedPairs();
    float benefit = (piScore - sigmaScore) / r.w;
    if (benefit > best) {
      best = benefit;
      bestInversion = r;
    }
  }

  if (best == 0) {

    integer sliceMistake = 0;
    integer currentSlice = 0;
    integer finalSlice = 0;
    while (sliceMistake < n + 1) {
      currentSlice = pi.currentElementSlice(sliceMistake);
      finalSlice = pi.finalElementSlice(sliceMistake);
      if (currentSlice != finalSlice) break;
      sliceMistake++;
    }

    if (sliceMistake == n + 1) {
      // Every element is in the right slice.
      return adjustSlices(pi, problem);
    } else {
      // We are in the second case, we simply perform a
      // symmetric inversion in order to move the blocking
      // element to the other side. After that, we will have an
      // option.

      // Here we lie to the caller saying we have a positive
      // benefit, but it is for a good cause.
      integer i = currentSlice;
      if (finalSlice > currentSlice) i = finalSlice;

      integer j = symmetric(i, n);
      if (i > j)
	return Inversion(j, i, problem.getInversionWeight(j, i));

      return Inversion(i, j, problem.getInversionWeight(i, j));

    }

  }

  return bestInversion;
}




// Heuristic NB ////////////////////////////////////////////////////////////////
// This heuristic must be used as basis for the heuristics nb+.... (except for,
// heuristic NB+SMP)
Inversion nb(Permutation &pi, const Problem &problem) {

  float best = 0;
  Inversion bestInversion;
  Inversion zeroUnitary;

  Inversions inversions = problem.getInversions();
  float piScore = pi.numberOfBreakpoints();

  for (InversionsIt it = inversions.begin(); it != inversions.end(); ++it) {
    Inversion r = *it;
    Permutation sigma = Permutation(pi);
    sigma.applyInversion(r.i, r.j);
    float sigmaScore = sigma.numberOfBreakpoints();
    float benefit = (piScore - sigmaScore) / r.w;
    if (benefit > best) {
      best = benefit;
      bestInversion = r;
    } else if (benefit == 0 && r.i == r.j && zeroUnitary.i == 0) {
      zeroUnitary = r;
    }
  }

  if (best > 0) return bestInversion;

  if (pi.isSigned() && zeroUnitary.i != 0) return zeroUnitary;

  return Inversion();
}




// Heuristic NB+LR /////////////////////////////////////////////////////////////
Inversion nb_plus_lr(Permutation &pi, const Problem &problem) {

  Inversion nbInversion = nb(pi, problem);

  if (nbInversion.i == 0) return lr(pi, problem);

  return nbInversion;
}



// Heuristic NB+STRIP //////////////////////////////////////////////////////////
Inversion nb_plus_strip(Permutation &pi, const Problem &problem) {

  Inversion nbInversion = nb(pi, problem);

  if (nbInversion.i == 0 && problem.getId() == SWI_LS) {

    integer right = 0;
    integer left  = 0;
    defineRightAndLeft(pi, right, left);

    return Inversion(right, left, problem.getInversionWeight(right, left));
  }

  return nbInversion;
}



// Heuristic NB+BESTSTRIP //////////////////////////////////////////////////////////
Inversion nb_plus_beststrip(Permutation &pi, const Problem &problem) {

  Inversion nbInversion = nb(pi, problem);

  if (nbInversion.i == 0 && problem.getId() == SWI_LS) {

    integer right = 0;
    integer left = 0;
    std::vector<integer> strips;
    defineStrips(pi, right, left, strips);

    integer lowestWeight = SHRT_MAX;
    Inversion bestInversion;
    integer size = strips.size();
    for (integer i = 0; i < size - 1; ++i) {
      for (integer j = i + 1; j < size; ++j) {
	integer w = problem.getInversionWeight(strips[i], strips[j] - 1);
	if (lowestWeight > w) {
	  lowestWeight = w;
	  bestInversion = Inversion(strips[i], strips[j] - 1, w);
	}
      }
    }

    return bestInversion;

  }

  return nbInversion;
}



// Heuristic NB+LRSTRIP //////////////////////////////////////////////////////////
Inversion nb_plus_lrstrip(Permutation &pi, const Problem &problem) {

  Inversion nbInversion = nb(pi, problem);

  if (nbInversion.i == 0 && problem.getId() == SWI_LS) {

    integer right = 0;
    integer left = 0;
    std::vector<integer> strips;
    defineStrips(pi, right, left, strips);

    integer pos_right = pi.position(right);
    integer pos_left  = pi.position(left);

    integer size = strips.size();
    integer i_right = right;
    integer j_right = right;
    for (integer i = 1; i < size; ++i) {
      if (strips[i] > pos_right) {
	j_right = strips[i] - 1;
	break;
      }
    }

    integer i_left = left;
    integer j_left = left;
    for (integer i = 1; i < size - 1; ++i) {
      if (strips[i] > pos_left) {
	i_left = strips[i - 1];
	break;
      }
    }

    integer w_right = problem.getInversionWeight(i_right, j_right);
    integer w_left  = problem.getInversionWeight(i_left, j_left);

    if (w_right <= w_left)
      return Inversion(i_right, j_right, w_right);

    return Inversion(i_left, j_left, w_left);

  }

  return nbInversion;
}



// Heuristic NB+SMP ////////////////////////////////////////////////////////////
Inversion nb_plus_smp(Permutation &pi, const Problem &problem) {

  integer n = pi.size();
  float n_sq = n * n;

  float best = 0;
  Inversion bestInversion;

  Inversions inversions;
  float piScore;

  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////  SIGNED SECTION ////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  if (pi.isSigned()) {
    inversions = problem.getInversions();
    piScore = pi.numberOfBreakpoints() + ((float)pi.sliceMisplacedPairs() / n_sq);

    for (InversionsIt it = inversions.begin(); it != inversions.end(); ++it) {
      Inversion r = *it;
      Permutation sigma = Permutation(pi);
      sigma.applyInversion(r.i, r.j);
      float sigmaScore = sigma.numberOfBreakpoints() + ((float)sigma.sliceMisplacedPairs() / n_sq);
      float benefit = (piScore - sigmaScore) / r.w;
      if (benefit > best) {
	best = benefit;
	bestInversion = r;
      }
    }

    if (best > 0)
      return bestInversion;
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////REMMAINING or UNSIGNED SECTION /////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  inversions = problem.getInversions();
  piScore = pi.numberOfBreakpointsUnsignedPermutation() + ((float)pi.sliceMisplacedPairs() / n_sq);

  for (InversionsIt it = inversions.begin(); it != inversions.end(); ++it) {
    Inversion r = *it;
    Permutation sigma = Permutation(pi);
    sigma.applyInversion(r.i, r.j);
    float sigmaScore = sigma.numberOfBreakpointsUnsignedPermutation() + ((float)sigma.sliceMisplacedPairs() / n_sq);
    float benefit = (piScore - sigmaScore) / r.w;
    if (benefit > best) {
      best = benefit;
      bestInversion = r;
    }
  }

  if (best > 0)
    return bestInversion;

  return nb_plus_beststrip(pi, problem);
}



// SORT ////////////////////////////////////////////////////////////////////////
integer Heuristics::sort(const permutation_int intPi, const integer n,
			 const bool sign, const Problem &problem,
			 const integer heuristic) {

  Inversion inversion;
  integer weight = 0;
  integer tries  = 0;
  integer limit  = n * LIMIT_MULTIPLIER;

  Permutation pi = Permutation(intPi, n, sign);
  while (!pi.isIdentity()) {
    switch (heuristic) {
    case LR:
      inversion = lr(pi, problem);
      break;
    case SMP:
      inversion = smp(pi, problem);
      break;
    case NB_PLUS_LR:
      inversion = nb_plus_lr(pi, problem);
      break;
    case NB_PLUS_STRIP:
      inversion = nb_plus_strip(pi, problem);
      break;
    case NB_PLUS_BESTSTRIP:
      inversion = nb_plus_beststrip(pi, problem);
      break;
    case NB_PLUS_LRSTRIP:
      inversion = nb_plus_lrstrip(pi, problem);
      break;
    case NB_PLUS_SMP:
      inversion = nb_plus_smp(pi, problem);
      break;
    default:
      inversion = Inversion();
    }
    if (inversion.i != 0) {
      pi.applyInversion(inversion.i, inversion.j);
      weight += inversion.w;
    } else {
      // No proposed inversion
      break;
    }
    ++tries;
    if (tries > limit) break; // We have a loop
  }

  if (pi.isIdentity()) return weight;

  return -1;

}

// SORT ////////////////////////////////////////////////////////////////////////
Inversions Heuristics::sort(const Permutation permutation, const Problem &problem,
			   const integer heuristic, integer &weight) {

  // Set weight to zero
  weight = 0;

  Inversion inversion;
  Inversions inversions;

  integer tries  = 0;
  integer limit  = permutation.size() * LIMIT_MULTIPLIER;

  Permutation pi = Permutation(permutation);

  while (!pi.isIdentity()) {

    switch (heuristic) {
    case LR:
      inversion = lr(pi, problem);
      break;
    case SMP:
      inversion = smp(pi, problem);
      break;
    case NB_PLUS_LR:
      inversion = nb_plus_lr(pi, problem);
      break;
    case NB_PLUS_STRIP:
      inversion = nb_plus_strip(pi, problem);
      break;
    case NB_PLUS_BESTSTRIP:
      inversion = nb_plus_beststrip(pi, problem);
      break;
    case NB_PLUS_LRSTRIP:
      inversion = nb_plus_lrstrip(pi, problem);
      break;
    case NB_PLUS_SMP:
      inversion = nb_plus_smp(pi, problem);
      break;
    default:
      inversion = Inversion();
    }
    inversions.push_back(inversion);
    if (inversion.i != 0) {
      pi.applyInversion(inversion.i, inversion.j);
      weight += inversion.w;
    } else {
      // No proposed inversion
      break;
    }
    ++tries;
    if (tries > limit) break; // We have a loop
  }

  if (!pi.isIdentity())
    weight = -1;

  return inversions;

}
