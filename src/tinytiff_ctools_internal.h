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


#ifndef TINYTIFF_CTOOLS_INTERNAL_H
#define TINYTIFF_CTOOLS_INTERNAL_H

#if defined(HAVE_FTELLI64) || defined(HAVE_FTELLO64)
#  define TINYTIFF_MAX_FILE_SIZE (0xFFFFFFFE)
#else
#  warning COMPILING TinyTIFFWriter without LARGE_FILE_SUPPORT ... File size is limited to 2GB!
#  define TINYTIFF_MAX_FILE_SIZE (2*1024*1024*1024-1)
#endif


#if defined(__has_builtin)
#  if __has_builtin(__builtin_object_size)
#    define tinytiff_builtin_object_size(ptr) __builtin_object_size(ptr, 2)
#    define tinytiff_builtin_object_size0(dest) __builtin_object_size (dest, 0)
#    define tinytiff_builtin_object_size1(dest) __builtin_object_size (dest, 1)
#    define TINYTIFF_HASOBJSIZE
#  endif
#endif

#ifdef HAVE_STRCPY_S
#define TINYTIFF_SET_LAST_ERROR(tiff, message) strcpy_s(tiff->lastError, TIFF_LAST_ERROR_SIZE, message);
#define TINYTIFF_STRCPY_S(dest, dsize, src) strcpy_s(dest, dsize, src);
#else
#  if defined(TINYTIFF_HASOBJSIZE) && __has_builtin(__builtin___strcpy_chk)
#    define TINYTIFF_SET_LAST_ERROR(tiff, message) __builtin___strcpy_chk(tiff->lastError, message, tinytiff_builtin_object_size0(tiff->lastError))
#    define TINYTIFF_STRCPY_S(dest, dsize, src) __builtin___strcpy_chk(dest, src, tinytiff_builtin_object_size0(dest))
#  else
#    define TINYTIFF_SET_LAST_ERROR(tiff, message) strcpy(tiff->lastError, message);
#    define TINYTIFF_STRCPY_S(dest, dsize, src) strcpy(dest, src);
#  endif
#endif


#ifdef HAVE_STRCAT_S
#define TINYTIFF_STRCAT_S(dest, dsize, src) strcat_s(dest, dsize, src);
#else
#  if defined(TINYTIFF_HASOBJSIZE) && __has_builtin(__builtin___strcat_chk)
#    define TINYTIFF_STRCAT_S(dest, dsize, src) __builtin___strcat_chk(dest, src, tinytiff_builtin_object_size0(dest))
#  else
#    define TINYTIFF_STRCAT_S(dest, dsize, src) strcat(dest, src);
#  endif
#endif


#ifdef HAVE_SPRINTF_S
#  define TINYTIFF_SPRINTF_S(str, ssize, message, ...) sprintf_s(str, ssize, message, __VA_ARGS__);
#  define TINYTIFF_SPRINTF_LAST_ERROR(tiff, message, ...) sprintf_s(tiff->lastError, TIFF_LAST_ERROR_SIZE, message, __VA_ARGS__);
#else
#  if defined(TINYTIFF_HASOBJSIZE) && __has_builtin(__builtin___sprintf_chk)
#    define TINYTIFF_SPRINTF_LAST_ERROR(tiff, message, ...) __builtin___sprintf_chk(tiff->lastError, 0, tinytiff_builtin_object_size0(tiff->lastError), message, __VA_ARGS__)
#    define TINYTIFF_SPRINTF_S(str, ssize, message, ...) __builtin___sprintf_chk(str, 0, tinytiff_builtin_object_size0(str), message, __VA_ARGS__)
#  else
#    define TINYTIFF_SPRINTF_S(str, ssize, message, ...) sprintf(str, message, __VA_ARGS__);
#    define TINYTIFF_SPRINTF_LAST_ERROR(tiff, message, ...) sprintf(tiff->lastError, message, __VA_ARGS__);
#  endif
#endif


/** \brief hardened memcpy(), that calls memcpy_s() or __builtin___memcpy_chk() if available
 *  \internal
 *  \ingroup tinytiffreader_internal
 */
void TinyTIFF_memcpy_s( void * dest, unsigned long destsz, const void * src, unsigned long count );

/** \brief hardened memset(), that calls memset_s() or __builtin___memset_chk() if available
 *  \internal
 *  \ingroup tinytiffreader_internal
 */
void TinyTIFF_memset_s( void * dest, unsigned long destsz, char ch, unsigned long count );

/** \brief hardened strlen(), that calls strlen_s() or __builtin___strlen_chk() if available
 *  \internal
 *  \ingroup tinytiffreader_internal
 */
unsigned long TinyTIFF_strlen_s( const char * str, unsigned long strsz);

#endif // TINYTIFF_CTOOLS_INTERNAL_H
