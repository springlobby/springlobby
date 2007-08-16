#ifndef SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H

#include <wx/dialog.h>

class wxTextCtrl;
class wxStaticText;
class wxChoice;
class wxStaticLine;
class wxButton;
class wxCommandEvent;


class AddBotDialog : public wxDialog 
{
  public:

    AddBotDialog( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Add bot"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 360,155 ), int style = wxDEFAULT_DIALOG_STYLE );

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

    DECLARE_EVENT_TABLE()
};

enum
{
  ADDBOT_ADD = wxID_HIGHEST,
  ADDBOT_CANCEL
};

#endif // SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H
