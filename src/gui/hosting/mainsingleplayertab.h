/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H

#include <wx/scrolwin.h>
#include "battleroommmoptionstab.h"

class Ui;
class SLNotebook;
class wxImageList;
class wxBoxSizer;
class SinglePlayerTab;
class BattleRestrictionsTab;

class MainSinglePlayerTab : public wxPanel
{
public:
	MainSinglePlayerTab(wxWindow* parent);
	~MainSinglePlayerTab();

	void UpdateMinimap();
	void OnUnitSyncReloaded();

	void ReloadRestrictions();
	void ReloadMapOptContrls();
	void ReloadModOptContrls();
	void ReloadPresetList();

	SinglePlayerTab& GetSinglePlayerTab();
	BattleRestrictionsTab& GetOptionsTab();
	BattleroomMMOptionsTab& GetMMOptionsTab();

	void LoadPerspective(const wxString& perspective_name = wxEmptyString);
	void SavePerspective(const wxString& perspective_name = wxEmptyString);

private:
	wxBoxSizer* m_main_sizer;
	wxImageList* m_imagelist;
	SLNotebook* m_tabs;

	SinglePlayerTab* m_sp_tab;
	BattleRestrictionsTab* m_opts_tab;
	BattleroomMMOptionsTab* m_mm_opts_tab;
};


#endif // SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H
