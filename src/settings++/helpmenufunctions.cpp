#include "helpmenufunctions.h"

#include "../images/springsettings.xpm"
#include <wx/icon.h>
#include "../utils.h"
#include "custom_dialogs.h"
#include <wx/string.h>
#include "se_utils.h"
#include "../revision.h"
#include <string>

#ifdef HAVE_WX28
#include <wx/aboutdlg.h>
#endif

void showAbout()
{
#ifdef HAVE_WX28

	std::string ver = std::string("0.1 revision ") + revision();
	 wxAboutDialogInfo info;
	 info.SetName(_T("SpringSettings"));
	 info.SetVersion(_T("0.1"));//WX_STRING(ver));
	 info.SetDescription(_T("SpringSettings is a graphical frontend to the options of the spring engine"));
	 info.SetCopyright(_T("(C) 2007 koshi <koshi@springlobby.info>"));
	 info.SetIcon(wxIcon(springsettings_xpm));
	 wxAboutBox(info);

#endif
}

void showCredits()
{
	CreditsDialog dlg(CustomMessageBox::getSettingspointer(),_T("Credits"),SS_MAIN_ICON);
	dlg.AddCredit(_T("IUB"),_T("BKJB"));
	dlg.AddCredit(_T("IUB"),_T("BKJB"));
	dlg.ShowModal();
}

void openNewTicket()
{
	openUrl(_T("http://trac.springlobby.info/newticket?component=springsettings%2Fsettings%2B%2B&owner=koshi"));
}
//springsettings/settings++

void openContactPage()
{
	openUrl(_T("http://trac.springlobby.info/wiki/Contact"));
}
