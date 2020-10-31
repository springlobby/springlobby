/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#ifndef DISABLE_SOUND
#include "alsound.h"

#ifdef __APPLE__
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

#ifdef _WIN32
//win32 compile uses static libs
#define ALURE_STATIC_LIBRARY
#endif

#include <alure.h>
#include <assert.h>
#include <lslutils/globalsmanager.h>
#include <wx/log.h>
#include <wx/thread.h>

#include "pm_sound.h"
#include "ring_sound.h"
#include "utils/conversion.h"

const char* GetALErrorString(ALenum& error)
{
	switch (error) {
	case AL_NO_ERROR:
		return "(OpenAL) there is not currently an error";
	case AL_INVALID_NAME:
		return "a bad name (ID) was passed to an OpenAL function";
	case AL_INVALID_ENUM:
		return "an invalid enum value was passed to an OpenAL function";
	case AL_INVALID_VALUE:
		return "an invalid value was passed to an OpenAL function";
	case AL_INVALID_OPERATION:
		return "(OpenAL) the requested operation is not valid";
	case AL_OUT_OF_MEMORY:
		return "the requested operation resulted in OpenAL running out of memory";
	default:
		return "unknown OpenAL error code";
	}
}

#define BailOnALError() \
	{ \
		ALenum alError; \
		if (AL_NO_ERROR != (alError = alGetError())) { \
			wxLogError("OpenAL error %d: %s", alError, GetALErrorString(alError)); \
			return nullptr; \
		} \
	}

#define AssertAlureResult(cond) \
	if (! (cond) ) { \
		wxLogError("Alure error: %s", alureGetErrorString()); \
		return nullptr; \
	} \

ALsound& slsound()
{
	static LSL::Util::LineInfo<ALsound> m(AT);
	static LSL::Util::GlobalObjectHolder<ALsound, LSL::Util::LineInfo<ALsound> > m_sound(m);
	return m_sound;
}

static bool isdone;
static void eos_callback(void*, ALuint)
{
	isdone = true;
}

class soundThread : public wxThread
{
public:
	explicit soundThread(int idx)
	    : wxThread(wxTHREAD_JOINABLE)
	{
		m_idx = idx;
		Create();
	}

private:
	int m_idx;
	ExitCode Entry()
	{
		alGetError(); // Clear potential error code

		ALuint m_sources[1];
		ALuint m_buffers[1];

		AssertAlureResult(AL_FALSE != alureInitDevice(0, 0));

		//set up 3D sound, coordinates are x,y,z
		alListener3f(AL_POSITION, 0, 0, 0);
		BailOnALError();
		alListener3f(AL_VELOCITY, 0, 0, 0);
		BailOnALError();

		ALfloat alori[6];
		alori[0] = 0;
		alori[1] = 0;
		alori[2] = -1;
		alori[3] = 0;
		alori[4] = 1;
		alori[5] = 0;
		alListenerfv(AL_ORIENTATION, alori);
		BailOnALError();


		alGenSources(1, m_sources);
		BailOnALError();

		if (m_idx == 0) {
			m_buffers[0] = alureCreateBufferFromMemory(pm_sound_data, sizeof(pm_sound_data) / sizeof(pm_sound_data[0]));
		} else {
			m_buffers[0] = alureCreateBufferFromMemory(ring_sound_data, sizeof(ring_sound_data) / sizeof(ring_sound_data[0]));
		}
		AssertAlureResult(AL_NONE != m_buffers[0]);
		BailOnALError(); // Just in case... (was checked before rewrite)

		alSourcei(m_sources[0], AL_BUFFER, m_buffers[0]);
		BailOnALError();


		isdone = false;
		AssertAlureResult(AL_FALSE != alurePlaySource(m_sources[0], eos_callback, &isdone));

		while (!isdone) {
			alureSleep(0.125);
			alureUpdate();
		}

		alureStopSource(m_sources[0], AL_FALSE);
		alDeleteSources(1, m_sources);
		alureShutdownDevice();
		return nullptr;
	}
};

ALsound::ALsound()
    : m_thread(nullptr)
{
}

ALsound::~ALsound()
{
	if (m_thread != nullptr) {
		m_thread->Wait();
		delete m_thread;
	}
}

void ALsound::Play(int idx)
{
	// use a thread to play sounds because having the sound device opened seems to require some cpu
	if ((m_thread == nullptr) || (!m_thread->IsRunning())) {
		delete m_thread;
		m_thread = new soundThread(idx);
		m_thread->Run();
	}
}

void ALsound::ring()
{
	Play(1);
}


void ALsound::pm()
{
	Play(0);
}

#endif
