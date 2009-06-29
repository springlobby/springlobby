#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/utils.h>
#include <wx/log.h>
#include <wx/intl.h>

#include <string>
#include <sstream>

#include "se_utils.h"

#include "custom_dialogs.h"
#include "../settings.h"
#include "../springunitsynclib.h"
//#include "../utils/.h"

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
      wxString untisyncpath;
      #ifdef __WXMSW_
      if ( IsSettingsStandAlone() )
      {
        unitsyncpath = GetExecutableFolder() + wxFileName::GetPathSeparator() + _T("unitsync") + GetLibExtension();
      }
        else
            untisyncpath = sett().GetCurrentUsedUnitSync();
			#else
			untisyncpath = sett().GetCurrentUsedUnitSync();
			#endif
      susynclib().Load( untisyncpath, sett().GetForcedSpringConfigFilePath() );
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

