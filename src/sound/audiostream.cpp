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

// Largely based on the AudioStream implementation found in ScummVM.

/** @file
 *  Streaming audio.
 */

#include <queue>

#include "src/common/error.h"
#include "src/common/mutex.h"

#include "src/sound/audiostream.h"

namespace Sound {

LoopingAudioStream::LoopingAudioStream(RewindableAudioStream *stream, size_t loops, bool disposeAfterUse)
    : _parent(stream), _disposeAfterUse(disposeAfterUse), _loops(loops), _completeIterations(0) {
}

LoopingAudioStream::~LoopingAudioStream() {
	if (_disposeAfterUse)
		delete _parent;
}

size_t LoopingAudioStream::readBuffer(int16 *buffer, const size_t numSamples) {
	if ((_loops && _completeIterations == _loops) || !numSamples)
		return 0;

	const size_t samplesRead = _parent->readBuffer(buffer, numSamples);
	if (samplesRead == kSizeInvalid)
		return kSizeInvalid;

	if (_parent->endOfStream()) {
		++_completeIterations;
		if (_completeIterations == _loops)
			return samplesRead;

		const size_t remainingSamples = numSamples - samplesRead;

		if (!_parent->rewind()) {
			// TODO: Properly indicate error
			_loops = _completeIterations = 1;
			return samplesRead;
		}

		const size_t samplesReadNext = readBuffer(buffer + samplesRead, remainingSamples);
		if (samplesReadNext == kSizeInvalid)
			return kSizeInvalid;

		return samplesRead + samplesReadNext;
	}

	return samplesRead;
}

bool LoopingAudioStream::endOfData() const {
	return (_loops != 0 && (_completeIterations == _loops));
}

AudioStream *makeLoopingAudioStream(RewindableAudioStream *stream, size_t loops) {
	if (loops != 1)
		return new LoopingAudioStream(stream, loops);
	else
		return stream;
}

bool LoopingAudioStream::rewind() {
	if (!_parent->rewind())
		return false;

	_completeIterations = 0;
	return true;
}

uint64 LoopingAudioStream::getLength() const {
	if (!_loops)
		return RewindableAudioStream::kInvalidLength;

	uint64 length = _parent->getLength();
	if (length == RewindableAudioStream::kInvalidLength)
		return RewindableAudioStream::kInvalidLength;

	return _loops * length;
}

uint64 LoopingAudioStream::getDuration() const {
	if (!_loops)
		return RewindableAudioStream::kInvalidLength;

	uint64 duration = _parent->getDuration();
	if (duration == RewindableAudioStream::kInvalidLength)
		return RewindableAudioStream::kInvalidLength;

	return _loops * duration;
}

uint64 LoopingAudioStream::getLengthOnce() const {
	return _parent->getLength();
}

uint64 LoopingAudioStream::getDurationOnce() const {
	return _parent->getDuration();
}

} // End of namespace Sound
