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
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
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

#define MAX_SZ 10000000

#define MAX(x, y) (((x) < (y)) ? (y) : (x))

static int
intcmp (const void *px, const void *py)
{
  /* struct timespec req; */
  /* struct timespec rem; */
  /* req.tv_sec = 0; */
  /* req.tv_nsec = 1L; */
  /* nanosleep(&req, &rem); */

  const int x = *((const int *) px);
  const int y = *((const int *) py);
  return ((x < y) ? -1 : ((x > y) ? 1 : 0));
}

clock_t
time_patience_sort (size_t sz, int arr[sz])
{
  int *result = malloc (sz * sizeof (int));
  clock_t t001 = clock ();
  patience_sort (arr, sz, sizeof (int), intcmp, result);
  clock_t t002 = clock ();
  return t002 - t001;
}

clock_t
time_qsort (size_t sz, int arr[sz])
{
  int *result = malloc (sz * sizeof (int));
  for (size_t i = 0; i != sz; i += 1)
    result[i] = arr[i];
  clock_t t001 = clock ();
  qsort (result, sz, sizeof (int), intcmp);
  clock_t t002 = clock ();
  return t002 - t001;
}

static void
time_uniform_random_array (size_t sz)
{
  printf ("Uniform random integers, size %zu\n", sz);
  int *arr = malloc (sz * sizeof (int));
  for (size_t i = 0; i < sz; i += 1)
    arr[i] = random_int (1, 1000);
  long double tpat = time_patience_sort (sz, arr);
  printf ("  patience sort : %10.6Lf\n", tpat / CLOCKS_PER_SEC);
  long double tq = time_qsort (sz, arr);
  printf ("  qsort         : %10.6Lf\n", tq / CLOCKS_PER_SEC);
}

static void
time_ascending_array (size_t sz)
{
  printf ("Ascending integers, size %zu\n", sz);
  int *arr = malloc (sz * sizeof (int));
  for (size_t i = 0; i < sz; i += 1)
    arr[i] = i;
  long double tpat = time_patience_sort (sz, arr);
  printf ("  patience sort : %10.6Lf\n", tpat / CLOCKS_PER_SEC);
  long double tq = time_qsort (sz, arr);
  printf ("  qsort         : %10.6Lf\n", tq / CLOCKS_PER_SEC);
}

static void
time_descending_array (size_t sz)
{
  printf ("Descending integers, size %zu\n", sz);
  int *arr = malloc (sz * sizeof (int));
  for (size_t i = 0; i < sz; i += 1)
    arr[i] = -i;
  long double tpat = time_patience_sort (sz, arr);
  printf ("  patience sort : %10.6Lf\n", tpat / CLOCKS_PER_SEC);
  long double tq = time_qsort (sz, arr);
  printf ("  qsort         : %10.6Lf\n", tq / CLOCKS_PER_SEC);
}

int
main (int argc, char *argv[])
{
  for (size_t sz = 0; sz <= MAX_SZ; sz = sz = MAX (1, 10 * sz))
    time_uniform_random_array (sz);
  for (size_t sz = 0; sz <= MAX_SZ; sz = sz = MAX (1, 10 * sz))
    time_ascending_array (sz);
  for (size_t sz = 0; sz <= MAX_SZ; sz = sz = MAX (1, 10 * sz))
    time_descending_array (sz);
  return 0;
}
