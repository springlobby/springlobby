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

wxTranslationHelper::wxTranslationHelper(wxApp & app,
										 const wxString & search_path,
										 bool use_native_config)
: m_App(app), m_SearchPath(search_path),
m_ConfigPath(wxEmptyString), m_Locale(NULL), m_UseNativeConfig(use_native_config)
{
	if(search_path.IsEmpty())
	{
		m_SearchPath = wxPathOnly(m_App.argv[0]);
	}
}

wxTranslationHelper::~wxTranslationHelper()
{
	Save();
	if(m_Locale)
	{
		wxDELETE(m_Locale);
	}
}

wxLocale * wxTranslationHelper::GetLocale()
{
	return m_Locale;
}

const wxString & wxTranslationHelper::GetSearchPath()
{
	return m_SearchPath;
}

void wxTranslationHelper::SetSearchPath(wxString & value)
{
	m_SearchPath = value;
	if(m_SearchPath.IsEmpty())
	{
		m_SearchPath = wxPathOnly(m_App.argv[0]);
	}
}

const wxString & wxTranslationHelper::GetConfigPath()
{
	return m_ConfigPath;
}

void wxTranslationHelper::SetConfigPath(wxString & value)
{
	m_ConfigPath = value;
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
	GetInstalledLanguages(names, identifiers);
	for(size_t i = 0; i < identifiers.Count(); i++)
	{
		if(identifiers[i] == language)
		{
			if(m_Locale) wxDELETE(m_Locale);
			m_Locale = new wxLocale;
			m_Locale->Init(identifiers[i]);
			m_Locale->AddCatalogLookupPathPrefix(m_SearchPath);
			m_Locale->AddCatalog(m_App.GetAppName());
			return true;
		}
	}
	return false;
}

void wxTranslationHelper::Save()
{
    sett().SetLanguageID( m_Locale->GetLanguage() );
}

void wxTranslationHelper::GetInstalledLanguages(wxArrayString & names,
												wxArrayLong & identifiers)
{
	names.Clear();
	identifiers.Clear();
	wxString filename;
	const wxLanguageInfo * langinfo;
	wxString name = wxLocale::GetLanguageName(wxLANGUAGE_DEFAULT);
	if(!name.IsEmpty())
	{
		names.Add(_("Default"));
		identifiers.Add(wxLANGUAGE_DEFAULT);
	}
	if(!wxDir::Exists(m_SearchPath))
	{
		wxLogTrace(wxTraceMask(), _("Directory %s DOES NOT EXIST !!!"),
			m_SearchPath.GetData());
		return;
	}
	wxDir dir(m_SearchPath);
	for(bool cont = dir.GetFirst(&filename,
#ifdef __WXMSW__
	wxT("*.*"),
#else
	wxT("*"),
#endif
	wxDIR_DEFAULT);
		cont; cont = dir.GetNext(&filename))
	{
		langinfo = wxLocale::FindLanguageInfo(filename);
		if(langinfo != NULL)
		{
			wxLogTrace(wxTraceMask(), _("SEARCHING FOR %s"),
				wxString(dir.GetName()+wxFileName::GetPathSeparator()+
				filename+wxFileName::GetPathSeparator()+
				m_App.GetAppName()+wxT(".mo")).GetData());
			if(wxFileExists(dir.GetName()+wxFileName::GetPathSeparator()+
				filename+wxFileName::GetPathSeparator()+
				m_App.GetAppName()+wxT(".mo")))
			{
				names.Add(langinfo->Description);
				identifiers.Add(langinfo->Language);
			}
		}
	}
}

bool wxTranslationHelper::AskUserForLanguage(wxArrayString & names,
											 wxArrayLong & identifiers)
{
	wxCHECK_MSG(names.Count() == identifiers.Count(), false,
		_("Array of language names and identifiers should have the same size."));
	long index = wxGetSingleChoiceIndex(_("Select the language"),
			_("Language"), names);
	if(index != -1)
	{
		if(m_Locale)
		{
			wxDELETE(m_Locale);
		}
		m_Locale = new wxLocale;
		m_Locale->Init(identifiers[index]);
		m_Locale->AddCatalogLookupPathPrefix(m_SearchPath);
		wxLogTrace(wxTraceMask(),
			_("wxTranslationHelper: Path Prefix = \"%s\""),
			m_SearchPath.GetData());
		m_Locale->AddCatalog(m_App.GetAppName());
		wxLogTrace(wxTraceMask(),
			_("wxTranslationHelper: Catalog Name = \"%s\""),
			m_App.GetAppName().GetData());
		return true;
	}
	return false;
}
