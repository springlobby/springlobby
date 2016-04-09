/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_HOSTING_IBATTLEMAPTAB_H_
#define SRC_GUI_HOSTING_IBATTLEMAPTAB_H_

#include <wx/panel.h>

class IBattle;
class User;

class IBattleMapTab : public wxPanel
{
protected:
	IBattleMapTab(wxWindow* parent, int id);
	virtual ~IBattleMapTab();

public:
	static IBattleMapTab* CreateInstance(wxWindow* parent, IBattle* battle);

	virtual void UpdateUser(User& user) = 0;

	virtual void Update() = 0;
	virtual void Update(const wxString& Tag) = 0;
	virtual void ReloadMaplist() = 0;

	virtual void SetMap(int index) = 0;
	virtual void SetBattle(IBattle* battle) = 0;
};

#endif /* SRC_GUI_HOSTING_IBATTLEMAPTAB_H_ */
