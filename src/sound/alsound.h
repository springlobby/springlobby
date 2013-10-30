#ifndef SPRINGLOBBY_HEADERGUARD_SDLSOUND_H
#define SPRINGLOBBY_HEADERGUARD_SDLSOUND_H

#ifndef DISABLE_SOUND
#ifdef __WXMAC__
#include <al.h>
#else
#include <AL/al.h>
#endif

class soundThread;
class ALsound
{
	public:
		ALsound();
		~ALsound();

		void ring();
		void pm();

	private:
		void Play(int idx);
		soundThread* m_thread;
};

ALsound& sound();

#endif

#endif // SPRINGLOBBY_HEADERGUARD_SDLSOUND_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/
