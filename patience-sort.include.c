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
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define LINK_NIL ((size_t) 0)
#define VALUE 0
#define LINK  1

#define LEN_THRESHOLD   128
#define PILES_SIZE      128
#define LINKS_SIZE      128
#define WORKSPACE_SIZE  512

_Static_assert (PILES_SIZE == LEN_THRESHOLD, "bad constants");
_Static_assert (LINKS_SIZE == LEN_THRESHOLD, "bad constants");
_Static_assert (WORKSPACE_SIZE == 4 * LEN_THRESHOLD, "bad constants");

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
find_pile (void *base, size_t size, compar_t *compar,
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
find_last_elem (void *base, size_t size, compar_t *compar,
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
patience_sort_deal (void *base, size_t nmemb, size_t size,
                    compar_t *compar, void *arg, size_t *num_piles,
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

  memset (piles, LINK_NIL, nmemb * sizeof (size_t));
  memset (links, LINK_NIL, nmemb * sizeof (size_t));
  memset (last_elems, LINK_NIL, nmemb * sizeof (size_t));
  memset (tails, LINK_NIL, nmemb * sizeof (size_t));
  size_t m = 0;

  for (size_t q = nmemb; q != 0; q -= 1)
    {
      const size_t i = find_pile (base, size, compar, arg,
                                  m, piles, q);
      if (i == m + 1)
        {
/* FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
          const size_t i = find_last_elem (base, size, compar, arg,
                                           m, last_elems, q);
FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME */
          if (i == m + 1)
            {                   /* Start a new pile. */
              piles[m] = q;
              last_elems[m] = q;
              tails[m] = q;
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
    piles[i] = links[piles[i] - 1];
}

static inline size_t
find_opponent (size_t i)
{
  return (i ^ 1);
}

static size_t
play_game (void *base, size_t size, compar_t *compar,
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
build_tree (void *base, size_t size, compar_t *compar,
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
replay_games (void *base, size_t size, compar_t *compar,
              void *arg, size_t *winners, size_t i)
{
  while (i != 1)
    {
      const size_t j = find_opponent (i);
      const size_t iwinner =
        play_game (base, size, compar, arg, i, j,
                   winners_get (winners, VALUE, i),
                   winners_get (winners, VALUE, j));
      const size_t i2 = (i >> 1);
      winners_set (winners, VALUE, i2,
                   winners_get (winners, VALUE, iwinner));
      winners_set (winners, LINK, i2,
                   winners_get (winners, LINK, iwinner));
      i = i2;
    }
}

static void
merge (void *base, size_t nmemb, size_t size,
       compar_t *compar, void *arg,
       size_t *piles, const size_t *links,
       size_t total_nodes, size_t *winners,
       size_t *indices, void *elements)
{
  for (size_t isorted = 0; isorted != nmemb; isorted += 1)
    {
      const size_t winner = winners_get (winners, VALUE, 1);
      if (indices != NULL)
        indices[isorted] = winner - 1;
      if (elements != NULL)
        memcpy ((char *) elements + isorted * size,
                (char *) base + (winner - 1) * size,
                size);

      /* Move to the next element in the winner’s pile. */
      const size_t ilink = winners_get (winners, LINK, 1);
      const size_t inext = piles[ilink - 1];
      if (inext != LINK_NIL)
        piles[ilink - 1] = links[inext - 1];

      /* Replay games, with the new element as a competitor. */
      const size_t i = (total_nodes >> 1) + ilink;
      winners_set (winners, VALUE, i, inext);
      replay_games (base, size, compar, arg, winners, i);
    }
}

static void
k_way_merge (void *base, size_t nmemb, size_t size,
             compar_t *compar, void *arg,
             size_t num_piles, size_t *piles,
             const size_t *links, size_t *winners,
             size_t *indices, void *elements)
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
  merge (base, nmemb, size, compar, arg, piles, links,
         total_nodes, winners, indices, elements);
}

static void *
xmalloc (size_t n)
{
  void *p = malloc (n);
  if (p == NULL)
    {
      fprintf
        (stderr,
         "Memory exhausted while trying to allocate %zu bytes.\n",
         n);
      exit (1);
    }
  return p;
}

static void
sort_out_of_place (void *base, size_t nmemb, size_t size,
                   compar_t *compar, void *arg,
                   size_t *indices, void *elements)
{
  if (nmemb == 0)
    {
      /* Do nothing. */
    }
  else if (nmemb <= LEN_THRESHOLD)
    {
      /* Use stack storage. */

      size_t piles[PILES_SIZE];
      size_t links[LINKS_SIZE];
      size_t workspace[WORKSPACE_SIZE];

      size_t *const last_elems = workspace;
      size_t *const tails = workspace + nmemb;

      size_t num_piles;

      patience_sort_deal (base, nmemb, size, compar, arg,
                          &num_piles, piles, links,
                          last_elems, tails);

      size_t *const winners = workspace;

      k_way_merge (base, nmemb, size, compar, arg,
                   num_piles, piles, links, winners,
                   indices, elements);
    }
  else
    {
      /* Use malloc storage. */

      size_t *piles = xmalloc (nmemb * sizeof (size_t));
      size_t *links = xmalloc (nmemb * sizeof (size_t));
      size_t *workspace = xmalloc (2 * nmemb * sizeof (size_t));

      size_t *const last_elems = workspace;
      size_t *const tails = workspace + nmemb;

      size_t num_piles;

      patience_sort_deal (base, nmemb, size, compar, arg,
                          &num_piles, piles, links,
                          last_elems, tails);

      const size_t power = next_power_of_two (num_piles);

      if (4 * power <= 2 * nmemb)
        {
          size_t *const winners = workspace;
          k_way_merge (base, nmemb, size, compar, arg,
                       num_piles, piles, links, winners,
                       indices, elements);
          free (workspace);
        }
      else
        {
          free (workspace);
          size_t *winners = xmalloc (4 * power * sizeof (size_t));
          k_way_merge (base, nmemb, size, compar, arg,
                       num_piles, piles, links, winners,
                       indices, elements);
          free (winners);
        }

      free (piles);
      free (links);
    }
}

static void
sort (void *base, size_t nmemb, size_t size,
      compar_t *compar, void *arg,
      size_t *indices, void *elements)
{
  if (elements == base)
    {
      /* Fake an in-place sort. */
      if (nmemb * size <= LEN_THRESHOLD * sizeof (size_t))
        {
          char buffer[nmemb * size];
          sort_out_of_place (base, nmemb, size, compar, arg,
                             indices, buffer);
          memcpy (elements, buffer, nmemb * size);
        }
      else
        {
          void *buffer = xmalloc (nmemb * size);
          sort_out_of_place (base, nmemb, size, compar, arg,
                             indices, buffer);
          memcpy (elements, buffer, nmemb * size);
          free (buffer);
        }
    }
  else
    sort_out_of_place (base, nmemb, size, compar, arg,
                       indices, elements);
}
