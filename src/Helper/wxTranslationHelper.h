#ifndef _WX_TRANSLATION_HELPER_H
#define _WX_TRANSLATION_HELPER_H

//note: copied from http://wxwidgets.info/?q=wxTranslation
//      and modified to fit SL needs

#include <wx/string.h>

class wxApp;
class wxLocale;
class wxArrayString;
class wxArrayLong;

class wxTranslationHelper
{

public:
	wxTranslationHelper( wxApp & app, const wxString & search_path );
	~wxTranslationHelper();
	wxLocale * GetLocale();

	/** \brief put names and ID for all found .mo files
        \param current_selected will contain the index of currently in settings selected languageID
                in name/identifiers array, or -1 if notfound
    */
	void GetInstalledLanguages( wxArrayString& names, wxArrayLong& identifiers, int& current_selected );
	bool AskUserForLanguage( wxArrayString& names, wxArrayLong& identifiers, int selected_index );
	bool Load();
	void Save();

	const wxString & GetSearchPath();
	void SetSearchPath( wxString& value );

private:
    wxApp& m_App;
	wxString m_SearchPath;
	wxLocale * m_Locale;
	bool m_UseNativeConfig;

    wxTranslationHelper( const wxTranslationHelper& );
};

#endif

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

