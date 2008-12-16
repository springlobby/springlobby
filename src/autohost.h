/* Author: Tobi Vollebregt */

#ifndef SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
#define SPRINGLOBBY_HEADERGUARD_AUTOHOST_H

//including this header is only really needed for time_t ..
#include <wx/string.h>

class Battle;
class User;
class wxString;

//! @brief Autohost logic
class AutoHost
{
  public:

    AutoHost( Battle& battle );

    void SetEnabled( const bool enabled );
    bool GetEnabled();

    void OnSaidBattle( const wxString& nick, const wxString& msg );
    void OnUserAdded( User& user );
    void OnUserRemoved( User& user );
  private:

    void StartBattle();

    Battle& m_battle;

    bool m_enabled;
    time_t m_lastActionTime;
};

#endif // SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
