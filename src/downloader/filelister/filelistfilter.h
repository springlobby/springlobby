/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_FILELISTFILTER_H
#define SPRINGLOBBY_HEADERGUARD_FILELISTFILTER_H

#ifndef NO_TORRENT_SYSTEM

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
class PlasmaResourceInfo;


///////////////////////////////////////////////////////////////////////////////
/// Class FileListFilter
///////////////////////////////////////////////////////////////////////////////
class FileListFilter : public wxPanel
{
	public:
    FileListFilter( wxWindow* parent, wxWindowID id, FileListDialog* parentBattleListTab, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxEXPAND );

    void OnActivate       ( wxCommandEvent& event );

    void SetActiv         ( bool state );

    void OnChange            ( wxCommandEvent& event );
    void OnChangeName         ( wxCommandEvent& event );
    void OnChangeType         ( wxCommandEvent& event );
    void OnChangeOndisk         ( wxCommandEvent& event );
    //! return false if info is to be displayed, false otherwise
    bool DoFilterResource( const PlasmaResourceInfo& );

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

		wxCheckBox* m_filter_ondisk;


	DECLARE_EVENT_TABLE()
};

enum
{
    FILE_FILTER_NAME_EDIT,
    FILE_FILTER_TYPE_CHOICE,
    FILE_FILTER_ONDISK
};

#endif

#endif //SPRINGLOBBY_HEADERGUARD_BATTLELISTFILTER_H

