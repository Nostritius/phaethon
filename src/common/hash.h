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

/** @file
 *  Utility hash functions.
 */

#ifndef COMMON_HASH_H
#define COMMON_HASH_H

#include "common/types.h"
#include "common/ustring.h"

namespace Common {

/** The algorithm used for hashing. */
enum HashAlgo {
	kHashNone  = -1, ///< No hashing at all.
	kHashDJB2  =  0, ///< djb2 hash function by Daniel J. Bernstein.
	kHashFNV32 =  1, ///< 32bit Fowler–Noll–Vo hash by Glenn Fowler, Landon Curt Noll and Phong Vo.
	kHashFNV64 =  2, ///< 64bit Fowler–Noll–Vo hash by Glenn Fowler, Landon Curt Noll and Phong Vo.
	kHashMAX         ///< For range checks.
};

/** djb2 hash function by Daniel J. Bernstein. */
static inline uint32 hashStringDJB2(const UString &string) {
	uint32 hash = 5381;

	for (UString::iterator it = string.begin(); it != string.end(); ++it)
		hash = ((hash << 5) + hash) + *it;

	return hash;
}

/** 32bit Fowler–Noll–Vo hash by Glenn Fowler, Landon Curt Noll and Phong Vo. */
static inline uint32 hashStringFNV32(const UString &string) {
	uint32 hash = 0x811C9DC5;

	for (UString::iterator it = string.begin(); it != string.end(); ++it)
		hash = (hash * 16777619) ^ *it;

	return hash;
}

/** 64bit Fowler–Noll–Vo hash by Glenn Fowler, Landon Curt Noll and Phong Vo. */
static inline uint64 hashStringFNV64(const UString &string) {
	uint64 hash = 0xCBF29CE484222325LL;

	for (UString::iterator it = string.begin(); it != string.end(); ++it)
		hash = (hash * 1099511628211LL) ^ *it;

	return hash;
}

static inline uint64 hashString(const UString &string, HashAlgo algo) {
	switch (algo) {
		case kHashDJB2:
			return hashStringDJB2(string);

		case kHashFNV32:
			return hashStringFNV32(string);

		case kHashFNV64:
			return hashStringFNV64(string);

		default:
			break;
	}

	return 0;
}

static inline UString formatHash(uint64 hash) {
	return UString::sprintf("0x%04X%04X%04X%04X",
			(uint) ((hash >> 48) & 0xFFFF), (uint) ((hash >> 32) & 0xFFFF),
			(uint) ((hash >> 16) & 0xFFFF), (uint) ( hash        & 0xFFFF));
}

} // End of namespace Common

#endif // COMMON_HASH_H
