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

// Largely based on the Ogg Vorbis implementation found in ScummVM.

/** @file
 *  Decoding Ogg Vorbis.
 */

#ifndef SOUND_DECODERS_VORBIS_H
#define SOUND_DECODERS_VORBIS_H

#include "src/common/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

class RewindableAudioStream;

/**
 * Create a new RewindableAudioStream from the Ogg Vorbis data in the given stream.
 *
 * @param stream          The RewindableAudioStream from which to read the Ogg Vorbis data.
 * @param disposeAfterUse Whether to delete the stream after use.
 *
 * @return A new RewindableAudioStream, or 0, if an error occurred.
 */
RewindableAudioStream *makeVorbisStream(
	Common::SeekableReadStream *stream,
	bool disposeAfterUse);

} // End of namespace Sound

#endif // SOUND_DECODERS_VORBIS_H
