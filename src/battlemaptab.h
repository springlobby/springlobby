#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H

#include <wx/panel.h>

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

class BattleMapTab : public wxPanel
{
  public:
    BattleMapTab( wxWindow* parent, Ui& ui, Battle& battle );
     ~BattleMapTab();

  protected:
    Ui& m_ui;
    Battle& m_battle;

    DECLARE_EVENT_TABLE()
};

enum
{
    BMAP_NONE
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H
