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
class TorrentOptionsPanel;
class ManageGroupsPanel;
class LobbyOptionsTab;


/** \brief A container for the various option panels
 * Contains a notebook holding the real option panels as pages. Handles "apply" and "restore" events for those pages,
 * rather then those having to implement (and duplicate) this functionality. \n
 * See SpringOptionsTab, TorrentOptionsPanel, ChatOptionsTab
 */
class MainOptionsTab : public wxPanel
{
public:
    MainOptionsTab( wxWindow* parent, Ui& ui );
    ~MainOptionsTab();

    /** \brief delegate the data setting to memeber panels */
    void OnApply( wxCommandEvent& event );
    void OnRestore( wxCommandEvent& event );

    void OnOpenGroupsTab();
    void SetSelection( const unsigned int page );


protected:
    wxBoxSizer* m_main_sizer;

    wxImageList* m_imagelist;

    wxNotebook* m_tabs;

    SpringOptionsTab* m_spring_opts;
    ChatOptionsTab* m_chat_opts;
    TorrentOptionsPanel* m_torrent_opts;
    ManageGroupsPanel* m_groups_opts;
    LobbyOptionsTab* m_lobby_opts;


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
