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

static size_t
next_power_of_two (size_t i)
{
  /* This is not a fast implementation, nor need it be. */
  size_t j = 1;
  while (j < i)
    j += j;
  return j;
}

static size_t
find_pile (void *base, size_t size, const compar_t *compar,
           void *arg, size_t num_piles, const size_t *piles,
           size_t q)
{
  /*
    ------------------------------------------------------------------

    Bottenbruch search for the *leftmost* pile whose *first* element
    is *greater* than or equal to the next value dealt by "deal".

    References:

      * H. Bottenbruch, "Structure and use of ALGOL 60", Journal of
        the ACM, Volume 9, Issue 2, April 1962, pp.161-221.
        https://doi.org/10.1145/321119.321120

        The general algorithm is described on pages 214 and 215.

      * https://en.wikipedia.org/w/index.php?title=Binary_search_algorithm&oldid=1062988272#Alternative_procedure

    ------------------------------------------------------------------
  */

  size_t retval;

  if (num_piles == 0)
    retval = 1;
  else
    {
      size_t j = 0;
      size_t k = num_piles - 1;
      while (j != k)
        {
          const size_t i1 = q - 1;
          const size_t i2 = piles[j] - 1;
          const int cmp = COMPAR ((char *) base + i2 * size,
                                  (char *) base + i1 * size,
                                  arg);
          const size_t i = j + ((k - j) >> 1);
          if (cmp < 0)
            j = i + 1;
          else
            k = i;
        }

      if (j + 1 != num_piles)
        retval = j + 1;
      else
        {
          const size_t i1 = q - 1;
          const size_t i2 = piles[j] - 1;
          const int cmp = COMPAR ((char *) base + i2 * size,
                                  (char *) base + i1 * size,
                                  arg);
          if (cmp < 0)
            retval = num_piles + 1;
          else
            retval = num_piles;
        }
    }

  return retval;
}

static size_t
find_last_elem (void *base, size_t size, const compar_t *compar,
                void *arg, size_t num_piles, const size_t *last_elems,
                size_t q)
{
  /*
    ------------------------------------------------------------------

    Bottenbruch search for the *rightmost* pile whose *last* element
    is *less* than or equal to the next value dealt by "deal".

    References:

      * H. Bottenbruch, "Structure and use of ALGOL 60", Journal of
        the ACM, Volume 9, Issue 2, April 1962, pp.161-221.
        https://doi.org/10.1145/321119.321120

        The general algorithm is described on pages 214 and 215.

      * https://en.wikipedia.org/w/index.php?title=Binary_search_algorithm&oldid=1062988272#Alternative_procedure

    ------------------------------------------------------------------
  */

  size_t retval;

  if (num_piles == 0)
    retval = 1;
  else
    {
      size_t j = 0;
      size_t k = num_piles - 1;
      while (j != k)
        {
          const size_t i1 = q - 1;
          const size_t i2 = last_elems[num_piles - 1 - j] - 1;
          const int cmp = COMPAR ((char *) base + i1 * size,
                                  (char *) base + i2 * size,
                                  arg);
          const size_t i = j + ((k - j) >> 1);
          if (cmp < 0)
            j = i + 1;
          else
            k = i;
        }

      if (j + 1 != num_piles)
        retval = num_piles - j;
      else
        {
          const size_t i1 = q - 1;
          const size_t i2 = last_elems[num_piles - 1 - j] - 1;
          const int cmp = COMPAR ((char *) base + i1 * size,
                                  (char *) base + i2 * size,
                                  arg);
          if (cmp < 0)
            retval = num_piles + 1;
          else
            retval = 1;
        }
    }

  return retval;
}
