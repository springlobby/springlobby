/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef BATTLEROOMDOWNLOADS_H
#define BATTLEROOMDOWNLOADS_H
#include <wx/panel.h>
class wxGauge;
class wxStaticText;
class ObserverDownloadInfo;
class IBattle;
class wxBoxSize;

class BattleRoomDownloadProgres;

class BattleRoomDownloads : public wxPanel
{
public:
	BattleRoomDownloads(wxWindow* parent, IBattle* battle);
	~BattleRoomDownloads();
	void OnUpdate();
	void SetBattle(IBattle* battle);

private:
	BattleRoomDownloadProgres* m_mod;
	BattleRoomDownloadProgres* m_map;

	IBattle* m_battle;
};

#endif // BATTLEROOMDOWNLOADS_H
