
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
    m_battle.CustomBattleOptions().loadOptions( OptionsWrapper::ModOption, m_modname );
	const wxString sk_dir = m_mod_customs.getSingleValue( _T("skirmish_directory"), OptionsWrapper::ModCustomizations );

    //this block populates the radiobox and loads the skirmish options into the map
	OptionsWrapper::GameOption optFlag = OptionsWrapper::ModCustomizations;
    for ( IUnitSync::OptionMapListConstIter it = m_mod_customs.m_opts[optFlag].list_map.begin(); it != m_mod_customs.m_opts[optFlag].list_map.end(); ++it) {
	    mmOptionList current = it->second;
	    if ( _T("scenarios") == current.key ) {
            m_radioBox1 = new wxRadioBox( this, wxID_ANY, _("Select skirmish setup"), wxDefaultPosition, wxDefaultSize, current.cbx_choices, 1, wxRA_SPECIFY_ROWS );
            m_radioBox1->SetSelection( 0 );
            bSizer1->Add( m_radioBox1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 10 );

            wxString tooltip;
            int i = 0;
            for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor )
            {
                tooltip+= _T("\n") + itor->name + _T(": ") + itor->desc;
                OptionsWrapper temp;
                wxString filename = sk_dir + _T("/") + itor->key + _T(".lua") ;
                temp.loadOptions( OptionsWrapper::SkirmishOptions, m_modname, filename );
                m_skirmishes[current.cbx_choices[i]] =  temp;
                i++;
            }
            m_radioBox1->SetToolTip(TE(tooltip));

            m_radioBox1->SetName(current.key);
        }
	}

    optFlag = OptionsWrapper::SkirmishOptions;
    mmOptionList suggested_maps;
    OptionsWrapper map_op = m_skirmishes[m_radioBox1->GetStringSelection()];
    for ( IUnitSync::OptionMapListConstIter it = map_op.m_opts[optFlag].list_map.begin(); it != map_op.m_opts[optFlag].list_map.end(); ++it) {
	    mmOptionList current = it->second;
        if ( _T("suggested_maps") == current.key ) {
            suggested_maps = current;
        }
    }

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_map_label = new wxStaticText( this, wxID_ANY, _("Map"), wxDefaultPosition, wxDefaultSize, 0 );
	m_map_label->Wrap( -1 );
	bSizer2->Add( m_map_label, 0, wxALL, 5 );

	wxArrayString m_mapChoices = suggested_maps.cbx_choices.Count() > 0 ? suggested_maps.cbx_choices : usync().GetMapList() ;
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
	m_radioBox1->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( SkirmishDialog::OnRadioBox ), NULL, this );
}

SkirmishDialog::~SkirmishDialog()
{
	// Disconnect Events
	m_back->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnBack ), NULL, this );
	m_advanced->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnAdvanced ), NULL, this );
	m_start->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnStart ), NULL, this );
	m_radioBox1->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( SkirmishDialog::OnRadioBox ), NULL, this );
}

void SkirmishDialog::OnBack( wxCommandEvent& event )
{
    Destroy();
}

void SkirmishDialog::OnRadioBox( wxCommandEvent& event )
{
    m_map->Clear();
    OptionsWrapper::GameOption optFlag = OptionsWrapper::SkirmishOptions;
    wxArrayString maps = usync().GetMapList();
    OptionsWrapper map_op = m_skirmishes[m_radioBox1->GetStringSelection()];
    for ( IUnitSync::OptionMapListConstIter it = map_op.m_opts[optFlag].list_map.begin(); it != map_op.m_opts[optFlag].list_map.end(); ++it) {
	    mmOptionList current = it->second;
        if ( _T("suggested_maps") == current.key ) {
            maps = current.cbx_choices;
            break;
        }
    }

    for ( int i = 0; i < maps.Count() ; ++i ) {
        m_map->Append( maps[i] );
    }
    m_map->SetSelection( 0 );

}

void SkirmishDialog::OnAdvanced( wxCommandEvent& event )
{
    event.Skip();
}

template <class Cont>
void printCO( const Cont& c )
{
    typename Cont::const_iterator it= c.begin();
    for( ; it != c.end(); ++it ) {
        wxLogMessage( _T("opts:: ") + it->first + _T(" | ") + it->second.first + _T(" | ") + it->second.second );
    }
}

void SkirmishDialog::OnStart( wxCommandEvent& event )
{
    OptionsWrapper& opts = m_skirmishes[m_radioBox1->GetStringSelection()];
    // this overwrites any modoptions with those found in the skirmish definition
    m_battle.CustomBattleOptions().MergeOptions( opts, OptionsWrapper::ModOption );

    //now add AIs
    wxString ai_name = m_mod_customs.getSingleValue( _T("default_ai" ) );
    OptionsWrapper::GameOption optFlag = OptionsWrapper::SkirmishOptions;
    for ( IUnitSync::OptionMapListConstIter it = opts.m_opts[optFlag].list_map.begin(); it != opts.m_opts[optFlag].list_map.end(); ++it) {
        mmOptionList current = it->second;
        if ( current.key == _T("ai_team_ids") ) {

            for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); itor++ ) {
                m_battle.AddBot( ai_name, FromwxString<int>( itor->name ) );
            }
        break;
        }
    }


    m_battle.SetHostMap( m_map->GetStringSelection() , _T("") );
    m_battle.StartSpring();
}
