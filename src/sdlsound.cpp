#ifndef DISABLE_SOUND
#include "sdlsound.h"

#include "AL/alure.h"

#include "sound/ring_sound.h"
#include "sound/pm_sound.h"
#include <wx/log.h>
#include "utils/debug.h"
#include "utils/conversion.h"

#include <assert.h>

ALsound& sound()
{
	static ALsound m_sound;
    return m_sound;
}
volatile int isdone = 0;
static void eos_callback(void *unused, ALuint unused2)
{
	isdone = 1;
	(void)unused;
	(void)unused2;
}
ALsound::ALsound()
{
  //Initialise SDL, mixer subsystem and load sound sample
  //Fails are treated silently
//  SDL_RWops *ringwave = SDL_RWFromConstMem(ring_sound_data, sizeof(ring_sound_data));
//  SDL_RWops *pmwave = SDL_RWFromConstMem(pm_sound_data, sizeof(pm_sound_data));
	m_num_buffer = 0;
	m_num_sources = 0;


	ALuint alwavs = 0;

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
	//*

	//create single file source
	assert(alureInitDevice(0, 0));

	alGenSources(1,&alsource);
	if(alGetError()!=AL_NO_ERROR)
	{
		wxLogError( TowxString(alureGetErrorString()) );
		assert( false );
	}
	buf = alureCreateBufferFromMemory( pm_sound_data, sizeof(pm_sound_data)/sizeof(pm_sound_data[0]) );
	if ( alGetError()!=AL_NO_ERROR )
	{
		wxLogError( TowxString(alureGetErrorString()) );
		assert( false );
		return;
	}
	alSourcei(alsource, AL_BUFFER, buf);
}

ALsound::~ALsound()
{

}

void ALsound::ring() const
{

}


void ALsound::pm() const
{
	if (alurePlaySource(alsource, eos_callback, 0) == AL_FALSE)
	{
		wxLogError( TowxString(alureGetErrorString()) );
		assert( false );
		return;
	}
}

#endif
