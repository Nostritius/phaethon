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

// Based upon the (I)MDCT code in FFmpeg
// Copyright (c) 2002 Fabrice Bellard

/** @file
 *  (Inverse) Modified Discrete Cosine Transforms.
 */

#ifndef COMMON_MDCT_H
#define COMMON_MDCT_H

#include "common/types.h"

namespace Common {

class FFT;

/** (Inverse) Modified Discrete Cosine Transforms. */
class MDCT {
public:
	MDCT(int bits, bool inverse, double scale);
	~MDCT();

	/** Compute MDCT of size N = 2^nbits. */
	void calcMDCT(float *output, const float *input);

	/** Compute inverse MDCT of size N = 2^nbits. */
	void calcIMDCT(float *output, const float *input);

private:
	int _bits;
	int _size;

	float *_tCos;
	float *_tSin;

	FFT *_fft;

	/** Compute the middle half of the inverse MDCT of size N = 2^nbits,
	 *  thus excluding the parts that can be derived by symmetry.
	 */
	void calcHalfIMDCT(float *output, const float *input);
};

} // End of namespace Common

#endif // COMMON_MDCT_H
