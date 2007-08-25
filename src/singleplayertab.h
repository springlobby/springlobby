#ifndef SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERTAB_H
#define SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERTAB_H


#include <wx/panel.h>

#include "singleplayerbattle.h"


class Ui;
class MapCtrl;
class SinglePlayerBattle;

class wxWindow;
class wxButton;
class wxStaticLine;
class wxChoice;
class wxStaticText;
class wxCommandEvent;


class SinglePlayerTab: public wxPanel
{
  public:

    SinglePlayerTab( wxWindow* parent, Ui& ui, MainSinglePlayerTab& msptab );
    ~SinglePlayerTab();

    void UpdateMinimap();
    void ReloadMaplist();
    void ReloadModlist();

    SinglePlayerBattle& GetBattle() { return m_battle; }

    void OnMapSelect( wxCommandEvent& event );
    void OnModSelect( wxCommandEvent& event );
  protected:

    Ui& m_ui;
    SinglePlayerBattle m_battle;

    MapCtrl* m_minimap;
    wxChoice* m_map_pick;
    wxChoice* m_mod_pick;
    wxStaticText* m_map_lbl;
    wxStaticText* m_mod_lbl;
    wxButton* m_select_btn;
    wxButton* m_addbot_btn;
    wxStaticLine* m_buttons_sep;
    wxButton* m_reset_btn;
    wxButton* m_start_btn;

  DECLARE_EVENT_TABLE()
};


enum
{
  SP_MAP_PICK = wxID_HIGHEST,
  SP_MOD_PICK,
  SP_BROWSE_MAP,
  SP_ADD_BOT,
  SP_RESET,
  SP_START
};

#endif // SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERTAB_H
