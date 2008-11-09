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
	void GetInstalledLanguages( wxArrayString& names, wxArrayLong& identifiers );
	bool AskUserForLanguage( wxArrayString& names, wxArrayLong& identifiers );
	bool Load();
	void Save();

	const wxString & GetSearchPath();
	void SetSearchPath( wxString& value );

private:
    wxApp& m_App;
	wxString m_SearchPath;
	wxLocale * m_Locale;
	bool m_UseNativeConfig;

};

#endif
