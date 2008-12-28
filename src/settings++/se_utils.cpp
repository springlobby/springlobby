#include "se_utils.h"

#include <string>
#include <sstream>
#include <wx/intl.h>
#include "custom_dialogs.h"
#include <wx/utils.h>
#include <wx/log.h>
#include "../settings.h"
#include "../springunitsynclib.h"

static bool standalonemode = true;

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
      wxString unitsyncpath;
      if ( IsSettingsStandAlone() )
      {
      	bool portable_mode = sett().IsPortableMode();
      	sett().SetPortableMode( true ); // force portable mode to get untisync path in current bin dir
        untisyncpath = sett().GetCurrentUsedUnitSync();
        sett().SetPortableMode( portable_mode ); // restore old value
      }
			else unitsyncpath = sett().GetCurrentUsedUnitSync();
      susynclib().Load( unitsyncpath, sett().GetForcedSpringConfigFilePath() );
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

