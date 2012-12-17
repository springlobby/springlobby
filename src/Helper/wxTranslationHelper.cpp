#include "wxTranslationHelper.h"
#include <wx/dir.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/choicdlg.h>
#include <wx/intl.h>
#include "../settings.h"
#include "../utils/customdialogs.h"
#include "../utils/platform.h"

wxTranslationHelper::wxTranslationHelper( wxApp & app, const wxString & search_path ) :
	m_App(app),
	m_SearchPath(search_path),
	m_Locale(NULL),
	m_UseNativeConfig(false)
{
	if(search_path.IsEmpty())
	{
		m_SearchPath = wxPathOnly(m_App.argv[0]);
	}
}

wxTranslationHelper::~wxTranslationHelper()
{
	if(m_Locale)
	{
		Save();
		wxDELETE(m_Locale);
	}
}

wxLocale * wxTranslationHelper::GetLocale()
{
	return m_Locale;
}

const wxString & wxTranslationHelper::GetSearchPath() const
{
	return m_SearchPath;
}

void wxTranslationHelper::SetSearchPath( const wxString& value )
{
	m_SearchPath = value;
	if( m_SearchPath.IsEmpty() )
	{
		m_SearchPath = wxPathOnly( m_App.argv[0] );
	}
}

bool wxTranslationHelper::Load()
{
	long language = sett().GetLanguageID();
	if(language == wxLANGUAGE_UNKNOWN)
	{
		return false;
	}

	wxArrayString names;
	wxArrayLong identifiers;
	int dummy;
	GetInstalledLanguages( names, identifiers, dummy );
	for(size_t i = 0; i < identifiers.Count(); i++)
	{
		if( identifiers[i] == language )
		{
			if(m_Locale) wxDELETE( m_Locale );
			m_Locale = new wxLocale;
			m_Locale->Init( identifiers[i] );
			m_Locale->AddCatalogLookupPathPrefix( m_SearchPath );
			m_Locale->AddCatalog( m_App.GetAppName() );
			m_Locale->AddCatalog( _T("wxstd") );
			return true;
		}
	}
	return false;
}

void wxTranslationHelper::Save()
{
    sett().SetLanguageID( m_Locale->GetLanguage() );
    sett().SaveSettings();
}

void wxTranslationHelper::GetInstalledLanguages( wxArrayString & names,
												 wxArrayLong & identifiers,
												 int& selected_index )
{
	names.Clear();
	identifiers.Clear();
	wxString filename;
	const wxLanguageInfo * langinfo;
	wxString name = wxLocale::GetLanguageName( wxLANGUAGE_DEFAULT );
	if(!name.IsEmpty())
	{
		names.Add( _("Default") );
		identifiers.Add( wxLANGUAGE_DEFAULT );
	}
	if( !wxDir::Exists( m_SearchPath ) )
	{
		wxLogError( _T("Directory %s DOES NOT EXIST !!!"),
                    m_SearchPath.GetData() );
		return;
	}
	wxDir dir( m_SearchPath );

#ifdef __WXMSW__
	wxString mask = wxT("*.*");
#else
	wxString mask = wxT("*");
#endif

    selected_index = -1;
	for(bool cont = dir.GetFirst(&filename, mask, wxDIR_DEFAULT);
            cont; cont = dir.GetNext( &filename) )
	{
		langinfo = wxLocale::FindLanguageInfo(filename);
		if(langinfo != NULL)
		{
		    wxString mo_file = dir.GetName() +
                            wxFileName::GetPathSeparator() +
                            filename +
                            wxFileName::GetPathSeparator() +
                            _T("LC_MESSAGES") +
                            wxFileName::GetPathSeparator() +
                            m_App.GetAppName().Lower() + wxT(".mo") ;
			wxLogInfo( _("SEARCHING FOR %s"), mo_file.GetData() );
			if( wxFileExists( mo_file ) )
			{
				names.Add(langinfo->Description);
				identifiers.Add(langinfo->Language);
				if ( langinfo->Language == sett().GetLanguageID() )
                    selected_index = names.GetCount() -1;
			}
		}
	}
}

bool wxTranslationHelper::AskUserForLanguage( wxArrayString& names,
											  wxArrayLong& identifiers,
											  int selected_index)
{
	wxCHECK_MSG( names.Count() == identifiers.Count(), false,
		_("Array of language names and identifiers should have the same size.") );
	long index = GetSingleChoiceIndex( _("Select the language"),
			_("Language"), names, selected_index );
	if( index != -1 )
	{
		if( m_Locale )
		{
			wxDELETE( m_Locale );
		}
		m_Locale = new wxLocale;
		m_Locale->Init( identifiers[index] );
		m_Locale->AddCatalogLookupPathPrefix( m_SearchPath );
		wxLogInfo( _("wxTranslationHelper: Path Prefix = \"%s\""),
			m_SearchPath.GetData() );
		m_Locale->AddCatalog( GetAppName( true ) );
		wxLogInfo( _("wxTranslationHelper: Catalog Name = \"%s\""),
			GetAppName( true ).c_str() );
		return true;
	}
	return false;
}
