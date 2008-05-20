#ifndef DISABLE_SOUND
#include "SDL.h"
#include "SDL_mixer.h"

#include "ring_sound.h"
#endif

#include "sdlring.h"




SDLRing::SDLRing() :
  initialized(0)
{
#ifndef DISABLE_SOUND
  //Initialise SDL, mixer subsystem and load sound sample
  //Fails are treated silently
  SDL_RWops *rw = SDL_RWFromConstMem(ring_sound_data, sizeof(ring_sound_data)); 

  if(SDL_Init(SDL_INIT_AUDIO) < 0) {
    return;
  }
  
  if (Mix_OpenAudio(11025, AUDIO_S16SYS, 1, 1024) < 0)
    {
      return;
    }

  

  ring_sound = Mix_LoadWAV_RW(rw, 0);
  
  initialized = 1;
#endif
}

SDLRing::~SDLRing() 
{
#ifndef DISABLE_SOUND
  Mix_FreeChunk(ring_sound);
  
  Mix_CloseAudio();

  SDL_Quit();
#endif
}

void SDLRing::ring()
{
#ifndef DISABLE_SOUND
  if(initialized)
    {
      Mix_PlayChannel(-1, ring_sound, 0);
    }
#endif
}
