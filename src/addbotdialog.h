#ifndef SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H

#include <wx/dialog.h>

class wxTextCtrl;
class wxStaticText;
class wxChoice;
class wxStaticLine;
class wxButton;
class wxCommandEvent;
class IBattle;

class AddBotDialog : public wxDialog
{
  public:

    AddBotDialog( wxWindow* parent, IBattle& battle, bool singleplayer = false );

    wxString GetNick();
    wxString GetAI();

    void ReloadAIList();

    void OnClose( wxCommandEvent& event );
    void OnAddBot( wxCommandEvent& event );
    void OnSelectBot( wxCommandEvent& event );

  protected:
    wxStaticText* m_nick_lbl;
    wxTextCtrl* m_nick;
    wxStaticText* m_ai_lbl;
    wxChoice* m_ai;
    wxStaticLine* m_buttons_sep;
    wxButton* m_cancel_btn;
    wxButton* m_add_btn;

    IBattle& m_battle;

    bool m_sp;

    DECLARE_EVENT_TABLE()
};

enum
{
  ADDBOT_ADD = wxID_HIGHEST,
  ADDBOT_CANCEL,
  ADDBOT_AI
};

#endif // SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H
