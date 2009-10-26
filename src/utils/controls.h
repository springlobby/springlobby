#ifndef SPRINGLOBBY_HEADERGUARD_CONTROLS_H
#define SPRINGLOBBY_HEADERGUARD_CONTROLS_H

class wxWindow;
class wxColour;
class wxArrayString;

#include <wx/intl.h>

#ifdef __WXMSW__
    static const int CONTROL_HEIGHT = 22;
#else
    static const int CONTROL_HEIGHT = 28;
#endif

static const int SCROLL_RATE = 3;

#define bool2yn(b) ((b)?_("Yes"):_("No"))

#ifdef __WXMSW__
    const wxString DEFAULT_COLORDLG_TITLE = _("Choose color");
#else
    const wxString DEFAULT_COLORDLG_TITLE = _("Choose color (only first 16 will be saved)");
#endif


//! when querying for a color, always use this (it'll autosave/retrieve custom defined colors)
wxColour GetColourFromUser(wxWindow *parent, const wxColour& colInit,
        const wxString& caption = DEFAULT_COLORDLG_TITLE, const wxString& palette = _T("Default") );

#define TE(v) TooltipEnable(v)

/** @brief when setting tooltips use output of this func as input
    it outputs empty string if tooltips are disabled in SL settings
*/
const wxChar* TooltipEnable(const wxChar* input);

int GetMaxStringWidth( const wxWindow& win, const wxArrayString& strings );

#endif // SPRINGLOBBY_HEADERGUARD_CONTROLS_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

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
