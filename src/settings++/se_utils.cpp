#include "se_utils.h"

#include <string>
#include <sstream>
#include <wx/intl.h>
#include "custom_dialogs.h"
#include <wx/utils.h>
#include <wx/log.h>
#include "se_settings.h"
#include "../springunitsynclib.h"

void toString(wxString& s, int t) {

}

int fromString(const wxString& s) {
        long temp = 0;
        s.ToLong(&temp);
        return int(temp);
}

void loadUnitsync()
{
	//should be done in susynclib()->Load
	//wxSetWorkingDirectory(OptionsHandler.getUsyncLoc().BeforeLast('\\'));

	try
	{
		wxCriticalSection m_lock;
		wxCriticalSectionLocker lock_criticalsection(m_lock);
		susynclib()->Load(OptionsHandler.getUsyncLoc());
	}
	catch (...)
	{
		//TODO log that
	}
}

void openUrl(const wxString& url)
{
    if ( !wxLaunchDefaultBrowser( url ) )
    {
      wxLogWarning( _("can't launch default browser") );
      customMessageBox(SL_MAIN_ICON, _("Couldn't launch browser. URL is: ") + url, _("Couldn't launch browser.")  );
    }
}

