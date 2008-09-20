/* Author: Tobi Vollebregt */

#ifndef SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
#define SPRINGLOBBY_HEADERGUARD_AUTOHOST_H

#include <wx/string.h>

class Battle;

//! @brief Autohost logic
class AutoHost
{
  public:

    AutoHost( Battle& battle );

    void SetEnabled( const bool enabled );

    void OnSaidBattle( const wxString& nick, const wxString& msg );

  private:

    void StartBattle();

    Battle& m_battle;

    bool m_enabled;
    time_t m_lastActionTime;
};

#endif // SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
