/* Author: Tobi Vollebregt */

#ifndef SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
#define SPRINGLOBBY_HEADERGUARD_AUTOHOST_H

#include <wx/string.h>

class Battle;

//! @brief Autohost logic
class AutoHost
{
  public:

    AutoHost() : m_lastActionTime(0) {}

    void OnSaidBattle( Battle& battle, const wxString& nick, const wxString& msg );

  private:

    void StartBattle( Battle& battle );

    time_t m_lastActionTime;
};

#endif // SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
