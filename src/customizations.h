#ifndef SPRINGLOBBY_HEADERGUARD_CUSTOMIZATIONS_H
#define SPRINGLOBBY_HEADERGUARD_CUSTOMIZATIONS_H

#include "globalsmanager.h"
#include "mmoptionswrapper.h"
#include <wx/string.h>
#include <wx/icon.h>
#include <wx/bitmap.h>

class Customizations {
    protected:
        Customizations();

        wxString m_modname;
        OptionsWrapper m_customs;
        wxIcon m_app_ico;
        wxBitmap m_background;
        wxString m_help_url;
    public:
        ~Customizations() {}

        bool Init( const wxString& modname );

        wxString GetModname();
        wxString GetHelpUrl();
        wxIcon GetAppIcon();
        wxBitmap GetBackground();
        wxSize GetBackgroundSize();
        const OptionsWrapper& GetCustomizations();

    friend class GlobalObjectHolder<Customizations, LineInfo<Customizations> >;
};

Customizations& SLcustomizations();

#endif // SPRINGLOBBY_HEADERGUARD_CUSTOMIZATIONS_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

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
