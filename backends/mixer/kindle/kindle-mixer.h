/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef BACKENDS_MIXER_KINDLE_H
#define BACKENDS_MIXER_KINDLE_H

#ifdef KINDLE

#include "backends/mixer/mixer.h"
#include <pthread.h>

/**
 * Kindle audio mixer: feeds ScummVM's mixed PCM into a GStreamer pipeline
 * via a Unix pipe so it reaches the Kindle's proprietary mixersink element.
 */
class KindleGstMixerManager : public MixerManager {
public:
	KindleGstMixerManager();
	virtual ~KindleGstMixerManager();

	virtual void init() override;
	virtual void suspendAudio() override;
	virtual int resumeAudio() override;

private:
	static void *threadEntry(void *arg);
	void audioLoop();

	int           _pipeFd[2];      // [0] read end (gstreamer), [1] write end (us)
	pthread_t     _thread;
	bool          _threadStarted;
	volatile bool _running;
	void         *_pipeline;       // GstElement*, kept opaque to avoid gst.h in header
};

#endif // KINDLE
#endif // BACKENDS_MIXER_KINDLE_H
