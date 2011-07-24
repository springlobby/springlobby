#ifndef SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H
#define SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H

#include <wx/aui/aui.h>

class wxAuiManagerDupe {
    public:
        wxAuiManagerDupe(wxWindow* /*managed_wnd*/ = NULL, unsigned int /*flags*/ = 0 ) {}
        bool AddPane(wxWindow* /*window*/, int /*direction*/ = wxLEFT, const wxString& /*caption */= wxEmptyString) const { return false; }
        bool DetachPane(wxWindow* /*window*/) const { return false; }
        void UnInit() const {}
        wxString SavePerspective() const { return wxEmptyString; }
        bool LoadPerspective(const wxString& /*perspective*/, bool /*update*/ = true) const { return false; }
};

class AuiManagerContainer
{
  public:
    AuiManagerContainer() : manager(NULL) {}

    typedef wxAuiManager
        ManagerType;

    ManagerType* manager;
};

AuiManagerContainer& GetAui();



#endif // SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H

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

