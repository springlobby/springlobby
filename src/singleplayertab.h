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
class wxCommandEvent;


class SinglePlayerTab: public wxPanel
{
  public:

    SinglePlayerTab( wxWindow* parent, Ui& ui );
    ~SinglePlayerTab();

    void ReloadMaplist();

    SinglePlayerBattle& GetBattle() { return m_battle; }

    void OnMapSelect( wxCommandEvent& event );
  protected:

    Ui& m_ui;
    SinglePlayerBattle m_battle;

		MapCtrl* m_minimap;
		wxChoice* m_map_pick;
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
  SP_BROWSE_MAP,
  SP_ADD_BOT,
  SP_RESET,
  SP_START
};

#endif // SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERTAB_H
