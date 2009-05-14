#ifndef SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED
#define SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED

#include <wx/event.h>
class wxCommandEvent;

extern const wxEventType UnitSyncReloadRequest;
extern const wxEventType ExeDownloadEvt;

class IGlobalObjectHolder;
template <class T>
class GlobalObjectHolder;

class SL_GlobalEvtHandler : public wxEvtHandler
{
protected:
    friend class IGlobalObjectHolder;
    friend class GlobalObjectHolder<SL_GlobalEvtHandler>;
    SL_GlobalEvtHandler();

    DECLARE_EVENT_TABLE()

public:
    static SL_GlobalEvtHandler& GetSL_GlobalEvtHandler();
    void OnUnitSyncReload(wxCommandEvent& event);

};

#endif // SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED

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

