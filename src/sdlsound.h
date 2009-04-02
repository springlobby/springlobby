#ifndef SPRINGLOBBY_HEADERGUARD_SDLSOUND_H
#define SPRINGLOBBY_HEADERGUARD_SDLSOUND_H

#ifndef DISABLE_SOUND

//struct Mix_Chunk;
#include <SDL_mixer.h>

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

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

