#include "tasclientimport.h"

#ifdef __WXMSW__

#include "../settings.h"

#include <wx/msw/registry.h>

bool ImportTASClientSettings()
{
    wxRegKey base( _T("HKEY_CURRENT_USER\\Software\\TASClient\\Preferences") );

}

bool TASClientPresent()
{
    wxRegKey base( _T("HKEY_CURRENT_USER\\Software\\TASClient\\Preferences") );
    return base.Exists();
}
#endif
