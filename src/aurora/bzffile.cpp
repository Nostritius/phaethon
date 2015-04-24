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
 *  Handling BioWare's BZFs (resource data files), used in the iOS version of
 *  Knights of the Old Republic.
 *
 *  Essentially, they are BIF files with LZMA-compressed data.
 */

#include <lzma.h>

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"
#include "common/file.h"

#include "aurora/bzffile.h"

static const uint32 kBZFID     = MKTAG('B', 'I', 'F', 'F');
static const uint32 kVersion1  = MKTAG('V', '1', ' ', ' ');

namespace Aurora {

BZFFile::BZFFile(const Common::UString &fileName) : _fileName(fileName) {
	load();
}

BZFFile::~BZFFile() {
}

void BZFFile::open(Common::File &file) const {
	if (!file.open(_fileName))
		throw Common::Exception(Common::kOpenError);
}

void BZFFile::load() {
	Common::File bzf;
	open(bzf);

	readHeader(bzf);

	if (_id != kBZFID)
		throw Common::Exception("Not a BZF file");

	if (_version != kVersion1)
		throw Common::Exception("Unsupported BZF file version %08X", _version);

	uint32 varResCount = bzf.readUint32LE();
	uint32 fixResCount = bzf.readUint32LE();

	if (fixResCount != 0)
		throw Common::Exception("TODO: Fixed BZF resources");

	uint32 offVarResTable = bzf.readUint32LE();

	try {

		_resources.resize(varResCount);
		readVarResTable(bzf, offVarResTable);

		if (bzf.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading BZF file");
		throw;
	}

}

void BZFFile::readVarResTable(Common::SeekableReadStream &bzf, uint32 offset) {
	if (!bzf.seek(offset))
		throw Common::Exception(Common::kSeekError);

	for (uint32_t i = 0; i < _resources.size(); i++) {
		bzf.skip(4); // ID

		_resources[i].offset = bzf.readUint32LE();
		_resources[i].size   = bzf.readUint32LE();
		_resources[i].type   = (FileType) bzf.readUint32LE();

		if (i > 0)
			_resources[i - 1].packedSize = _resources[i].offset - _resources[i - 1].offset;
	}

	if (!_resources.empty())
		_resources.back().packedSize = bzf.size() - _resources.back().offset;
}

Common::SeekableReadStream *BZFFile::getResource(uint32 index) const {
	const Resource &res = getRes(index);
	if ((res.packedSize == 0) || (res.size == 0))
		return new Common::MemoryReadStream(0, 0);

	Common::File bzf;
	open(bzf);

	if (!bzf.seek(res.offset))
		throw Common::Exception(Common::kSeekError);

	byte *compressedData = new byte[res.packedSize];

	Common::SeekableReadStream *resStream = 0;
	try {
		if (bzf.read(compressedData, res.packedSize) != res.packedSize)
			throw Common::Exception(Common::kReadError);

		resStream = decompress(compressedData, res.packedSize, res.size);

	} catch (...) {
		delete[] compressedData;
		throw;
	}

	delete[] compressedData;
	return resStream;
}

Common::SeekableReadStream *BZFFile::decompress(byte *compressedData, uint32 packedSize, uint32 unpackedSize) const {

	lzma_filter filters[2];
	filters[0].id      = LZMA_FILTER_LZMA1;
	filters[0].options = 0;
	filters[1].id      = LZMA_VLI_UNKNOWN;
	filters[1].options = 0;

	if (!lzma_filter_decoder_is_supported(filters[0].id))
		throw Common::Exception("LZMA1 compression not supported");

	uint32_t propsSize;
	if (lzma_properties_size(&propsSize, &filters[0]) != LZMA_OK)
		throw Common::Exception("Can't get LZMA1 properties size");

	if (lzma_properties_decode(&filters[0], 0, compressedData, propsSize) != LZMA_OK)
		throw Common::Exception("Failed to decode LZMA properties");

	compressedData += propsSize;
	packedSize     -= propsSize;

	byte *uncompressedData = new byte[unpackedSize];
	size_t posIn = 0, posOut = 0;

	lzma_ret decodeRet = lzma_raw_buffer_decode(filters, 0,
			compressedData  , &posIn , packedSize,
			uncompressedData, &posOut, unpackedSize);

	/* Ignore LZMA_DATA_ERROR and LZMA_BUF_ERROR thrown from the uncompressor.
	 * LZMA data in BZF may or may not contain an end marker.
	 * - If there is no end marker, LZMA_BUF_ERROR is thrown
	 * - If there is an end marker, LZMA_DATA_ERROR is thrown because we already
	 *   know the size of the uncompressed data
	 */
	if ((decodeRet != LZMA_OK) && (decodeRet != LZMA_DATA_ERROR) && (decodeRet != LZMA_BUF_ERROR)) {
		delete[] uncompressedData;
		throw Common::Exception("Failed to uncompress LZMA data: %d", (int) decodeRet);
	}

	return new Common::MemoryReadStream(uncompressedData, unpackedSize, true);
}

} // End of namespace Aurora
