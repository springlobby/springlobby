#ifndef SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H
#define SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H

#include <wx/aui/aui.h>

class AuiManagerContainer
{
  public:
    AuiManagerContainer() {}

    wxAuiManager* manager;

    static AuiManagerContainer& AuiMngr()
    {
        static AuiManagerContainer m_aui_mngr;
        return m_aui_mngr;
    }

};



#endif // SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H
