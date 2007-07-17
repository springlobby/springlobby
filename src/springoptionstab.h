//
// Class: BattleListTab
//

#ifndef _SPRINGOPTIONSTAB_H_
#define _SPRINGOPTIONSTAB_H_

#include <wx/panel.h>

class wxStaticBoxSizer;
class wxStaticBox;
class wxStaticText;
class wxRadioButton;
class wxButton;
class wxTextCtrl;
class wxBoxSizer;
class Ui;

#ifdef WIN32
#define DLLEXT _T(".dll")
#define EXEEXT _T(".exe")
#define CHOOSE_EXE _("Executables (*.exe)|*.exe|Any File (*.*)|*.*")
#define CHOOSE_DLL _("Library (*.dll)|*.dll|Any File (*.*)|*.*")
#else
#define DLLEXT _T(".so")
#define EXEEXT _T("")
#define CHOOSE_EXE _("Any file (*.*)|*.*")
#define CHOOSE_DLL _("Library (*.so)|*.so|Any File (*.*)|*.*")
#endif

class SpringOptionsTab : public wxPanel
{
  public:
    SpringOptionsTab( wxWindow* parent, Ui& ui );
     ~SpringOptionsTab();

    void OnBrowseDir( wxCommandEvent& event );
    void OnBrowseExec( wxCommandEvent& event );
    void OnBrowseSync( wxCommandEvent& event );

    void OnApply( wxCommandEvent& event );
    void OnRestore( wxCommandEvent& event );

    void DoRestore();

  protected:

    wxStaticText* m_dir_text;
    wxStaticText* m_exec_loc_text;
    wxStaticText* m_sync_loc_text;

    wxButton* m_dir_browse_btn;
    wxButton* m_dir_find_btn;
    wxButton* m_exec_browse_btn;
    wxButton* m_exec_find_btn;
    wxButton* m_sync_browse_btn;
    wxButton* m_sync_find_btn;
    wxButton* m_auto_btn;

    wxRadioButton* m_exec_def_radio;
    wxRadioButton* m_exec_spec_radio;
    wxRadioButton* m_sync_def_radio;
    wxRadioButton* m_sync_spec_radio;

    wxTextCtrl* m_dir_edit;
    wxTextCtrl* m_exec_edit;
    wxTextCtrl* m_sync_edit;

    wxStaticBox* m_exec_box;
    wxStaticBox* m_sync_box;
    wxStaticBoxSizer* m_exec_box_sizer;
    wxStaticBoxSizer* m_sync_box_sizer;

    wxBoxSizer* m_main_sizer;
    wxBoxSizer* m_dir_sizer;
    wxBoxSizer* m_exec_sizer;
    wxBoxSizer* m_sync_sizer;
    wxBoxSizer* m_aconf_sizer;
    wxBoxSizer* m_exec_loc_sizer;
    wxBoxSizer* m_sync_loc_sizer;

    Ui& m_ui;

    DECLARE_EVENT_TABLE()
};

enum
{
    SPRING_DIRBROWSE = wxID_HIGHEST,
    SPRING_EXECBROWSE,
    SPRING_SYNCBROWSE
};

#endif  //_SPRINGOPTIONSTAB_H_

