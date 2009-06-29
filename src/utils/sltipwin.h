#ifndef SPRINGLOBBY_HEADERGUARD_UTIL_TIPWIN_H
#define SPRINGLOBBY_HEADERGUARD_UTIL_TIPWIN_H

#include <wx/platform.h>
#if wxUSE_TIPWINDOW
#include <wx/tipwin.h>

class SLTipWindow : public wxTipWindow{
    public:
        SLTipWindow(wxWindow *parent, const wxString &text);

        void Cancel(wxMouseEvent& event);

        DECLARE_EVENT_TABLE()
};

#endif

#endif // SPRINGLOBBY_HEADERGUARD_UTIL_TIPWIN_H

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
