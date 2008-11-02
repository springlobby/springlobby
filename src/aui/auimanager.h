#ifndef SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H
#define SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H

#include <wx/aui/aui.h>

class AuiManagerContainer
{
  public:
    AuiManagerContainer() {}

    wxAuiManager* manager;

};

AuiManagerContainer& GetAui();



#endif // SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H
