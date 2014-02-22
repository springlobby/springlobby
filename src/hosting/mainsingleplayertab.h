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
class BattleOptionsTab;

class MainSinglePlayerTab : public wxScrolledWindow
{
  public:
    MainSinglePlayerTab( wxWindow* parent );
    ~MainSinglePlayerTab();

    void UpdateMinimap();
    void OnUnitSyncReloaded();
	void UpdateTag( const wxString& Tag );

    void ReloadRestrictions();
    void ReloadMapOptContrls();
    void ReloadModOptContrls();
    void ReloadPresetList();

    SinglePlayerTab& GetSinglePlayerTab();
    BattleOptionsTab& GetOptionsTab();
    BattleroomMMOptionsTab<SinglePlayerBattle>& GetMMOptionsTab();

    void LoadPerspective( const wxString& perspective_name = wxEmptyString );
    void SavePerspective( const wxString& perspective_name = wxEmptyString );

  protected:

    wxBoxSizer* m_main_sizer;
    wxImageList* m_imagelist;
    SLNotebook* m_tabs;

    SinglePlayerTab* m_sp_tab;
    BattleOptionsTab* m_opts_tab;
    BattleroomMMOptionsTab<SinglePlayerBattle>* m_mm_opts_tab;

};


#endif // SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H
