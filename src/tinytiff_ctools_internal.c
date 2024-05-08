/*
    Copyright (c) 2008-2024 Jan W. Krieger (<jan@jkrieger.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/
#include "tinytiff_ctools_internal.h"
#include <string.h>
#include <assert.h>

#ifdef __has_builtin

#  if defined(TINYTIFF_HASOBJSIZE) && __has_builtin (__builtin___memcpy_chk)
#    define tinytiff_builtin_memcpy_chk(dest, src, n) __builtin___memcpy_chk(dest, src, n, tinytiff_builtin_object_size0(dest))
#  endif

#  if defined(TINYTIFF_HASOBJSIZE) && __has_builtin (__builtin___memset_chk)
#    define tinytiff_builtin_memset_chk(dest, ch, n) __builtin___memset_chk(dest, ch, n, tinytiff_builtin_object_size0(dest))
#  endif

#endif

void TinyTIFF_memcpy_s(void *dest, unsigned long destsz, const void *src, unsigned long count) {
    if (count<=0) return;
#ifdef HAVE_MEMCPY_S
    memcpy_s(dest, destsz, src, count);
#else
#  if defined(tinytiff_builtin_memcpy_chk)
     tinytiff_builtin_memcpy_chk(dest, src, count);
#  else
    assert((dest!=NULL) && "TinyTIFF_memcpy_s: dest==NULL");
    assert((src!=NULL) && "TinyTIFF_memcpy_s: src==NULL");
    #if defined(TINYTIFF_HASOBJSIZE)
        assert((tinytiff_builtin_object_size(dest,1)>=destsz) && "TinyTIFF_memcpy_s: dest too small (destsz incorrect)");
        assert((tinytiff_builtin_object_size(dest,1)>=count) && "TinyTIFF_memcpy_s: dest too small (count larger than available memory)");
        assert((tinytiff_builtin_object_size(src,1)>=count) && "TinyTIFF_memcpy_s: src too small");
    #endif
     memcpy(dest,  src, count);
#  endif
#endif
}

void TinyTIFF_memset_s(void *dest, unsigned long destsz, char ch, unsigned long count)
{
#ifdef HAVE_MEMSET_S
    memset_s(dest, destsz, ch, count);
#else
#  if defined(tinytiff_builtin_memset_chk)
    tinytiff_builtin_memset_chk(dest, ch, count);
#  else
    memset(dest,  ch, count);
#  endif
#endif

}

unsigned long TinyTIFF_strlen_s(const char *str, unsigned long strsz)
{
#ifdef HAVE_STRLEN_S
    return strlen_s(str, strsz);
#else
    if (str==NULL) return 0;
    #if defined(TINYTIFF_HASOBJSIZE)
        assert((tinytiff_builtin_object_size1(str)>=strsz) && "TinyTIFF_strlen_s: str too small (strsz incorrect)");
    #endif
    return strlen(str);
#endif

}
