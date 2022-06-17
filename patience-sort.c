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

#include <patience-sort.h>

typedef int compar_t (const void *, const void *);
#define COMPAR(x, y, arg) compar ((x), (y))

#include "patience-sort.include.c"

void
patience_sort_indices (const void *base, size_t nmemb, size_t size,
                       int (*compar) (const void *,
                                      const void *),
                       size_t *result)
{
  sort_out_of_place (base, nmemb, size, compar, NULL, result, NULL);
}

void
patience_sort (const void *base, size_t nmemb, size_t size,
               int (*compar) (const void *, const void *),
               void *result)
{
  sort_out_of_place (base, nmemb, size, compar, NULL, NULL, result);
}

void
patience_sort_in_place (void *base, size_t nmemb, size_t size,
                        int (*compar) (const void *, const void *))
{
  sort_in_place (base, nmemb, size, compar, NULL);
}
