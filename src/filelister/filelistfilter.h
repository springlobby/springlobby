#ifndef SPRINGLOBBY_HEADERGUARD_FILELISTFILTER_H
#define SPRINGLOBBY_HEADERGUARD_FILELISTFILTER_H

#include <wx/panel.h>

#include "filelistdialog.h"


///////////////////////////////////////////////////////////////////////////

class FileListDialog;
class wxToggleButton;
class wxCheckBox;
class wxStaticText;
class wxTextCtrl;
class wxChoice;
class wxButton;
class wxRegEx;
class wxStaticText;
class Battle;


///////////////////////////////////////////////////////////////////////////////
/// Class FileListFilter
///////////////////////////////////////////////////////////////////////////////
class FileListFilter : public wxPanel
{
	public:
    FileListFilter( wxWindow* parent, wxWindowID id, FileListDialog* parentBattleListTab, const wxPoint& pos, const wxSize& size, long style );

    void OnActivate       ( wxCommandEvent& event );

    void SetActiv         ( bool state );

    void OnChange            ( wxCommandEvent& event );
    void OnChangeName         ( wxCommandEvent& event );
    void OnChangeType         ( wxCommandEvent& event );

    bool FilterBattle(Battle& battle);
    bool GetActiv() const;

	protected:

		FileListDialog* m_parent_filelistdialog;
		wxStaticText* m_filter_text;

        //filename
		wxStaticText* m_filter_name_text;
		wxTextCtrl*   m_filter_name_edit;
        wxRegEx*      m_filter_name_expression;

        //filetype
		wxStaticText* m_filter_type_text;
		wxChoice* m_filter_type_choice;
		int m_filter_type_choice_value;


    DECLARE_EVENT_TABLE();
};

enum
{
    FILE_FILTER_NAME_EDIT,
    FILE_FILTER_TYPE_CHOICE,

};

#endif //SPRINGLOBBY_HEADERGUARD_BATTLELISTFILTER_H
