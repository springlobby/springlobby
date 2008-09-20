/* Author: Tobi Vollebregt */

#ifndef SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
#define SPRINGLOBBY_HEADERGUARD_AUTOHOST_H

#include <wx/string.h>

class Battle;

//! @brief Autohost logic
class AutoHost
{
  public:

    void OnSaidBattle( Battle& battle, const wxString& nick, const wxString& msg );

  private:

    void StartBattle( Battle& battle );
};

#endif // SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
