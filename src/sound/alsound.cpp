/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef DISABLE_SOUND

#ifdef __APPLE__
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

#include "alsound.h"

#ifdef WIN32
//win32 compile uses static libs
#define ALURE_STATIC_LIBRARY
#endif

#include <alure.h>

#include "ring_sound.h"
#include "pm_sound.h"
#include <wx/log.h>
#include "utils/conversion.h"
#include <lslutils/globalsmanager.h>
#include <assert.h>
#include <wx/thread.h>

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
		ALuint m_sources[1];
		ALuint m_buffers[1];
		//Init
		alGetError();
		//*

		//set up 3d sound
		ALfloat alpos[3];
		ALfloat alvel[3];
		ALfloat alori[6];
		alpos[0] = 0;
		alpos[1] = 0;
		alpos[2] = 0;
		alvel[0] = 0;
		alvel[1] = 0;
		alvel[2] = 0;
		alori[0] = 0;
		alori[1] = 0;
		alori[2] = -1;
		alori[3] = 0;
		alori[4] = 1;
		alori[5] = 0;

		alListenerfv(AL_POSITION, alpos);
		alListenerfv(AL_VELOCITY, alvel);
		alListenerfv(AL_ORIENTATION, alori);
		alureInitDevice(0, 0);
		//*

		alGenSources(1, m_sources);
		if (alGetError() != AL_NO_ERROR) {
			wxLogError(TowxString(alureGetErrorString()));
			return NULL;
		}

		if (m_idx == 0) {
			m_buffers[0] = alureCreateBufferFromMemory(pm_sound_data, sizeof(pm_sound_data) / sizeof(pm_sound_data[0]));
		} else {
			m_buffers[0] = alureCreateBufferFromMemory(ring_sound_data, sizeof(ring_sound_data) / sizeof(ring_sound_data[0]));
		}
		if (alGetError() != AL_NO_ERROR) {
			wxLogError(TowxString(alureGetErrorString()));
			return NULL;
		}
		alSourcei(m_sources[0], AL_BUFFER, m_buffers[0]);
		if (alGetError() != AL_NO_ERROR) {
			wxLogError(TowxString(alureGetErrorString()));
			return NULL;
		}
		isdone = false;
		if (alurePlaySource(m_sources[0], eos_callback, &isdone) == AL_FALSE) {
			wxLogError(TowxString(alureGetErrorString()));
		}
		while (!isdone) {
			alureSleep(0.125);
			alureUpdate();
		}
		alureStopSource(m_sources[0], AL_FALSE);
		alDeleteSources(1, m_sources);
		alureShutdownDevice();
		return NULL;
	}
};

ALsound::ALsound()
    : m_thread(NULL)
{
}

ALsound::~ALsound()
{
	if (m_thread != NULL) {
		m_thread->Wait();
		delete m_thread;
	}
}

void ALsound::Play(int idx)
{
	// use a thread to play sounds because having the sound device opened seems to require some cpu
	if ((m_thread == NULL) || (!m_thread->IsRunning())) {
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
