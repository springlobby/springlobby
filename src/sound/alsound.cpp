#ifndef DISABLE_SOUND
#include "alsound.h"

#ifdef WIN32
//win32 compile uses static libs
#define ALURE_STATIC_LIBRARY
#endif

#include <alure.h>

#include "sound/ring_sound.h"
#include "sound/pm_sound.h"
#include <wx/log.h>
#include "utils/debug.h"
#include "utils/conversion.h"
#include <lslutils/globalsmanager.h>
#include <assert.h>

ALsound& sound()
{
	static LSL::Util::LineInfo<ALsound> m( AT );
	static LSL::Util::GlobalObjectHolder<ALsound, LSL::Util::LineInfo<ALsound> > m_sound( m );
	return m_sound;
}
//volatile int isdone = 0;
//static void eos_callback(void *unused, ALuint unused2)
//{
//	isdone = 1;
//	(void)unused;
//	(void)unused2;
//}
ALsound::ALsound()
{
	//Init
	alGetError();
	//*

	//set up 3d sound
	ALfloat	alpos[3];
	ALfloat	alvel[3];
	ALfloat	alori[6];
	alpos[0] = 0; alpos[1] = 0; alpos[2] = 0;
	alvel[0] = 0; alvel[1] = 0; alvel[2] = 0;
	alori[0] = 0; alori[1] = 0; alori[2] = -1;
	alori[3] = 0; alori[4] = 1; alori[5] = 0;

	alListenerfv(AL_POSITION,alpos);
	alListenerfv(AL_VELOCITY,alvel);
	alListenerfv(AL_ORIENTATION,alori);
	alureInitDevice(0, 0);
	//*

	alGenSources(m_num_sources,m_sources);
	if(alGetError()!=AL_NO_ERROR)
	{
		wxLogError( TowxString(alureGetErrorString()) );
		return;
	}

	m_buffers[0] = alureCreateBufferFromMemory( pm_sound_data, sizeof(pm_sound_data)/sizeof(pm_sound_data[0]) );
	m_buffers[1] = alureCreateBufferFromMemory( ring_sound_data, sizeof(ring_sound_data)/sizeof(ring_sound_data[0]) );

	if ( alGetError()!=AL_NO_ERROR )
	{
		wxLogError( TowxString(alureGetErrorString()) );
		return;
	}
	alSourcei(m_sources[0], AL_BUFFER, m_buffers[0] );
	alSourcei(m_sources[1], AL_BUFFER, m_buffers[1] );
	if ( alGetError()!=AL_NO_ERROR )
	{
		wxLogError( TowxString(alureGetErrorString()) );
		return;
	}
}

ALsound::~ALsound()
{
	alureStopSource(m_sources[0], AL_FALSE);
	alureStopSource(m_sources[1], AL_FALSE);

	alDeleteSources(m_num_sources, m_sources);
	alureShutdownDevice();
}

void ALsound::ring() const
{
	if (alurePlaySource(m_sources[1], 0, 0) == AL_FALSE) {
		wxLogError( TowxString(alureGetErrorString()) );
	}
}


void ALsound::pm() const
{
	if (alurePlaySource(m_sources[0], 0, 0) == AL_FALSE) {
		wxLogError( TowxString(alureGetErrorString()) );
	}
}

#endif
