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
 *  Generic image decoder interface.
 */

#ifndef IMAGES_DECODER_H
#define IMAGES_DECODER_H

#include <vector>

#include "src/common/types.h"

#include "src/images/types.h"

namespace Common {
	class SeekableReadStream;
	class UString;
}

namespace Images {

/** A generic interface for image decoders. */
class Decoder {
public:
	Decoder();
	virtual ~Decoder();

	/** A mip map. */
	struct MipMap {
		int    width;  ///< The mip map's width.
		int    height; ///< The mip map's height.
		uint32 size;   ///< The mip map's size in bytes.
		byte  *data;   ///< The mip map's data.

		MipMap();
		~MipMap();

		void swap(MipMap &right);
	};

	/** Return the image's general format. */
	PixelFormat getFormat() const;

	/** Return the number of mip maps contained in the image. */
	uint32 getMipMapCount() const;

	/** Return a mip map. */
	const MipMap &getMipMap(uint32 mipMap) const;

	/** Return TXI data, if embedded in the image. */
	virtual Common::SeekableReadStream *getTXI() const;

	/** Dump the image into a TGA. */
	bool dumpTGA(const Common::UString &fileName) const;

protected:
	PixelFormat _format;

	std::vector<MipMap *> _mipMaps;

	/** Is the image data compressed? */
	bool isCompressed() const;

	/** Manually decompress the texture image data. */
	void decompress();

	static void decompress(MipMap &out, const MipMap &in, PixelFormat format);
};

} // End of namespace Images

#endif // IMAGES_DECODER_H
