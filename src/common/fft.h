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

// Based upon the (I)FFT code in FFmpeg
// Copyright (c) 2008 Loren Merritt
// Copyright (c) 2002 Fabrice Bellard
// Partly based on libdjbfft by D. J. Bernstein

/** @file
 *  (Inverse) Fast Fourier Transform.
 */

#ifndef COMMON_FFT_H
#define COMMON_FFT_H

#include "src/common/types.h"

namespace Common {

struct Complex;

/** (Inverse) Fast Fourier Transform. */
class FFT {
public:
	FFT(int bits, bool inverse);
	~FFT();

	const uint16 *getRevTab() const;

	/** Do the permutation needed BEFORE calling calc(). */
	void permute(Complex *z);

	/** Do a complex FFT.
	 *
	 *  The input data must be permuted before.
	 *  No 1.0/sqrt(n) normalization is done.
	 */
	void calc(Complex *z);

private:
	int  _bits;
	bool _inverse;

	uint16 *_revTab;

	Complex *_expTab;
	Complex *_tmpBuf;

	int _splitRadix;

	static int splitRadixPermutation(int i, int n, bool inverse);
};

} // End of namespace Common

#endif // COMMON_FFT_H
