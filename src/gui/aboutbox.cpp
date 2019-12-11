/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "aboutbox.h"

#include <lslutils/globalsmanager.h>
#include <wx/aboutdlg.h>

#include "iconimagelist.h"
#include "uiutils.h"
#include "utils/conversion.h"
#include "utils/version.h"

slAboutBox::slAboutBox()
{
	info = new wxAboutDialogInfo();

	info->SetName(TowxString(GetSpringlobbyName()));
	info->SetVersion(TowxString(GetSpringlobbyVersion()));
	info->SetDescription(TowxString(GetSpringlobbyName()) + _(" is a cross-platform lobby client for the Spring RTS engine"));
	//info.SetCopyright(wxEmptyString;
	info->SetWebSite(_T("https://springlobby.springrts.com"));
	info->SetLicence(_T("GPL v2 or later"));
	info->AddDeveloper(_T("abma"));
	info->AddDeveloper(_T("BrainDamage"));
	info->AddDeveloper(_T("dizekat"));
	info->AddDeveloper(_T("insaneinside"));
	info->AddDeveloper(_T("Kaot"));
	info->AddDeveloper(_T("koshi"));
	info->AddDeveloper(_T("semi_"));
	info->AddDeveloper(_T("tc-"));
	info->AddTranslator(_T("chaosch (simplified chinese)"));
	info->AddTranslator(_T("lejocelyn (french)"));
	info->AddTranslator(_T("Suprano (german)"));
	info->AddTranslator(_T("tc- (swedish)"));
	info->AddTranslator(_("The numerous contributors from launchpad.net"));
}

slAboutBox::~slAboutBox()
{
	wxDELETE(info);
}

void slAboutBox::Show()
{
	wxAboutBox(*info);
}

slAboutBox& aboutbox()
{
	static LSL::Util::LineInfo<slAboutBox> m(AT);
	static LSL::Util::GlobalObjectHolder<slAboutBox, LSL::Util::LineInfo<slAboutBox> > m_sett(m);
	return m_sett;
}

void slAboutBox::openNewTicket()
{
	OpenWebBrowser(_T("https://github.com/springlobby/springlobby/issues/new"));
}

void slAboutBox::openContactPage()
{
	OpenWebBrowser(_T("https://github.com/springlobby?tab=members"));
}

void slAboutBox::showDocs()
{
	OpenWebBrowser(_T("https://github.com/springlobby/springlobby/wiki/"));
}

void slAboutBox::showChangelog()
{
	OpenWebBrowser(_T("https://github.com/springlobby/springlobby/blob/master/ChangeLog"));
}
