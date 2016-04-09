/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_HOSTING_IBATTLEROOMTAB_H_
#define SRC_GUI_HOSTING_IBATTLEROOMTAB_H_

#include <wx/panel.h>

#include "utils/uievents.h"

class IBattle;
class User;
class ChatPanel;

class IBattleRoomTab : public wxPanel
{
protected:
	IBattleRoomTab(wxWindow* parent, int id);
	virtual ~IBattleRoomTab();

public:
	static IBattleRoomTab* CreateInstance(wxWindow* parent, IBattle* battle);

	virtual void UpdateUser(User& user, bool userJustAdded = false) = 0;

	virtual IBattle* GetBattle() = 0;
	virtual ChatPanel& GetChatPanel() = 0;

	virtual void UpdateBattleInfo() = 0;
	virtual void UpdateBattleInfo(const wxString& Tag) = 0;

	virtual void OnBattleActionEvent(UiEvents::UiEventData data) = 0;

	virtual void OnUserJoined(User& user) = 0;
	virtual void OnUserLeft(User& user) = 0;

	virtual void ReloadMaplist() = 0;
	virtual void SetMap(int index) = 0;

	virtual void UpdateHighlights() = 0;
	virtual void UpdatePresetList() = 0;
	virtual void SortPlayerList() = 0;
	virtual void SetBattle(IBattle* battle) = 0;

	virtual void PrintAllySetup() = 0;
};

#endif /* SRC_GUI_HOSTING_IBATTLEROOMTAB_H_ */
