#include "helpmenufunctions.h"
#include <wx/aboutdlg.h>
#include "../images/springsettings.xpm"
#include <wx/icon.h>
#include "../utils.h"

void showAbout()
{
#ifdef HAVE_WX28
	 wxAboutDialogInfo info;
	 info.SetName(_T("SpringSettings"));
	 info.SetVersion(_T("0.1"));
	 info.SetDescription(_T("SpringSettings is a graphical frontend to the options of the spring engine"));
	 info.SetCopyright(_T("(C) 2007 koshi <koshi@springlobby.info>"));
	 info.SetIcon(wxIcon(springsettings_xpm));
	 wxAboutBox(info);

#endif
}
