#ifndef SPRINGLOBBY_HEADERGUARD_SDLSOUND_H
#define SPRINGLOBBY_HEADERGUARD_SDLSOUND_H

#ifndef DISABLE_SOUND

class Mix_Chunk;

class SDLSound
{
 public:
  SDLSound();
  ~SDLSound();

  void ring();
  void pm();

 private:

  Mix_Chunk *ring_sound;
  Mix_Chunk *pm_sound;

};

SDLSound& sound();

#endif

#endif // SPRINGLOBBY_HEADERGUARD_SDLSOUND_H
