#include "se_utils.h"

#include <string>
#include <sstream>
#include <wx/intl.h>
#include "custom_dialogs.h"
#include <wx/utils.h>
#include <wx/log.h>
#include "../settings.h"
#include "../springunitsynclib.h"

static bool standalonemode;

bool IsSettingsStandAlone()
{
  return standalonemode;
}

void SetSettingsStandAlone( bool value )
{
  standalonemode = value;
}


int fromString(const wxString& s) {
        long temp = 0;
        s.ToLong(&temp);
        return int(temp);
}

void loadUnitsync()
{
  try
  {
      wxCriticalSection m_lock;
      wxCriticalSectionLocker lock_criticalsection(m_lock);
      susynclib().Load(sett().GetCurrentUsedUnitSync());
  }
  catch (...)
  {
      wxLogError( _T("springsettings: couldn't load unitsync") );
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

