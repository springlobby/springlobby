#include "helpmenufunctions.h"

#include <wx/icon.h>
#include "../images/springsettings.xpm"

#include "../utils/customdialogs.h"
#include "../utils/platform.h"
#include "../updater/updatehelper.h"
#include <wx/intl.h>
#include "se_utils.h"
#include <string>

#include <wx/aboutdlg.h>

void showAbout()
{
    wxAboutDialogInfo info;
	if ( IsSettingsStandAlone() )
	{
		wxString name ( GetAppName() );
		if ( GetAppName() != _T("SpringSettings") )
			name += _T(" (SpringSettings)");
		info.SetName(name);
		info.SetVersion(_T("0.2.2"));
	}
	else
	{
		info.SetName( GetAppName() );
		info.SetVersion(GetSpringLobbyVersion());
	}
    info.SetDescription(_("SpringSettings is a graphical frontend to the Settings of the Spring engine"));
	info.SetCopyright(_T("(C) 2007-2011 koshi <koshi@springlobby.info>"));
    info.SetIcon(wxIcon(springsettings_xpm));
    wxAboutBox(info);
}

void showCredits()
{
	CreditsDialog dlg(CustomMessageBoxBase::getSettingspointer(),_T("Credits"),SS_MAIN_ICON);
	dlg.AddCredit(_("koshi"),_T("Principal Author"));
	dlg.AddCredit(_("Very Bad Soldier"),_T("wrote the excellent hotkey editor"));
	dlg.AddCredit(_("Kloot"),_T("wrote Settings++ from which SpringSettings originated"));
	dlg.AddCredit(_("The SpringLobby team"),_T(""));
	dlg.AddCredit(_("everyone reporting bugs/suggestions"),_T(""));
	dlg.ShowModal();
}

void openNewTicket()
{
	openUrl(_T("https://github.com/springlobby/springlobby/issues/new"));
}

void openContactPage()
{
	openUrl(_T("https://github.com/springlobby?tab=members"));
}
