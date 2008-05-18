#ifndef SPRINGLOBBY_HEADERGUARD_CHATOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_CHATOPTIONSTAB_H

#include <wx/panel.h>
#include <wx/checkbox.h>

class wxStaticBoxSizer;
class wxStaticBox;
class wxStaticText;
class wxRadioButton;
class wxButton;
class wxTextCtrl;
class wxBoxSizer;
class Ui;


class ChatOptionsTab : public wxPanel
{
  public:
    ChatOptionsTab( wxWindow* parent, Ui& ui );
    ~ChatOptionsTab();

    void UpdateTextSample();

    void DoRestore();

    void OnApply( wxCommandEvent& event );
    void OnRestore( wxCommandEvent& event );

    void OnSelectFont( wxCommandEvent& event );
    void OnUseSystemColors( wxCommandEvent& event );
    void OnNormalSelect( wxCommandEvent& event );
    void OnBGSelect( wxCommandEvent& event );
    void OnActionSelect( wxCommandEvent& event );
    void OnHighlightSelect( wxCommandEvent& event );
    void OnJoinLeaveSelect( wxCommandEvent& event );
    void OnNotificationSelect( wxCommandEvent& event );
    void OnMyMessSelect( wxCommandEvent& event );
    void OnServerSelect( wxCommandEvent& event );
    void OnClientSelect( wxCommandEvent& event );
    void OnErrorSelect( wxCommandEvent& event );
    void OnTimestampSelect( wxCommandEvent& event );
    void OnSaveLogs( wxCommandEvent& event );
    void OnBrowseLog( wxCommandEvent& event );
    void OnFindLog( wxCommandEvent& event );

  protected:

    enum
    {
      ID_SELFONT = 1000,
      ID_SYSCOLS,
      ID_NORMAL,
      ID_BG,
      ID_ACTION,
      ID_HIGHLIGHT,
      ID_JOINLEAVE,
      ID_NOTIFICATION,
      ID_MYMESS,
      ID_SERVER,
      ID_CLIENT,
      ID_ERROR,
      ID_TIMESTAMP,
      ID_SAVELOGS,
      ID_BROWSE_LOGS,
      ID_HIWORDS,
    };

    wxStaticText* m_text_sample;
    wxStaticText* m_fontname;
    wxStaticText* m_font_label;
    wxButton* m_select_font;
    wxCheckBox* m_use_sys_colors;
    wxPanel* m_custom_colors;
    wxButton* m_normal_color;
    wxStaticText* m_normal_label;
    wxButton* m_bg_color;
    wxStaticText* m_bg_label;
    wxButton* m_action_color;
    wxStaticText* m_action_label;
    wxButton* m_highlight_color;
    wxStaticText* m_highlight_label;
    wxButton* m_joinleave_color;
    wxStaticText* m_joinleave_label;
    wxButton* m_note_color;
    wxStaticText* m_note_label;
    wxButton* m_my_color;
    wxStaticText* m_my_label;
    wxButton* m_server_color;
    wxStaticText* m_server_label;
    wxButton* m_client_color;
    wxStaticText* m_client_label;
    wxButton* m_error_color;
    wxStaticText* m_error_label;
    wxButton* m_ts_color;
    wxStaticText* m_ts_label;
    wxTextCtrl* m_test_text;
    wxCheckBox* m_save_logs;
    wxStaticText* m_chat_save_label;
    wxTextCtrl* m_log_save;
    wxButton* m_browse_log;
    wxStaticText* m_hilight_words_label;

    wxTextCtrl* m_highlight_words;

    wxCheckBox* m_smart_scroll;

    wxFont m_chat_font;

    Ui& m_ui;

    DECLARE_EVENT_TABLE()
};

// enum
// {
//     SPRING_DIRBROWSE = wxID_HIGHEST,
//     SPRING_EXECBROWSE,
//     SPRING_SYNCBROWSE,
//     SPRING_WEBBROWSE,
//     SPRING_DEFEXE,
//     SPRING_DEFUSYNC,
//     SPRING_DEFWEB,
//     SPRING_AUTOCONF,
//     SPRING_DIRFIND,
//     SPRING_EXECFIND,
//     SPRING_SYNCFIND,
//
//     //Chat Log
//     CHATLOG_FIND,
//     CHATLOG_BROWSE,
//     CHATLOG_ENABLE,
//
// };

#endif // SPRINGLOBBY_HEADERGUARD_CHATOPTIONSTAB_H
