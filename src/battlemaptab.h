#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H

#include <wx/scrolwin.h>

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
class wxMouseEvent;

/** \brief select map, draw startboxes (in sp define startpos)
 * \todo DOCMEMORE */
class BattleMapTab : public wxScrolledWindow
{
  public:
    BattleMapTab( wxWindow* parent, Ui& ui, Battle& battle );
     ~BattleMapTab();

     void UpdateUser( User& user );

     void Update();
     void Update( const wxString& Tag );
     void ReloadMaplist();

     void OnMapSelect( wxCommandEvent& event );
     void OnMapBrowse( wxCommandEvent& event );
     void OnStartTypeSelect( wxCommandEvent& event );
     void OnMouseWheel( wxMouseEvent& event );

    void OnUnitSyncReloaded();

    void SetMap( int index );

  protected:
    Ui& m_ui;
    Battle& m_battle;
    //UnitSyncMap m_map;

    MapCtrl* m_minimap;
    wxChoice* m_map_combo;
    wxButton* m_browse_btn;
    wxRadioBox* m_start_radios;
    wxListCtrl* m_map_opts_list;

    enum {
      BMAP_MAP_SEL = wxID_HIGHEST,
      BMAP_MAP_BROWSE,
      BMAP_START_TYPE
    };

    DECLARE_EVENT_TABLE()
};


#endif // SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H
