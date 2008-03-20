#ifndef SPRINGLOBBY_HEADERGUARD_MAINOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINOPTIONSTAB_H

#include <wx/panel.h>

class Ui;
class wxCommandEvent;
class wxBoxSizer;
class wxImageList;
class wxNotebook;
class SpringOptionsTab;
class ChatOptionsTab;
class wxButton;
class wxBoxSizer;

class MainOptionsTab : public wxPanel
{
  public:
    MainOptionsTab( wxWindow* parent, Ui& ui );
    ~MainOptionsTab();

    void OnApply( wxCommandEvent& event );
    void OnRestore( wxCommandEvent& event );

  protected:
    wxBoxSizer* m_main_sizer;

    wxImageList* m_imagelist;

    wxNotebook* m_tabs;

    SpringOptionsTab* m_spring_opts;
    ChatOptionsTab* m_chat_opts;

    wxButton* m_restore_btn;
    wxButton* m_apply_btn;

    wxBoxSizer* m_button_sizer;

    Ui& m_ui;

    DECLARE_EVENT_TABLE()
};

enum
{
    OPTIONS_TABS = wxID_HIGHEST,
};

#endif // SPRINGLOBBY_HEADERGUARD_MAINOPTIONSTAB_H
