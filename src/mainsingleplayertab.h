#ifndef SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H


#include <wx/panel.h>


class Ui;
class wxNotebook;
class wxImageList;
class wxBoxSizer;
class SinglePlayerTab;
class BattleOptionsTab;
class BattleroomMMOptionsTab;


class MainSinglePlayerTab : public wxPanel
{
  public:
    MainSinglePlayerTab( wxWindow* parent, Ui& ui );
    ~MainSinglePlayerTab();

    void UpdateMinimap();
    void OnUnitSyncReloaded();
    void Update( const wxString& Tag );

    void ReloadRestrictions();
    void ReloadMapOptContrls();
    void ReloadModOptContrls();
    void ReloadPresetList();

    SinglePlayerTab& GetSinglePlayerTab();
    BattleOptionsTab& GetOptionsTab();
    BattleroomMMOptionsTab& GetMMOptionsTab();

  protected:

    Ui& m_ui;

    wxBoxSizer* m_main_sizer;
    wxImageList* m_imagelist;
    wxNotebook* m_tabs;

    SinglePlayerTab* m_sp_tab;
    BattleOptionsTab* m_opts_tab;
    BattleroomMMOptionsTab* m_mm_opts_tab;

};


#endif // SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H
