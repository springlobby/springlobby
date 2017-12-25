/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "wxTranslationHelper.h"
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/intl.h>

#include "utils/slconfig.h"
#include "utils/conversion.h"
#include "gui/customdialogs.h"

SLCONFIG("/General/LanguageID", (long)wxLANGUAGE_DEFAULT, "Language ID");

wxTranslationHelper::wxTranslationHelper(const wxString& catalog, const wxString& search_path)
    : m_SearchPath(search_path)
    , catalogname(catalog)
    , m_Locale(NULL)
{
	wxLogWarning(_T("Using LocalePath %s"), m_SearchPath.c_str());
	long language = cfg().ReadLong(_T("/General/LanguageID"));
	if (!Load(language)) { //fallback when something went wrong
		wxLogWarning("Couldn't load locale %ld, fallback to default", language);
		int localeid = wxLocale::GetSystemLanguage();
		Load(localeid);
	}
}

wxTranslationHelper::~wxTranslationHelper()
{
	if (m_Locale) {
		wxDELETE(m_Locale);
	}
}

wxLocale* wxTranslationHelper::GetLocale()
{
	return m_Locale;
}

bool wxTranslationHelper::Load(int language)
{

	wxArrayString names;
	wxArrayLong identifiers;
	GetInstalledLanguages(names, identifiers);
	for (size_t i = 0; i < identifiers.Count(); i++) {
		if (identifiers[i] == language) {
			if (m_Locale) {
				wxDELETE(m_Locale);
			}
			m_Locale = new wxLocale(identifiers[i]);
			//bool res = m_Locale->Init(identifiers[i]);
			m_Locale->AddCatalogLookupPathPrefix(m_SearchPath);
			m_Locale->AddCatalog(catalogname);
			//m_Locale->AddCatalog(_T("wxstd"));
			wxLogWarning(wxString::Format(_("Loaded locale %s %d %s %s"), m_Locale->GetName().c_str(), m_Locale->IsOk(), m_SearchPath.c_str(), m_Locale->GetCanonicalName().c_str()));
			return m_Locale->IsOk();
		}
	}
	return false;
}

void wxTranslationHelper::GetInstalledLanguages(wxArrayString& names, wxArrayLong& identifiers)
{
	names.Clear();
	identifiers.Clear();
	wxString filename;
	const wxLanguageInfo* langinfo;

	names.Add(_("English"));
	identifiers.Add(wxLANGUAGE_ENGLISH);

	if (!wxDir::Exists(m_SearchPath)) {
		wxLogError(_T("Directory %s DOES NOT EXIST"), m_SearchPath.GetData());
		return;
	}
	wxDir dir(m_SearchPath);

	int localeid = wxLocale::GetSystemLanguage();

	for (bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS); cont; cont = dir.GetNext(&filename)) {
		langinfo = wxLocale::FindLanguageInfo(filename);
		if (langinfo == nullptr)
			continue;
		wxString mo_file = dir.GetName() + wxFileName::GetPathSeparator() + filename + wxFileName::GetPathSeparator() + _T("LC_MESSAGES") + wxFileName::GetPathSeparator() + catalogname + wxT(".mo");
		wxLogInfo(_("SEARCHING FOR %s"), mo_file.GetData());
		if (!wxFileExists(mo_file))
			continue;
		if (langinfo->Language == localeid)
			names.Add(langinfo->Description + _(" (Default)"));
		else
			names.Add(langinfo->Description);
		identifiers.Add(langinfo->Language);
	}
}

long wxTranslationHelper::GetLangID(const long index, const wxArrayLong& identifiers) const
{
	int j = 0;
	for (auto i : identifiers) {
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
	long index = GetSingleChoiceIndex(_("Select the language"), _("Language"), names, GetLangID(cfgidx, identifiers));
	if (index != -1) {
		int lang = identifiers[index];
		if (Load(lang)) {
			cfg().Write(_T( "/General/LanguageID" ), lang);
			cfg().SaveFile(); //instant save to config
			return true;
		}
	}
	return false;
}
