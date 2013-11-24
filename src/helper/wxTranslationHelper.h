#ifndef _WX_TRANSLATION_HELPER_H
#define _WX_TRANSLATION_HELPER_H

//note: copied from http://wxwidgets.info/?q=wxTranslation
//      and modified to fit SL needs

#include <wx/string.h>

class wxApp;
class wxLocale;
class wxArrayString;
class wxArrayLong;

//! manages locale setting/finding with user interface
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
	bool AskUserForLanguage();

private:
	bool Load();
	void Save();
	void GetInstalledLanguages( wxArrayString& names, wxArrayLong& identifiers);
	long GetLangID(const long index, const wxArrayLong identifiers) const;
	wxString m_SearchPath;
	wxString catalogname;
	wxLocale * m_Locale;
	bool m_UseNativeConfig;

};

#endif

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

