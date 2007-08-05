#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H

#include <wx/panel.h>

#include "iunitsync.h"

class Ui;
class Battle;
class User;
class ChatPanel;
class wxCommandEvent;
class wxBoxSizer;
class wxComboBox;
class wxStaticText;
class wxSplitterWindow;
class wxStaticLine;
class wxButton;
class wxCheckBox;
class MapCtrl;
class wxListCtrl;
class wxChoice;
class wxRadioBox;

class BattleMapTab : public wxPanel
{
  public:
    BattleMapTab( wxWindow* parent, Ui& ui, Battle& battle );
     ~BattleMapTab();

     void UpdateUser( User& user );

     void UpdateMap();
     void ReloadMaplist();

     void OnMapSelect( wxCommandEvent& event );

  protected:
    Ui& m_ui;
    Battle& m_battle;
    UnitSyncMap m_map;

		MapCtrl* m_minimap;
		wxChoice* m_map_combo;
		wxButton* m_browse_btn;
		wxRadioBox* m_start_radios;
		wxListCtrl* m_map_opts_list;

    DECLARE_EVENT_TABLE()
};

enum
{
    BMAP_MAP_SEL
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H
