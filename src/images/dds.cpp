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
 *  DDS (DirectDraw Surface) loading.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "images/dds.h"

static const uint32 kDDSID  = MKTAG('D', 'D', 'S', ' ');
static const uint32 kDXT1ID = MKTAG('D', 'X', 'T', '1');
static const uint32 kDXT3ID = MKTAG('D', 'X', 'T', '3');
static const uint32 kDXT5ID = MKTAG('D', 'X', 'T', '5');

static const uint32 kHeaderFlagsHasMipMaps = 0x00020000;

static const uint32 kPixelFlagsHasAlpha  = 0x00000001;
static const uint32 kPixelFlagsHasFourCC = 0x00000004;
static const uint32 kPixelFlagsIsIndexed = 0x00000020;
static const uint32 kPixelFlagsIsRGB     = 0x00000040;

namespace Images {

DDS::DDS(Common::SeekableReadStream &dds) {
	load(dds);
}

DDS::~DDS() {
}

void DDS::load(Common::SeekableReadStream &dds) {
	try {

		readHeader(dds);
		readData  (dds);

		if (dds.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading DDS file");
		throw;
	}

	// In Phaethon, we always want decompressed images
	decompress();
}

void DDS::readHeader(Common::SeekableReadStream &dds) {
	if (dds.readUint32BE() == kDDSID)
		// We found the FourCC of a standard DDS
		readStandardHeader(dds);
	else
		// FourCC not found, should be a BioWare DDS then
		readBioWareHeader(dds);
}

void DDS::readStandardHeader(Common::SeekableReadStream &dds) {
	// All DDS header should be 124 bytes (+ 4 for the FourCC) big
	if (dds.readUint32LE() != 124)
		throw Common::Exception("Header size invalid");

	// DDS features
	uint32 flags = dds.readUint32LE();

	// Image dimensions
	uint32 height = dds.readUint32LE();
	uint32 width  = dds.readUint32LE();

	dds.skip(4 + 4); // Pitch + Depth
	//uint32 pitchOrLineSize = dds.readUint32LE();
	//uint32 depth           = dds.readUint32LE();
	uint32 mipMapCount     = dds.readUint32LE();

	// DDS doesn't provide any mip maps, only one full-size image
	if ((flags & kHeaderFlagsHasMipMaps) == 0)
		mipMapCount = 1;

	dds.skip(44); // Reserved

	// Read the pixel data format
	DDSPixelFormat format;
	format.size     = dds.readUint32LE();
	format.flags    = dds.readUint32LE();
	format.fourCC   = dds.readUint32BE();
	format.bitCount = dds.readUint32LE();
	format.rBitMask = dds.readUint32LE();
	format.gBitMask = dds.readUint32LE();
	format.bBitMask = dds.readUint32LE();
	format.aBitMask = dds.readUint32LE();

	// Detect which specific format it describes
	detectFormat(format);

	dds.skip(16 + 4); // DDCAPS2 + Reserved

	_mipMaps.reserve(mipMapCount);
	for (uint32 i = 0; i < mipMapCount; i++) {
		MipMap *mipMap = new MipMap;

		mipMap->width  = MAX<uint32>(width , 1);
		mipMap->height = MAX<uint32>(height, 1);

		setSize(*mipMap);

		mipMap->data = 0;

		width  >>= 1;
		height >>= 1;

		_mipMaps.push_back(mipMap);
	}

}

#define IsPower2(x) ((x) && (((x) & ((x) - 1)) == 0))
void DDS::readBioWareHeader(Common::SeekableReadStream &dds) {
	dds.seek(0);

	// Image dimensions
	uint32 width  = dds.readUint32LE();
	uint32 height = dds.readUint32LE();

	// Check that the width and height are really powers of 2
	if (!IsPower2(width) || !IsPower2(height))
		throw Common::Exception("Width and height must be powers of 2");

	// Check which compression
	uint32 bpp = dds.readUint32LE();
	if      (bpp == 3) {
		_format = kPixelFormatDXT1;
	} else if (bpp == 4) {
		_format = kPixelFormatDXT5;
	} else
		throw Common::Exception("Unsupported bytes per pixel value (%d)", bpp);

	// Sanity check for the image data size
	uint32 dataSize = dds.readUint32LE();
	if (((bpp == 3) && (dataSize != ((width * height) / 2))) ||
		  ((bpp == 4) && (dataSize != ((width * height)    ))))
		throw Common::Exception("Invalid data size (%dx%dx%d %d)", width, height, bpp, dataSize);

	dds.skip(4); // Some float

	// Number of bytes left for the image data
	uint32 fullDataSize = dds.size() - dds.pos();

	// Detect how many mip maps are in the DDS
	do {
		MipMap *mipMap = new MipMap;

		mipMap->width  = MAX<uint32>(width,  1);
		mipMap->height = MAX<uint32>(height, 1);

		setSize(*mipMap);

		if (fullDataSize < mipMap->size) {
			// Wouldn't fit
			delete mipMap;
			break;
		}

		fullDataSize -= mipMap->size;

		_mipMaps.push_back(mipMap);

		width  >>= 1;
		height >>= 1;

	} while ((width >= 1) && (height >= 1));
}

void DDS::setSize(MipMap &mipMap) {
	// Depending on the pixel format, set the image data size in bytes

	if (_format == kPixelFormatDXT1) {
		mipMap.size = ((mipMap.width + 3) / 4) * ((mipMap.height + 3) / 4) *  8;
	} else if (_format == kPixelFormatDXT3) {
		mipMap.size = ((mipMap.width + 3) / 4) * ((mipMap.height + 3) / 4) * 16;
	} else if (_format == kPixelFormatDXT5) {
		mipMap.size = ((mipMap.width + 3) / 4) * ((mipMap.height + 3) / 4) * 16;
	} else if (_format == kPixelFormatB8G8R8A8) {
		mipMap.size = mipMap.width * mipMap.height * 4;
	} else if (_format == kPixelFormatB8G8R8) {
		mipMap.size = mipMap.width * mipMap.height * 3;
	} else if (_format == kPixelFormatA1R5G5B5) {
		mipMap.size = mipMap.width * mipMap.height * 2;
	} else if (_format == kPixelFormatR5G6B5) {
		mipMap.size = mipMap.width * mipMap.height * 2;
	} else
		mipMap.size = 0;
}

void DDS::readData(Common::SeekableReadStream &dds) {
	// TODO: Do we need to flip the data?

	for (std::vector<MipMap *>::iterator mipMap = _mipMaps.begin(); mipMap != _mipMaps.end(); ++mipMap) {
		(*mipMap)->data = new byte[(*mipMap)->size];

		if (dds.read((*mipMap)->data, (*mipMap)->size) != (*mipMap)->size)
			throw Common::Exception(Common::kReadError);
	}
}

void DDS::detectFormat(const DDSPixelFormat &format) {
	// Big, ugly big pixel format description => format mapping

	if        ((format.flags & kPixelFlagsHasFourCC) && (format.fourCC == kDXT1ID)) {
		_format = kPixelFormatDXT1;
	} else if ((format.flags & kPixelFlagsHasFourCC) && (format.fourCC == kDXT3ID)) {
		_format = kPixelFormatDXT3;
	} else if ((format.flags & kPixelFlagsHasFourCC) && (format.fourCC == kDXT5ID)) {
		_format = kPixelFormatDXT5;
	} else if ((format.flags & kPixelFlagsIsRGB) && (format.flags & kPixelFlagsHasAlpha) &&
	           (format.bitCount == 32) &&
	           (format.rBitMask == 0x00FF0000) && (format.gBitMask == 0x0000FF00) &&
	           (format.bBitMask == 0x000000FF) && (format.aBitMask == 0xFF000000)) {
		_format = kPixelFormatB8G8R8A8;
	} else if ((format.flags & kPixelFlagsIsRGB) && !(format.flags & kPixelFlagsHasAlpha) &&
	           (format.bitCount == 24) &&
	           (format.rBitMask == 0x00FF0000) && (format.gBitMask == 0x0000FF00) &&
	           (format.bBitMask == 0x000000FF)) {
		_format = kPixelFormatB8G8R8;

		warning("Found untested DDS RGB8 data");

	} else if ((format.flags & kPixelFlagsIsRGB) && (format.flags & kPixelFlagsHasAlpha) &&
	           (format.bitCount == 16) &&
	           (format.rBitMask == 0x00007C00) && (format.gBitMask == 0x000003E0) &&
	           (format.bBitMask == 0x0000001F) && (format.aBitMask == 0x00008000)) {
		_format = kPixelFormatA1R5G5B5;

		warning("Found untested DDS RGB5A1 data");

	} else if ((format.flags & kPixelFlagsIsRGB) && !(format.flags & kPixelFlagsHasAlpha) &&
	           (format.bitCount == 16) &&
	           (format.rBitMask == 0x0000F800) && (format.gBitMask == 0x000007E0) &&
	           (format.bBitMask == 0x0000001F)) {
		_format = kPixelFormatR5G6B5;

		warning("Found untested DDS RGB5 data");

	} else if (format.flags & kPixelFlagsIsIndexed)
		// Hopefully, we'll never need to support that :P
		throw Common::Exception("Unsupported feature: Palette");
	else
		// We'll see if there's more formats in the data files :P
		throw Common::Exception("Unknown pixel format (%X, %X, %d, %X, %X, %X, %X)",
				format.flags, format.fourCC, format.bitCount,
				format.rBitMask, format.gBitMask, format.bBitMask, format.aBitMask);
}

} // End of namespace Images
