#include "helpmenufunctions.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <wx/icon.h>
#include "../images/springsettings.xpm"

#include "custom_dialogs.h"
#include <wx/intl.h>
#include "se_utils.h"
#include <string>

#include <wx/aboutdlg.h>

void showAbout()
{
    std::string ver = std::string("0.2.1 revision ") + VERSION;
    wxAboutDialogInfo info;
    info.SetName(_T("SpringSettings"));
    info.SetVersion(_T("0.2.1"));//WX_STRING(ver));
    info.SetDescription(_("SpringSettings is a graphical frontend to the Settings of the Spring engine"));
    info.SetCopyright(_T("(C) 2007-2008 koshi <koshi@springlobby.info>"));
    info.SetIcon(wxIcon(springsettings_xpm));
    wxAboutBox(info);
}

void showCredits()
{
	CreditsDialog dlg(CustomMessageBoxBase::getSettingspointer(),_T("Credits"),SS_MAIN_ICON);
	dlg.AddCredit(_("Kloot"),_T("wrote Settings++ from which SpringSettings originated"));
	dlg.AddCredit(_("The SpringLobby team"),_("thanks for inviting me in, code to re-use, infrastructure and help in general"));
	dlg.AddCredit(_("everyone reporting bugs/suggestions"),_T(""));
	dlg.ShowModal();
}

void openNewTicket()
{
	openUrl(_T("http://trac.springlobby.info/newticket?component=springsettings%2Fsettings%2B%2B&owner=koshi"));
}

void openContactPage()
{
	openUrl(_T("http://trac.springlobby.info/wiki/Contact"));
}
