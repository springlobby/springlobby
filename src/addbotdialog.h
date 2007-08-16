#ifndef SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H

#include <wx/dialog.h>

class wxTextCtrl;
class wxStaticText;
class wxChoice;
class wxStaticLine;
class wxButton;
class wxCommandEvent;
class Battle;

class AddBotDialog : public wxDialog 
{
  public:

    AddBotDialog( wxWindow* parent, Battle& battle );

    wxString GetNick();
    wxString GetAI();

    void ReloadAIList();

    void OnClose( wxCommandEvent& event );
    void OnAddBot( wxCommandEvent& event );

  protected:
    wxStaticText* m_nick_lbl;
    wxTextCtrl* m_nick;
    wxStaticText* m_ai_lbl;
    wxChoice* m_ai;
    wxStaticLine* m_buttons_sep;
    wxButton* m_cancel_btn;
    wxButton* m_add_btn;

    Battle& m_battle;

    DECLARE_EVENT_TABLE()
};

enum
{
  ADDBOT_ADD = wxID_HIGHEST,
  ADDBOT_CANCEL
};

#endif // SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H
