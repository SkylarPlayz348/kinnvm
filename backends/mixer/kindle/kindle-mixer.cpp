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

// GStreamer 0.10 headers are plain C
#include <gst/gst.h>

#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "audio/mixer_intern.h"
#include "backends/mixer/kindle/kindle-mixer.h"
#include "common/debug.h"
#include "common/textconsole.h"

// 44100 Hz, stereo, 16-bit.  512 stereo frames ≈ 11 ms per buffer.
static const int kRate = 44100;
static const int kFrames = 512;
static const int kBufBytes = kFrames * 2 * 2; // frames × channels × bytes/sample

KindleGstMixerManager::KindleGstMixerManager()
	: _threadStarted(false), _running(false), _pipeline(nullptr) {
	_pipeFd[0] = _pipeFd[1] = -1;
}

KindleGstMixerManager::~KindleGstMixerManager() {
	if (_mixer)
		_mixer->setReady(false);

	// Signal thread to stop and close the write end so the pipeline sees EOF.
	_running = false;
	if (_pipeFd[1] != -1) {
		close(_pipeFd[1]);
		_pipeFd[1] = -1;
	}

	if (_threadStarted)
		pthread_join(_thread, nullptr);

	if (_pipeline) {
		GstElement *p = (GstElement *)_pipeline;
		gst_element_set_state(p, GST_STATE_NULL);
		gst_object_unref(p);
		_pipeline = nullptr;
	}

	if (_pipeFd[0] != -1) {
		close(_pipeFd[0]);
		_pipeFd[0] = -1;
	}
}

void KindleGstMixerManager::init() {
	// Prevent SIGPIPE if the pipeline dies while we are writing.
	signal(SIGPIPE, SIG_IGN);

	gst_init(nullptr, nullptr);

	if (pipe(_pipeFd) != 0) {
		warning("KindleGst: pipe() failed");
		return;
	}

	// Shrink the kernel pipe buffer to the minimum (4096 bytes on Linux) so
	// at most one buffer sits in the pipe at a time, keeping end-to-end
	// latency low when audio content changes (e.g. room transitions).
	fcntl(_pipeFd[0], F_SETPIPE_SZ, 4096);

	// Pipeline: fdsrc reads raw PCM from our pipe, single-buffer queue
	// decouples threads, mixersink outputs to hardware.
	// latency-time (µs) shrinks mixersink's internal ring buffer from its
	// default (~2 s) to ~100 ms so transitions feel responsive.
	char pstr[300];
	snprintf(pstr, sizeof(pstr),
			 "fdsrc fd=%d blocksize=%d ! "
			 "audio/x-raw-int,endianness=1234,signed=true,width=16,depth=16,"
			 "rate=%d,channels=2 ! "
			 "queue max-size-buffers=1 max-size-bytes=0 max-size-time=0 ! "
			 "mixersink latency-time=100000",
			 _pipeFd[0], kBufBytes, kRate);

	GError *err = nullptr;
	GstElement *pipeline = gst_parse_launch(pstr, &err);
	if (!pipeline) {
		warning("KindleGst: gst_parse_launch failed: %s",
				err ? err->message : "unknown");
		if (err)
			g_error_free(err);
		return;
	}
	if (err) {
		// Non-fatal parse warning
		warning("KindleGst: gst_parse_launch warning: %s", err->message);
		g_error_free(err);
	}

	GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		warning("KindleGst: failed to start pipeline");
		gst_object_unref(pipeline);
		return;
	}
	_pipeline = pipeline;

	_mixer = new Audio::MixerImpl(kRate, true, kFrames);
	assert(_mixer);
	_mixer->setReady(true);

	_running = true;
	pthread_create(&_thread, nullptr, threadEntry, this);
	_threadStarted = true;

	debug(1, "KindleGst: audio pipeline running (%d Hz, fdsrc fd=%d)",
		  kRate, _pipeFd[0]);
}

void KindleGstMixerManager::suspendAudio() {
	if (_pipeline)
		gst_element_set_state((GstElement *)_pipeline, GST_STATE_PAUSED);
	_audioSuspended = true;
}

int KindleGstMixerManager::resumeAudio() {
	if (!_audioSuspended)
		return -2;
	if (_pipeline)
		gst_element_set_state((GstElement *)_pipeline, GST_STATE_PLAYING);
	_audioSuspended = false;
	return 0;
}

void *KindleGstMixerManager::threadEntry(void *arg) {
	((KindleGstMixerManager *)arg)->audioLoop();
	return nullptr;
}

void KindleGstMixerManager::audioLoop() {
	byte buf[kBufBytes];

	// How long (ns) each buffer represents at the output rate.
	const long bufNs = (long)((long long)kFrames * 1000000000LL / kRate);

	struct timespec next;
	clock_gettime(CLOCK_MONOTONIC, &next);

	while (_running) {
		if (_audioSuspended) {
			// Reset reference clock so we don't burst on resume.
			clock_gettime(CLOCK_MONOTONIC, &next);
			usleep(10000);
			continue;
		}

		_mixer->mixCallback(buf, kBufBytes);

		const byte *ptr = buf;
		int remaining = kBufBytes;
		while (remaining > 0 && _running) {
			ssize_t n = write(_pipeFd[1], ptr, remaining);
			if (n <= 0) {
				_running = false;
				break;
			}
			ptr += n;
			remaining -= n;
		}

		// Advance deadline by one buffer duration and sleep until then.
		// This keeps us from flooding the pipe faster than realtime.
		next.tv_nsec += bufNs;
		while (next.tv_nsec >= 1000000000L) {
			next.tv_sec++;
			next.tv_nsec -= 1000000000L;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, nullptr);
	}
}
