#ifndef SPRINGLOBBY_HEADERGUARD_SDLSOUND_H
#define SPRINGLOBBY_HEADERGUARD_SDLSOUND_H

#ifndef DISABLE_SOUND
#ifdef __WXMAC__
#include <al.h>
#else
#include <AL/al.h>
#endif
class ALsound
{
	public:
		ALsound();
		~ALsound();

		void ring() const;
		void pm() const;

	private:
		static const int m_num_buffer = 2;
		static const int m_num_sources = 2;
		ALuint m_sources[m_num_sources];
		ALuint m_buffers[m_num_buffer];
};

ALsound& sound();

#endif

#endif // SPRINGLOBBY_HEADERGUARD_SDLSOUND_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/
