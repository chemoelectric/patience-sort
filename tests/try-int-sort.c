/*
  Copyright © 2022 Barry Schwartz

  This program is free software: you can redistribute it and/or
  modify it under the terms of the GNU General Public License, as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received copies of the GNU General Public License
  along with this program. If not, see
  <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <patience-sort.h>

/* (*------------------------------------------------------------------*) */
/* (* A simple linear congruential generator.                          *) */

/* (* The multiplier lcg_a comes from Steele, Guy; Vigna, Sebastiano (28 */
/*    September 2021). "Computationally easy, spectrally good multipliers */
/*    for congruential pseudorandom number generators". */
/*    arXiv:2001.05304v3 [cs.DS] *) */
/* macdef lcg_a = $UN.cast{uint64} 0xf1357aea2e62a9c5LLU */

/* (* lcg_c must be odd. *) */
/* macdef lcg_c = $UN.cast{uint64} 0xbaceba11beefbeadLLU */

/* var seed : uint64 = $UN.cast 0 */
/* val p_seed = addr@ seed */

/* fn */
/* random_double () :<!wrt> double = */
/*   let */
/*     val (pf, fpf | p_seed) = $UN.ptr0_vtake{uint64} p_seed */
/*     val old_seed = ptr_get<uint64> (pf | p_seed) */

/*     (* IEEE "binary64" or "double" has 52 bits of precision. We will */
/*        take the high 48 bits of the seed and divide it by 2**48, to */
/*        get a number 0.0 <= randnum < 1.0 *) */
/*     val high_48_bits = $UN.cast{double} (old_seed >> 16) */
/*     val divisor = $UN.cast{double} (1LLU << 48) */
/*     val randnum = high_48_bits / divisor */

/*     (* The following operation is modulo 2**64, by virtue of standard */
/*        C behavior for uint64_t. *) */
/*     val new_seed = (lcg_a * old_seed) + lcg_c */

/*     val () = ptr_set<uint64> (pf | p_seed, new_seed) */
/*     prval () = fpf pf */
/*   in */
/*     randnum */
/*   end */

/* fn */
/* random_int (m : int, n : int) :<!wrt> int = */
/*   m + $UN.cast{int} (random_double () * (n - m + 1)) */

int
main (int argc, char *argv[])
{
  return 0;
}
