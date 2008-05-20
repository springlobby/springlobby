#ifndef SPRINGLOBBY_HEADERGUARD_SDLRING_H
#define SPRINGLOBBY_HEADERGUARD_SDLRING_H

#ifndef DISABLE_SOUND
#include "SDL.h"
#include "SDL_mixer.h"
#endif


class SDLRing
{
 public:
  SDLRing();
  ~SDLRing();

  void ring();

 private:
  bool initialized;

#ifndef DISABLE_SOUND
  Mix_Chunk *ring_sound;
#endif
};

#endif // SPRINGLOBBY_HEADERGUARD_SDLRING_H
