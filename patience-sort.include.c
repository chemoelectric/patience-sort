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

#include <stdbool.h>
#include <string.h>

#define LINK_NIL ((size_t) 0)
#define VALUE 0
#define LINK  1

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

static void
patience_sort_deal (void *base, size_t size, const compar_t *compar,
                    void *arg, size_t *num_piles,
                    size_t *piles, size_t *links,
                    size_t *last_elems, size_t *tails)
{
  /*

    I borrow, from the following paper, the trick of building on both
    sides of a pile:

      Badrish Chandramouli and Jonathan Goldstein, ‘Patience is a
        virtue: revisiting merge and sort on modern processors’,
        SIGMOD ’14: Proceedings of the 2014 ACM SIGMOD International
        Conference on Management of Data, June 2014, 731–742.
        https://doi.org/10.1145/2588555.2593662

    Dealing is done backwards through the arr array, so an array
    already sorted in the desired order will result in a single pile
    with just consing.

  */

  memset (piles, LINK_NIL, size * sizeof (size_t));
  memset (links, LINK_NIL, size * sizeof (size_t));
  memset (last_elems, LINK_NIL, size * sizeof (size_t));
  memset (tails, LINK_NIL, size * sizeof (size_t));
  size_t m = 0;

  for (size_t q = size; q != 0; q -= 1)
    {
      const size_t i = find_pile (base, size, compar, arg,
                                  m, piles, q);
      if (i == m + 1)
        {
          const size_t i = find_last_elem (base, size, compar, arg,
                                           m, last_elems, q);
          if (i == m + 1)
            {                   /* Start a new pile. */
              piles[i - 1] = q;
              last_elems[i - 1] = q;
              tails[i - 1] = q;
              m += 1;
            }
          else
            {                   /* Append to the end of a pile. */
              const size_t i0 = tails[i - 1];
              links[i0 - 1] = q;
              last_elems[i - 1] = q;
              tails[i - 1] = q;
            }
        }
      else
        {                     /* Cons onto the beginning of a pile. */
          links[q - 1] = piles[i - 1];
          piles[i - 1] = q;
        }
    }

  *num_piles = m;
}

static inline size_t
winners_get (const size_t *winners, int field, size_t i)
{
  return winners[i + i + field];
}

static inline void
winners_set (size_t *winners, int field, size_t i, size_t x)
{
  winners[i + i + field] = x;
}

static void
init_competitors (size_t total_external_nodes, size_t *winners,
                  size_t num_piles, const size_t *piles)
{
  /* The top of each pile becomes a starting competitor. The LINK
     field tells which pile a winner will have come from.  */
  for (size_t i = 0; i != num_piles; i += 1)
    {
      winners_set (winners, VALUE, total_external_nodes + i,
                   piles[i]);
      winners_set (winners, LINK, total_external_nodes + i, i + 1);
    }
}

static void
discard_top_of_each_pile (size_t num_piles, size_t *piles,
                          const size_t *links)
{
  for (size_t i = 0; i != num_piles; i += 1)
    piles[i] = links[piles[i]];
}

static inline size_t
find_opponent (size_t i)
{
  return (i ^ 1);
}

static size_t
play_game (void *base, size_t size, const compar_t *compar,
           void *arg, size_t i, size_t j,
           size_t winner_i, size_t winner_j)
{
  size_t iwinner;

  if (winner_i == LINK_NIL)
    iwinner = j;
  else if (winner_j == LINK_NIL)
    iwinner = i;
  else
    {
      const size_t i1 = winner_i - 1;
      const size_t i2 = winner_j - 1;
      const int cmp = COMPAR ((char *) base + i2 * size,
                              (char *) base + i1 * size,
                              arg);
      iwinner = (cmp < 0) ? j : i;
    }

  return iwinner;
}

static void
build_tree (void *base, size_t size, const compar_t *compar,
            void *arg, size_t total_external_nodes, size_t *winners)
{
  for (size_t istart = total_external_nodes;
       istart != 1;
       istart >>= 1)
    {
      size_t i = istart;
      bool done = false;
      while (!done)
        {
          if (istart + istart <= i)
            done = true;
          else
            {
              const size_t winner_i = winners_get (winners, VALUE, i);
              if (winner_i == LINK_NIL)
                done = true;    /* There are no more competitors. */
              else
                {
                  const size_t j = find_opponent (i);
                  const size_t winner_j =
                    winners_get (winners, VALUE, j);
                  const size_t iwinner =
                    play_game (base, size, compar, arg, i, j,
                               winner_i, winner_j);
                  const size_t i2 = (i >> 1);
                  winners_set (winners, VALUE, i2,
                               winners_get (winners, VALUE, iwinner));
                  winners_set (winners, LINK, i2,
                               winners_get (winners, LINK, iwinner));
                  if (winner_j == LINK_NIL)
                    done = true; /* There was no opponent. */
                  else
                    i += 2;
                }
            }
        }
    }
}

static void
k_way_merge (void *base, size_t nmemb, size_t size,
             const compar_t *compar, void *arg,
             size_t num_piles, size_t *piles,
             const size_t *links, size_t *winners,
             size_t *indices, size_t n_indices,
             void *elements, size_t n_elements)
{
  /*
    k-way merge by tournament tree.

    See Knuth, volume 3, and also
    https://en.wikipedia.org/w/index.php?title=K-way_merge_algorithm&oldid=1047851465#Tournament_Tree

    However, I store a winners tree instead of the recommended losers
    tree. If the tree were stored as linked nodes, it would probably
    be more efficient to store a losers tree. However, I am storing
    the tree as an array, and one can find an opponent quickly by
    simply toggling the least significant bit of a competitor's array
    index.
  */

  const size_t total_external_nodes = next_power_of_two (num_piles);
  const size_t total_nodes = (2 * total_external_nodes) - 1;
  
  /* We will ignore index 0 of the winners tree arrays. */
  const size_t winners_size = total_nodes + 1;

  memset (winners, LINK_NIL, 2 * winners_size * sizeof (size_t));
  init_competitors (total_external_nodes, winners, num_piles, piles);
  discard_top_of_each_pile (num_piles, piles, links);
  build_tree (base, size, compar, arg, total_external_nodes, winners);

}
