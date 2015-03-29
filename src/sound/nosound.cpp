/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "alsound.h"

ALsound::ALsound()
{
}

ALsound::~ALsound()
{
}

void ALsound::ring()
{
}
void ALsound::pm()
{
}

ALsound& slsound()
{
	static ALsound snd;
	return snd;
}
