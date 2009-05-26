#ifndef DISABLE_SOUND

#include "SDL.h"
#include "SDL_mixer.h"

#include "sounds/ring_sound.h"
#include "sounds/pm_sound.h"

#include "sdlsound.h"


SDLSound& sound()
{
    static SDLSound m_sound;
    return m_sound;
}

SDLSound::SDLSound()
{
  //Initialise SDL, mixer subsystem and load sound sample
  //Fails are treated silently
  SDL_RWops *ringwave = SDL_RWFromConstMem(ring_sound_data, sizeof(ring_sound_data));
  SDL_RWops *pmwave = SDL_RWFromConstMem(pm_sound_data, sizeof(pm_sound_data));

  if(SDL_Init(SDL_INIT_AUDIO) < 0) {
    return;
  }

  if (Mix_OpenAudio(11025, AUDIO_S16SYS, 1, 1024) < 0)
    {
      return;
    }



  ring_sound = Mix_LoadWAV_RW(ringwave, 0);
  pm_sound = Mix_LoadWAV_RW( pmwave, 0);

}

SDLSound::~SDLSound()
{
  if ( ring_sound ) Mix_FreeChunk(ring_sound);
  if ( pm_sound ) Mix_FreeChunk(pm_sound);


  Mix_CloseAudio();

  SDL_Quit();
}

void SDLSound::ring() const
{
  if ( ring_sound ) Mix_PlayChannel(-1, ring_sound, 0);
}


void SDLSound::pm() const
{
  if ( pm_sound ) Mix_PlayChannel(-1, pm_sound, 0);
}

#endif
