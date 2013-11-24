#include "wxTranslationHelper.h"
#include <wx/dir.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/choicdlg.h>
#include <wx/intl.h>
#include "helper/slconfig.h"
#include "utils/customdialogs.h"
#include "utils/platform.h"

#include "utils/conversion.h"

SLCONFIG("/General/LanguageID", (long)wxLANGUAGE_DEFAULT, "Language ID" );

wxTranslationHelper::wxTranslationHelper( wxApp & app, const wxString & search_path ) :
	m_SearchPath(search_path),
	catalogname(app.GetAppName().Lower()),
	m_Locale(NULL)
{
	if(search_path.IsEmpty()) {
		m_SearchPath = wxPathOnly(app.argv[0]);
	}
	Load();
}

wxTranslationHelper::~wxTranslationHelper()
{
	if(m_Locale) {
		cfg().Write(_T( "/General/LanguageID" ) , m_Locale->GetLanguage());
		wxDELETE(m_Locale);
	}
}

wxLocale * wxTranslationHelper::GetLocale()
{
	return m_Locale;
}

bool wxTranslationHelper::Load()
{
	long language = cfg().ReadLong(_T("/General/LanguageID"));

	wxArrayString names;
	wxArrayLong identifiers;
	GetInstalledLanguages( names, identifiers);
	for(size_t i = 0; i < identifiers.Count(); i++)
	{
		if( identifiers[i] == language )
		{
			if(m_Locale) wxDELETE( m_Locale );
			m_Locale = new wxLocale;
			m_Locale->Init( identifiers[i] );
			m_Locale->AddCatalogLookupPathPrefix( m_SearchPath );
			m_Locale->AddCatalog( catalogname );
			m_Locale->AddCatalog( _T("wxstd") );
			return true;
		}
	}
	return false;
}

void wxTranslationHelper::GetInstalledLanguages( wxArrayString & names, wxArrayLong & identifiers)
{
	names.Clear();
	identifiers.Clear();
	wxString filename;
	const wxLanguageInfo * langinfo;

	names.Add( _("English") );
	identifiers.Add( wxLANGUAGE_ENGLISH );

	if( !wxDir::Exists( m_SearchPath ) ) {
		wxLogError( _T("Directory %s DOES NOT EXIST"), m_SearchPath.GetData() );
		return;
	}
	wxDir dir( m_SearchPath );

#ifdef __WXMSW__
	wxString mask = wxT("*.*");
#else
	wxString mask = wxT("*");
#endif

	int localeid = wxLocale::GetSystemLanguage();

	for(bool cont = dir.GetFirst(&filename, mask, wxDIR_DEFAULT); cont; cont = dir.GetNext( &filename) ) {
		langinfo = wxLocale::FindLanguageInfo(filename);
		if (langinfo != NULL) {
			wxString mo_file = dir.GetName() + wxFileName::GetPathSeparator() + filename + wxFileName::GetPathSeparator() + _T("LC_MESSAGES") + wxFileName::GetPathSeparator() + catalogname + wxT(".mo") ;
			wxLogInfo( _("SEARCHING FOR %s"), mo_file.GetData() );
			if( wxFileExists( mo_file ) ) {
				if (langinfo->Language == localeid)
					names.Add(langinfo->Description + _(" (Default)"));
				else
					names.Add(langinfo->Description);
				identifiers.Add(langinfo->Language);
			}
		}
	}
}

long wxTranslationHelper::GetLangID(const long index, const wxArrayLong identifiers) const
{
	int j=0;
	for(auto i: identifiers) {
		if (index == i) {
			return j;
		}
		j++;
	}
	return -1;
}

bool wxTranslationHelper::AskUserForLanguage()
{
	wxArrayString names;
	wxArrayLong identifiers;

	GetInstalledLanguages(names, identifiers);
	const long cfgidx = cfg().ReadLong(_T("/General/LanguageID"));

	assert(names.Count() == identifiers.Count());
	long index = GetSingleChoiceIndex( _("Select the language"), _("Language"), names, GetLangID(cfgidx, identifiers));
	if( index != -1 ) {
		if( m_Locale ) {
			wxDELETE( m_Locale );
		}
		m_Locale = new wxLocale;
		m_Locale->Init( identifiers[index] );
		m_Locale->AddCatalogLookupPathPrefix( m_SearchPath );
		wxLogInfo( _("wxTranslationHelper: Path Prefix = \"%s\""), m_SearchPath.GetData() );
		m_Locale->AddCatalog( GetAppName( true ) );
		wxLogInfo( _("wxTranslationHelper: Catalog Name = \"%s\""), GetAppName( true ).c_str() );
		return true;
	}
	return false;
}
