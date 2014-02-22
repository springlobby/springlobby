/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
#define SPRINGLOBBY_HEADERGUARD_AUTOHOST_H

//including this header is only really needed for time_t ..
#include <wx/string.h>
#include <wx/arrstr.h>

class Battle;
class User;
class wxString;

//! @brief Autohost logic
class AutoHost
{
  public:

    AutoHost( Battle& battle );

    void SetEnabled( const bool enabled );
    bool GetEnabled() const;

    void OnSaidBattle( const wxString& nick, const wxString& msg );
    void OnUserAdded( User& user );
    void OnUserRemoved( User& user );
  private:

    void StartBattle();

    Battle& m_battle;

    bool m_enabled;
    time_t m_lastActionTime;
    wxArrayString m_userlist;
};

#endif // SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
