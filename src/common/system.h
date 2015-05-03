/* Phaethon - A FLOSS resource explorer for BioWare's Aurora engine games
 *
 * Phaethon is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * Phaethon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * Phaethon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Phaethon. If not, see <http://www.gnu.org/licenses/>.
 */

// Mostly based on ScummVM's architecture/system checks

/** @file
 *  Low-level detection of architecture/system properties.
 */

#ifndef COMMON_SYSTEM_H
#define COMMON_SYSTEM_H

#if defined(HAVE_CONFIG_H)
	#include "config.h"
#endif

#if defined(_MSC_VER)

	#include <cstdarg>
	#include <cstdio>

	#define snprintf c99_snprintf
	#define vsnprintf c99_vsnprintf

	#define PHAETHON_LITTLE_ENDIAN

	#define FORCEINLINE __forceinline
	#define NORETURN_PRE __declspec(noreturn)
	#define PLUGIN_EXPORT __declspec(dllexport)

	static FORCEINLINE int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap) {
		int count = -1;

		if (size != 0)
			count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
		if (count == -1)
			count = _vscprintf(format, ap);

		return count;
	}

	static FORCEINLINE int c99_snprintf(char* str, size_t size, const char* format, ...) {
		int count;
		va_list ap;

		va_start(ap, format);
		count = c99_vsnprintf(str, size, format, ap);
		va_end(ap);

		return count;
	}

	#ifndef WIN32
		#define WIN32
	#endif

	#define IGNORE_UNUSED_VARIABLES __pragma(warning(disable : 4101))

#elif defined(__MINGW32__)

	#define PHAETHON_LITTLE_ENDIAN

	#define PLUGIN_EXPORT __declspec(dllexport)

	#ifndef WIN32
		#define WIN32
	#endif

#elif defined(UNIX)

	#if defined(HAVE_CONFIG_H)
		#if defined(WORDS_BIGENDIAN)
			#define PHAETHON_BIG_ENDIAN
		#else
			#define PHAETHON_LITTLE_ENDIAN
		#endif
	#endif

#else
	#error No system type defined
#endif

//
// GCC specific stuff
//
#if defined(__GNUC__)
	#define NORETURN_POST __attribute__((__noreturn__))
	#define PACKED_STRUCT __attribute__((__packed__))
	#define GCC_PRINTF(x,y) __attribute__((__format__(printf, x, y)))
	#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

	#if (__GNUC__ >= 3)
		// Macro to ignore several "unused variable" warnings produced by GCC
		#define IGNORE_UNUSED_VARIABLES _Pragma("GCC diagnostic ignored \"-Wunused-variable\"") \
		                                _Pragma("GCC diagnostic ignored \"-Wunused-but-set-variable\"")
	#endif

	#if !defined(FORCEINLINE) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
		#define FORCEINLINE inline __attribute__((__always_inline__))
	#endif
#else
	#define PACKED_STRUCT
	#define GCC_PRINTF(x,y)
	#define UNUSED(x) UNUSED_ ## x
#endif

#if defined(__clang__)
	// clang does not know the "unused-but-set-variable" (but claims to be GCC)
	#undef IGNORE_UNUSED_VARIABLES
	#define IGNORE_UNUSED_VARIABLES _Pragma("GCC diagnostic ignored \"-Wunused-variable\"")
#endif

//
// Fallbacks / default values for various special macros
//
#ifndef FORCEINLINE
	#define FORCEINLINE inline
#endif

#ifndef NORETURN_PRE
	#define NORETURN_PRE
#endif

#ifndef NORETURN_POST
	#define NORETURN_POST
#endif

#ifndef STRINGBUFLEN
	#define STRINGBUFLEN 1024
#endif

#ifndef MAXPATHLEN
	#define MAXPATHLEN 256
#endif

#ifndef IGNORE_UNUSED_VARIABLES
	#define IGNORE_UNUSED_VARIABLES
#endif

#endif // COMMON_SYSTEM_H
