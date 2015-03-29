/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SDLSOUND_H
#define SPRINGLOBBY_HEADERGUARD_SDLSOUND_H

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

ALsound& slsound();

#endif // SPRINGLOBBY_HEADERGUARD_SDLSOUND_H
