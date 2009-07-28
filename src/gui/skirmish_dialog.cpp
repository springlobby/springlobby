
#include "skirmish_dialog.h"

#include <wx/string.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>

#include "../springunitsync.h"
#include "../springunitsynclib.h"
#include "../utils/customdialogs.h"
#include "../utils/conversion.h"
#include "../mmoptionswrapper.h"
#include "../utils/controls.h"

///////////////////////////////////////////////////////////////////////////

SkirmishDialog::SkirmishDialog( wxWindow* parent, const wxString& modname, OptionsWrapper mod_customs, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
    : wxFrame( parent, id, title, pos, size, style ),
    m_mod_customs( mod_customs ),
    m_modname( modname )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

    m_battle.SetHostMod( m_modname, wxEmptyString );
	const wxString sk_dir = m_mod_customs.getSingleValue( _T("skirmish_directory"), OptionsWrapper::ModCustomizations );

	OptionsWrapper::GameOption optFlag = OptionsWrapper::ModCustomizations;
    for ( IUnitSync::OptionMapListConstIter it = m_mod_customs.m_opts[optFlag].list_map.begin(); it != m_mod_customs.m_opts[optFlag].list_map.end(); ++it) {
	    mmOptionList current = it->second;
	    if ( _T("scenarios") == current.key ) {
            m_radioBox1 = new wxRadioBox( this, wxID_ANY, _("Select skirmish setup"), wxDefaultPosition, wxDefaultSize, current.cbx_choices, 1, wxRA_SPECIFY_ROWS );
            m_radioBox1->SetSelection( 0 );
            bSizer1->Add( m_radioBox1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 10 );

            wxString tooltip;
            for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); itor++ )
            {
                tooltip+= _T("\n") + itor->name + _T(": ") + itor->desc;
                OptionsWrapper temp;
                wxString filename = sk_dir + _T("/") + itor->key + _T(".lua") ;
                temp.loadOptions( OptionsWrapper::SkirmishOptions, m_modname, filename );
                m_skirmishes[current.name] =  temp;
            }
            m_radioBox1->SetToolTip(TE(tooltip));

            m_radioBox1->SetName(current.key);
        }
	}

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_map_label = new wxStaticText( this, wxID_ANY, _("Map"), wxDefaultPosition, wxDefaultSize, 0 );
	m_map_label->Wrap( -1 );
	bSizer2->Add( m_map_label, 0, wxALL, 5 );

	wxArrayString m_mapChoices;
	m_map = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_mapChoices, 0 );
	m_map->SetSelection( 0 );
	bSizer2->Add( m_map, 0, wxALL, 5 );

	m_map_random = new wxCheckBox( this, wxID_ANY, _("random"), wxDefaultPosition, wxDefaultSize, 0 );

	bSizer2->Add( m_map_random, 0, wxALL, 5 );

	bSizer1->Add( bSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL, 15 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_back = new wxButton( this, wxID_ANY, _("Back"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_back, 0, wxALL, 5 );

	m_advanced = new wxButton( this, wxID_ANY, _("Advanced setup"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_advanced, 0, wxALL, 5 );

	m_start = new wxButton( this, wxID_ANY, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_start->SetDefault();
	bSizer3->Add( m_start, 0, wxALL, 5 );

	bSizer1->Add( bSizer3, 1, wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_back->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnBack ), NULL, this );
	m_advanced->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnAdvanced ), NULL, this );
	m_start->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnStart ), NULL, this );

}

SkirmishDialog::~SkirmishDialog()
{
	// Disconnect Events
	m_back->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnBack ), NULL, this );
	m_advanced->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnAdvanced ), NULL, this );
}

void SkirmishDialog::OnBack( wxCommandEvent& event )
{
    event.Skip();
}

void SkirmishDialog::OnAdvanced( wxCommandEvent& event )
{
    event.Skip();
}

void SkirmishDialog::OnStart( wxCommandEvent& event )
{
    OptionsWrapper& opts = m_skirmishes[m_radioBox1->GetStringSelection()];
    UnitSyncMap map = usync().GetMapEx( 0 );
    m_battle.SetHostMap( map.name, map.hash );
    m_battle.StartSpring();
}
