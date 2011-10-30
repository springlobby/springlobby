/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//

#ifndef NO_TORRENT_SYSTEM

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/intl.h>
#include <wx/choice.h>
#include <wx/statbox.h>
#include <wx/event.h>
#include <wx/regex.h>
#include <wx/checkbox.h>

#include "filelistfilter.h"
#include "filelistctrl.h"
#include "filelistdialog.h"
#include "../uiutils.h"
#include "../utils/downloader.h"
#include "../torrentwrapper.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( FileListFilter, wxPanel )
	EVT_CHOICE( FILE_FILTER_TYPE_CHOICE, FileListFilter::OnChangeType )
	EVT_TEXT( FILE_FILTER_NAME_EDIT  , FileListFilter::OnChangeName )
	EVT_CHECKBOX( FILE_FILTER_ONDISK  , FileListFilter::OnChangeOndisk )
END_EVENT_TABLE()


FileListFilter::FileListFilter( wxWindow* parent, wxWindowID id, FileListDialog* parentBattleListTab, const wxPoint& pos, const wxSize& size, long style )
		: wxPanel( parent, id, pos, size, style ),
		m_parent_filelistdialog( parentBattleListTab )
{
	wxBoxSizer* m_filter_sizer;
	m_filter_sizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* m_filter_body_sizer;
	m_filter_body_sizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxEmptyString ), wxVERTICAL );

	wxBoxSizer* m_filter_body_row1_sizer;
	m_filter_body_row1_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_filter_name_text = new wxStaticText( this, wxID_ANY, _( "Filename:" ), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_filter_name_text->Wrap( -1 );
	m_filter_name_text->SetMinSize( wxSize( 90,-1 ) );


	m_filter_body_row1_sizer->Add( m_filter_name_text, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_filter_name_edit = new wxTextCtrl( this, FILE_FILTER_NAME_EDIT, _T( "" ), wxDefaultPosition, wxSize( -1,-1 ), 0|wxSIMPLE_BORDER );
	m_filter_name_edit->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	m_filter_name_edit->SetMinSize( wxSize( 220,-1 ) );
	m_filter_name_expression = new wxRegEx( m_filter_name_edit->GetValue(),wxRE_ICASE );

	m_filter_body_row1_sizer->Add( m_filter_name_edit, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* m_filter_type_sizer;
	m_filter_type_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_filter_type_text = new wxStaticText( this, wxID_ANY, _( "Filetype:" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_filter_type_text->Wrap( -1 );
	m_filter_type_sizer->Add( m_filter_type_text, 0, wxALIGN_RIGHT|wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* m_filter_ondisk_sizer;
	m_filter_ondisk_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_filter_ondisk = new wxCheckBox( this, FILE_FILTER_ONDISK, _T( "Filter files already on disk" ) );
	m_filter_ondisk_sizer->Add( m_filter_ondisk, 0, wxALIGN_CENTER_VERTICAL );

	wxString firstChoice = _T( "Any" );

	wxArrayString m_filter_type_choiceChoices;

	m_filter_type_choiceChoices.Add( firstChoice );
	m_filter_type_choiceChoices.Add( _( "Map" ) );
	m_filter_type_choiceChoices.Add( _( "Game" ) );

	m_filter_type_choice = new wxChoice( this, FILE_FILTER_TYPE_CHOICE, wxDefaultPosition, wxDefaultSize, m_filter_type_choiceChoices, wxSIMPLE_BORDER );

	m_filter_type_sizer->Add( m_filter_type_choice, 0, wxALIGN_RIGHT|wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_filter_body_row1_sizer->Add( m_filter_type_sizer, 0, wxEXPAND, 5 );
	m_filter_body_row1_sizer->Add( m_filter_ondisk_sizer, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_filter_body_sizer->Add( m_filter_body_row1_sizer, 1, wxEXPAND, 5 );


	m_filter_sizer->Add( m_filter_body_sizer, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( m_filter_sizer );
	this->Layout();
	m_filter_sizer->Fit( this );


	delete m_filter_name_expression;
	m_filter_name_expression = new wxRegEx( m_filter_name_edit->GetValue(),wxRE_ICASE );
	m_filter_type_choice_value = -1;
	wxCommandEvent dummy;
	OnChange( dummy );

}

bool FileListFilter::DoFilterResource( const PlasmaResourceInfo& /*info*/ )
{
//  if(!data.ok())return false;
//	if ( data->name.Upper().Find( m_filter_name_edit->GetValue().Upper() ) == wxNOT_FOUND
//	        && !m_filter_name_expression->Matches( data->name ) )
//		return false;
//	if ( m_filter_type_choice_value == 0 && data->type != SpringUnitSync::map ) return false;
//
//	if ( m_filter_type_choice_value == 1 && data->type != SpringUnitSync::mod ) return false;
//
//	if ( m_filter_ondisk->IsChecked() && data->HasFullFileLocal() )
//		return false;

	return false;
}

void FileListFilter::OnChange( wxCommandEvent&  )
{
    //needs dummy event data
	m_parent_filelistdialog->UpdateList( GlobalEvents::GlobalEventData() );
}

void FileListFilter::OnChangeName( wxCommandEvent& event )
{
	delete m_filter_name_expression;
	m_filter_name_expression = new wxRegEx( m_filter_name_edit->GetValue(),wxRE_ICASE );
	OnChange( event );
}

void FileListFilter::OnChangeType( wxCommandEvent& event )
{
	m_filter_type_choice_value = m_filter_type_choice->GetSelection()-1;
	OnChange( event );
}

void FileListFilter::OnChangeOndisk( wxCommandEvent& event )
{
	OnChange( event );
}

#endif
