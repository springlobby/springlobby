/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

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
	wxTranslationHelper(const wxString& catalog, const wxString& search_path);
	~wxTranslationHelper();
	wxLocale* GetLocale();

	/** \brief put names and ID for all found .mo files
	\param current_selected will contain the index of currently in settings selected languageID
	        in name/identifiers array, or -1 if notfound
	*/
	bool AskUserForLanguage();

private:
	bool Load(int language);
	void GetInstalledLanguages(wxArrayString& names, wxArrayLong& identifiers);
	long GetLangID(const long index, const wxArrayLong& identifiers) const;
	wxString m_SearchPath;
	wxString catalogname;
	wxLocale* m_Locale;
};

#endif
