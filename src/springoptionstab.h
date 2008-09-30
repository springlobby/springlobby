#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGOPTIONSTAB_H

#include <wx/scrolwin.h>
#include <wx/checkbox.h>

class wxStaticBoxSizer;
class wxStaticBox;
class wxStaticText;
class wxRadioButton;
class wxButton;
class wxTextCtrl;
class wxBoxSizer;
class Ui;

class SpringOptionsTab : public wxScrolledWindow
{
  public:
    SpringOptionsTab( wxWindow* parent, Ui& ui );
    ~SpringOptionsTab();

    void DoRestore();

    void HandleExeloc( bool defloc );
    void HandleUsyncloc( bool defloc );
    void HandleWebloc( bool defloc );

    wxString AutoFindSpringDir( const wxString& def = _T("") );
    wxString AutoFindSpringExe( const wxString& def = _T("") );
    wxString AutoFindUnitSyncLib( const wxString& def = _T("") );

    bool IsDataDir( const wxString& dir );
    bool IsSpringExe( const wxString& exe );
    bool IsUnitSyncLib( const wxString& lib );

    void ReloadSpringPathFromConfig();

    void OnBrowseDir( wxCommandEvent& event );
    void OnBrowseExec( wxCommandEvent& event );
    void OnBrowseSync( wxCommandEvent& event );
    void OnBrowseWeb( wxCommandEvent& event );

    void OnApply( wxCommandEvent& event );
    void OnRestore( wxCommandEvent& event );

    void OnDefaultExe( wxCommandEvent& event );
    void OnDefaultUsync( wxCommandEvent& event );
    void OnDefaultWeb( wxCommandEvent& event );

    void OnAutoConf( wxCommandEvent& event );
    void OnFindDir( wxCommandEvent& event );
    void OnFindExec( wxCommandEvent& event );
    void OnFindSync( wxCommandEvent& event );

  protected:

    wxStaticText* m_dir_text;
    wxStaticText* m_exec_loc_text;
    wxStaticText* m_sync_loc_text;
    wxStaticText* m_web_loc_text;

    wxButton* m_dir_browse_btn;
    wxButton* m_dir_find_btn;
    wxButton* m_exec_browse_btn;
    wxButton* m_exec_find_btn;
    wxButton* m_sync_browse_btn;
    wxButton* m_sync_find_btn;
    wxButton* m_web_browse_btn;

    wxButton* m_auto_btn;

    wxRadioButton* m_exec_def_radio;
    wxRadioButton* m_exec_spec_radio;
    wxRadioButton* m_sync_def_radio;
    wxRadioButton* m_sync_spec_radio;
    wxRadioButton* m_web_def_radio;
    wxRadioButton* m_web_spec_radio;

    wxTextCtrl* m_dir_edit;
    wxTextCtrl* m_exec_edit;
    wxTextCtrl* m_sync_edit;
    wxTextCtrl* m_web_edit;

    wxStaticBox* m_dir_box;
    wxStaticBox* m_exec_box;
    wxStaticBox* m_sync_box;
    wxStaticBox* m_web_box;
    wxStaticBoxSizer* m_dir_box_sizer;
    wxStaticBoxSizer* m_exec_box_sizer;
    wxStaticBoxSizer* m_sync_box_sizer;
    wxStaticBoxSizer* m_web_box_sizer;


    wxBoxSizer* m_main_sizer;
    wxBoxSizer* m_dir_sizer;
    wxBoxSizer* m_aconf_sizer;
    wxBoxSizer* m_exec_loc_sizer;
    wxBoxSizer* m_sync_loc_sizer;
    wxBoxSizer* m_web_loc_sizer;

    Ui& m_ui;

    DECLARE_EVENT_TABLE()
};

enum
{
    SPRING_DIRBROWSE = wxID_HIGHEST,
    SPRING_EXECBROWSE,
    SPRING_SYNCBROWSE,
    SPRING_WEBBROWSE,
    SPRING_DEFEXE,
    SPRING_DEFUSYNC,
    SPRING_DEFWEB,
    SPRING_AUTOCONF,
    SPRING_DIRFIND,
    SPRING_EXECFIND,
    SPRING_SYNCFIND,
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGOPTIONSTAB_H
