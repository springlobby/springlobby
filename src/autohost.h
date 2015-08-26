/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
#define SPRINGLOBBY_HEADERGUARD_AUTOHOST_H

#include <time.h>
#include <wx/arrstr.h>
#include <string>

class IBattle;
class User;
class wxString;

//! @brief Autohost logic
class AutoHost
{
public:
	AutoHost(IBattle& battle);

	void SetEnabled(const bool enabled);
	bool GetEnabled() const;

	void OnSaidBattle(const wxString& nick, const wxString& msg);
	void OnUserAdded(User& user);
	void OnUserRemoved(User& user);

private:
	void StartBattle();
	void DoAction(const std::string& str);

	IBattle& m_battle;

	bool m_enabled;
	time_t m_lastActionTime;
	wxArrayString m_userlist;
};

#endif // SPRINGLOBBY_HEADERGUARD_AUTOHOST_H
