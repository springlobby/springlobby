/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_CONTROLS_H
#define SPRINGLOBBY_HEADERGUARD_CONTROLS_H

#include <wx/intl.h>

#ifdef __WXMSW__
static const int CONTROL_HEIGHT = 22;
#else
static const int CONTROL_HEIGHT = 28;
#endif

static const int SCROLL_RATE = 3;

//! call this function everytime map app loses/acquires focus to enable/disable tooltips
void UpdateMainAppHasFocus(bool focus);

#endif // SPRINGLOBBY_HEADERGUARD_CONTROLS_H
