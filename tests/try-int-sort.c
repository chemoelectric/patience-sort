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

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <patience-sort.h>

/*------------------------------------------------------------------*/
/* A simple linear congruential generator.                          */

/* The multiplier LCG_A comes from Steele, Guy; Vigna, Sebastiano (28
   September 2021). "Computationally easy, spectrally good multipliers
   for congruential pseudorandom number generators".
   arXiv:2001.05304v3 [cs.DS] */
#define LCG_A UINT64_C(0xf1357aea2e62a9c5)

/* LCG_C must be odd. */
#define LCG_C UINT64_C(0xbaceba11beefbead)

uint64_t seed = 0;

static double
random_double ()
{
  /* IEEE "binary64" or "double" has 52 bits of precision. We will
     take the high 48 bits of the seed and divide it by 2**48, to get
     a number 0.0 <= randnum < 1.0 */
  const double high_48_bits = (double) (seed >> 16);
  const double divisor = (double) (UINT64_C(1) << 48);
  const double randnum = high_48_bits / divisor;

  /* The following operation is modulo 2**64, by virtue of standard C
     behavior for uint64_t. */
  seed = (LCG_A * seed) + LCG_C;

  return randnum;
}

static int
random_int (int m, int n)
{
  return m + (int) (random_double () * (n - m + 1));
}

/*------------------------------------------------------------------*/

#define MAX(x, y) (((x) < (y)) ? (y) : (x))

static int
intcmp (const void *px, const void *py)
{
  const int x = *((const int *) px);
  const int y = *((const int *) py);
  return ((x < y) ? -1 : ((x > y) ? 1 : 0));
}

static void
test_random_arrays ()
{
  for (size_t sz = 0; sz <= 1000000; sz = MAX (1, 10 * sz))
    {
      int *p1 = malloc (sz * sizeof (int));
      int *p2 = malloc (sz * sizeof (int));
      int *p3 = malloc (sz * sizeof (int));

      for (size_t i = 0; i < sz; i += 1)
        p1[i] = random_int (1, 1000);

      for (size_t i = 0; i < sz; i += 1)
        p2[i] = p1[i];
      qsort (p2, sz, sizeof (int), intcmp);

      patience_sort (p1, sz, sizeof (int), intcmp, p3);

      for (size_t i = 0; i < sz; i += 1)
        assert (p2[i] == p3[i]);

      free (p1);
      free (p2);
      free (p3);
    }
}

static void
test_random_arrays_in_place ()
{
  for (size_t sz = 0; sz <= 1000000; sz = MAX (1, 10 * sz))
    {
      int *p1 = malloc (sz * sizeof (int));
      int *p2 = malloc (sz * sizeof (int));

      for (size_t i = 0; i < sz; i += 1)
        p1[i] = random_int (1, 1000);

      for (size_t i = 0; i < sz; i += 1)
        p2[i] = p1[i];
      qsort (p2, sz, sizeof (int), intcmp);

      patience_sort_in_place (p1, sz, sizeof (int), intcmp);

      for (size_t i = 0; i < sz; i += 1)
        assert (p2[i] == p1[i]);

      free (p1);
      free (p2);
    }
}

static void
test_random_arrays_indices ()
{
  for (size_t sz = 0; sz <= 1000000; sz = MAX (1, 10 * sz))
    {
      int *p1 = malloc (sz * sizeof (int));
      int *p2 = malloc (sz * sizeof (int));
      size_t *p3 = malloc (sz * sizeof (size_t));

      for (size_t i = 0; i < sz; i += 1)
        p1[i] = random_int (1, 1000);

      for (size_t i = 0; i < sz; i += 1)
        p2[i] = p1[i];
      qsort (p2, sz, sizeof (int), intcmp);

      patience_sort_indices (p1, sz, sizeof (int), intcmp, p3);

      for (size_t i = 0; i < sz; i += 1)
        assert (p2[i] == p1[p3[i]]);

      free (p1);
      free (p2);
      free (p3);
    }
}

int
main (int argc, char *argv[])
{
  test_random_arrays ();
  test_random_arrays_in_place ();
  test_random_arrays_indices ();
  return 0;
}
