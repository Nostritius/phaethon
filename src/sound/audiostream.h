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

#ifndef SOUND_AUDIOSTREAM_H
#define SOUND_AUDIOSTREAM_H

#include "common/types.h"

namespace Sound {

/** Generic audio input stream. */
class AudioStream {
public:
	virtual ~AudioStream() {}

	/**
	 * Fill the given buffer with up to numSamples samples. Returns the actual
	 * number of samples read, or -1 if a critical error occurred (note: you
	 * *must* check if this value is less than what you requested, this can
	 * happen when the stream is fully used up).
	 *
	 * Data has to be in native endianess, 16 bit per sample, signed. For stereo
	 * stream, buffer will be filled with interleaved left and right channel
	 * samples, starting with a left sample. Furthermore, the samples in the
	 * left and right are summed up. So if you request 4 samples from a stereo
	 * stream, you will get a total of two left channel and two right channel
	 * samples.
	 *
	 * The same holds true for more channels. Channel configurations recognized:
	 * - 5.1: front left, front right, front center, low frequency rear left, rear right
	 */
	virtual int readBuffer(int16 *buffer, const int numSamples) = 0;

	/** Return the number channels in this stream. */
	virtual int getChannels() const = 0;

	/** Sample rate of the stream. */
	virtual int getRate() const = 0;

	/**
	 * End of data reached? If this returns true, it means that at this
	 * time there is no data available in the stream. However there may be
	 * more data in the future.
	 * This is used by e.g. a rate converter to decide whether to keep on
	 * converting data or stop.
	 */
	virtual bool endOfData() const = 0;

	/**
	 * End of stream reached? If this returns true, it means that all data
	 * in this stream is used up and no additional data will appear in it
	 * in the future.
	 * This is used by the mixer to decide whether a given stream shall be
	 * removed from the list of active streams (and thus be destroyed).
	 * By default this maps to endOfData()
	 */
	virtual bool endOfStream() const { return endOfData(); }
};

/**
 * A rewindable audio stream. This allows for reseting the AudioStream
 * to its initial state. Note that rewinding itself is not required to
 * be working when the stream is being played by Mixer!
 */
class RewindableAudioStream : public AudioStream {
public:
	static const uint64 kInvalidLength = UINT64_C(0xFFFFFFFFFFFFFFFF);

	/**
	 * Rewinds the stream to its start.
	 *
	 * @return true on success, false otherwise.
	 */
	virtual bool rewind() = 0;

	/**
	 * Estimate the total number of samples per channel in this stream.
	 * If this value is not calculatable, return kInvalidLength.
	 */
	virtual uint64 getLength() const { return kInvalidLength; }

	/**
	 * Estimate the total duration of the stream in milliseconds.
	 * If this value is not calculable, return kInvalidLength.
	 */
	virtual uint64 getDuration() const {
		if ((getLength() == kInvalidLength) || (getRate() <= 0))
			return kInvalidLength;

		return (getLength() * 1000) / getRate();
	}
};

/**
 * A looping audio stream. This object does nothing besides using
 * a RewindableAudioStream to play a stream in a loop.
 */
class LoopingAudioStream : public RewindableAudioStream {
public:
	/**
	 * Creates a looping audio stream object.
	 *
	 * @see makeLoopingAudioStream
	 *
	 * @param stream Stream to loop
	 * @param loops How often to loop (0 = infinite)
	 * @param disposeAfterUse Destroy the stream after the LoopingAudioStream has finished playback.
	 */
	LoopingAudioStream(RewindableAudioStream *stream, uint loops, bool disposeAfterUse = true);
	~LoopingAudioStream();

	int readBuffer(int16 *buffer, const int numSamples);
	bool endOfData() const;

	int getChannels() const { return _parent->getChannels(); }
	int getRate() const { return _parent->getRate(); }

	/** Returns number of loops the stream has played. */
	uint getCompleteIterations() const { return _completeIterations; }

	bool rewind();

	uint64 getLength() const;
	uint64 getDuration() const;

	/** Return the length of one loop. */
	uint64 getLengthOnce() const;
	/** Return the duration of one loop. */
	uint64 getDurationOnce() const;

private:
	RewindableAudioStream *_parent;
	bool _disposeAfterUse;

	uint _loops;
	uint _completeIterations;
};

} // End of namespace Sound

#endif
