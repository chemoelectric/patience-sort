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

#include <stddef.h>

/* Sorts returning indices. */
void patience_sort_indices (const void *base,
                            size_t nmemb, size_t size,
                            int (*compar) (const void *,
                                           const void *),
                            size_t *result);
void patience_sort_indices_r (const void *base,
                              size_t nmemb, size_t size,
                              int (*compar) (const void *,
                                             const void *,
                                             void *),
                              void *arg, size_t *result);

/* Sorts with out-of-place results. */
void patience_sort (const void *base,
                    size_t nmemb, size_t size,
                    int (*compar) (const void *, const void *),
                    void *result);
void patience_sort_r (const void *base,
                      size_t nmemb, size_t size,
                      int (*compar) (const void *, const void *,
                                     void *),
                      void *arg, void *result);

/* Sorts whose in-place results. It is unspecified whether the
   elements are swapped around in-place or simply computed
   out-of-place and then written into the original array. */
void patience_sort_in_place (void *base,
                             size_t nmemb, size_t size,
                             int (*compar) (const void *,
                                            const void *));
void patience_sort_in_place_r (void *base,
                               size_t nmemb, size_t size,
                               int (*compar) (const void *,
                                              const void *,
                                              void *),
                               void *arg);
