#include "mmoptionwindows.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/intl.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/combobox.h>
#include <wx/tipwin.h>
#include <wx/tooltip.h>
#include <wx/textctrl.h>

#include "ui.h"
#include "mmoptionswrapper.h"
#include "battle.h"
#include "utils/controls.h"
#include "utils/math.h"
#include "utils/conversion.h"
#include "spinctld.h"

BEGIN_EVENT_TABLE( SingleOptionDialog, wxDialog )

END_EVENT_TABLE()

SingleOptionDialog::SingleOptionDialog( IBattle& battle, const wxString& optiontag )
    : m_battle( battle ),
    m_tag( optiontag ),
    m_checkbox( 0 ),
    m_combobox( 0 ),
    m_spinctrl( 0 ),
    m_textctrl( 0 )
{
	OptionsWrapper& optWrap = m_battle.CustomBattleOptions();
	OptionsWrapper::GameOption optFlag = ( OptionsWrapper::GameOption )s2l( optiontag.BeforeFirst( '_' ) );
	wxString key = optiontag.AfterFirst( '_' );
	OptionType type = optWrap.GetSingleOptionType( key );
	Create( (wxWindow*)&ui().mw(), wxID_ANY, _( "Change option" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T( "OptionDialog" ) );
	if ( !optWrap.keyExists( key, optFlag, false, type ) )
	{
		EndModal( wxID_CANCEL );
		return;
	}

	wxBoxSizer* m_main_sizer = new wxBoxSizer( wxVERTICAL );

	// wxStaticText* m_labelctrl = wxStaticText();

	switch ( type )
	{
		case opt_bool:
			{
				mmOptionBool opt = optWrap.m_opts[optFlag].bool_map[key];
				m_checkbox = new wxCheckBox( this, wxID_ANY, opt.name );
				m_checkbox->SetToolTip( TE( opt.description ) );
				m_checkbox->SetValue( opt.value );
				m_main_sizer->Add( m_checkbox, 0, wxEXPAND );
				break;
			}
		case opt_float:
			{
				mmOptionFloat opt = optWrap.m_opts[optFlag].float_map[key];
				m_spinctrl = new wxSpinCtrlDbl();
				m_spinctrl->Create( this, wxID_ANY, _T( "" ), wxDefaultPosition, wxDefaultSize, 0, double( opt.min ), double( opt.max ), double( opt.value ), double( opt.stepping ), wxSPINCTRLDBL_AUTODIGITS, opt.key );
				m_spinctrl->SetToolTip( TE( opt.description ) );
				m_main_sizer->Add( m_spinctrl, 0, wxEXPAND );
				break;
			}
		case opt_string:
			{
				mmOptionString opt = optWrap.m_opts[optFlag].string_map[key];
				m_textctrl = new wxTextCtrl( this, wxID_ANY, opt.value, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, opt.key );
				m_textctrl->SetToolTip( TE( opt.description ) );
				m_main_sizer->Add( m_textctrl, 0, wxEXPAND );
				break;
			}
		case opt_list:
			{
				mmOptionList opt = optWrap.m_opts[optFlag].list_map[key];
				int temp = int( opt.cbx_choices.GetCount() - 1 );
				int index = clamp( opt.cur_choice_index, 0, temp );
				m_combobox = new wxComboBox( this, wxID_ANY, opt.cbx_choices[index], wxDefaultPosition, wxDefaultSize, opt.cbx_choices, wxCB_READONLY, wxDefaultValidator );
				wxString tooltip = opt.description + _T( "\n" );
				for ( ListItemVec::iterator itor = opt.listitems.begin(); itor != opt.listitems.end(); itor++ )
				{
					tooltip += _T( "\n" ) + itor->name + _T( ": " ) + itor->desc;
				}
				m_combobox->SetToolTip( TE( tooltip ) );
				m_main_sizer->Add( m_combobox, 0, wxEXPAND );
				break;
			}
		default:
			{
				EndModal( wxID_CANCEL );
				return;
				break;
			}
	}

	wxSize __SpacerSize_1 = wxDLG_UNIT( this, wxSize( 0, 0 ) );
	m_main_sizer->Add( __SpacerSize_1.GetWidth(), __SpacerSize_1.GetHeight(), 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5 );
	wxStaticLine* m_separator1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxSize( 10, -1 ), wxLI_HORIZONTAL, _T( "ID_STATICLINE1" ) );
	m_main_sizer->Add( m_separator1, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5 );
	wxBoxSizer* m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_cancel_button = new wxButton( this, ID_CANCEL, _( "Cancel" ), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T( "ID_CANCEL" ) );
	m_buttons_sizer->Add( m_cancel_button, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5 );
	m_buttons_sizer->Add( 0, 0, 1, wxALL | wxEXPAND | wxSHAPED | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0 );
	m_ok_button = new wxButton( this, ID_OK, _( "Ok" ), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T( "ID_OK" ) );
	m_buttons_sizer->Add( m_ok_button, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5 );
	m_main_sizer->Add( m_buttons_sizer, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0 );


	m_main_sizer->Fit( this );
	m_main_sizer->SetSizeHints( this );

	SetSizer( m_main_sizer );
	Layout();

	Connect( ID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, ( wxObjectEventFunction )&SingleOptionDialog::OnCancel );
	Connect( ID_OK, wxEVT_COMMAND_BUTTON_CLICKED, ( wxObjectEventFunction )&SingleOptionDialog::OnOk );
}

void SingleOptionDialog::OnCancel( wxCommandEvent& /*unused*/ )
{
	EndModal( wxID_CANCEL );
}

void SingleOptionDialog::OnOk( wxCommandEvent& /*unused*/ )
{
	OptionsWrapper::GameOption optFlag = ( OptionsWrapper::GameOption )s2l( m_tag.BeforeFirst( '_' ) );
	wxString key = m_tag.AfterFirst( '_' );
	wxString value;
	if ( m_textctrl ) value = m_textctrl->GetValue();
	else if ( m_combobox ) value = m_battle.CustomBattleOptions().GetNameListOptItemKey( key, m_combobox->GetValue(), optFlag );
	else if ( m_spinctrl ) value = TowxString( m_spinctrl->GetValue() );
	else if ( m_checkbox ) value = TowxString( m_checkbox->GetValue() );
	m_battle.CustomBattleOptions().setSingleOption( key, value, optFlag );
	m_battle.SendHostInfo( m_tag );
	EndModal( wxID_OK );
}
