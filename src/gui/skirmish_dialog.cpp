
#include "skirmish_dialog.h"

#include <wx/string.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/settings.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/combobox.h>
#include <cstdlib>

#include "../springunitsync.h"
#include "../springunitsynclib.h"
#include "../utils/customdialogs.h"
#include "../utils/conversion.h"
#include "../mmoptionswrapper.h"
#include "../utils/controls.h"
#include "../ui.h"
#include "../mainwindow.h"
#include "wxbackgroundimage.h"
#include "simplefront.h"
#include "../utils/controls.h"
#include "../customizations.h"


SkirmishDialog::SkirmishDialog( SimpleFront* parent, wxWindowID id )
    : wxGradientPanel( (wxWindow*)parent, id ),
    m_parent( parent ),
    m_mod_customs( SLcustomizations().GetCustomizations() )
{
    wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    m_battle.SetHostMod( SLcustomizations().GetModname(), wxEmptyString );
    m_battle.CustomBattleOptions().loadOptions( OptionsWrapper::ModOption, SLcustomizations().GetModname() );
	const wxString sk_dir = m_mod_customs.getSingleValue( _T("skirmish_directory"), OptionsWrapper::ModCustomizations );

    //this block populates the radiobox and loads the skirmish options into the map
	OptionsWrapper::GameOption optFlag = OptionsWrapper::ModCustomizations;
    for ( IUnitSync::OptionMapListConstIter it = m_mod_customs.m_opts[optFlag].list_map.begin(); it != m_mod_customs.m_opts[optFlag].list_map.end(); ++it) {
	    mmOptionList current = it->second;
	    if ( _T("scenarios") == current.key ) {
            m_scenario_choice = new wxChoice( this, wxID_ANY, wxDefaultPosition,wxDefaultSize, current.cbx_choices, wxTRANSPARENT_WINDOW );
            #ifdef __WXMSW__ //another msw fail: control will not size correctly when items are not added manually
                m_scenario_choice->Clear();
                for( size_t u = 0; u < current.cbx_choices.Count(); ++u ) {
                        m_scenario_choice->Append( current.cbx_choices[u] );
                }
            #endif
            m_scenario_choice->SetSelection( 0 );
            wxStaticText* label = new wxStaticText( this, wxID_ANY, _("Scenario"), wxDefaultPosition, wxDefaultSize, wxTRANSPARENT_WINDOW );
            fgSizer1->Add( label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
//            m_scenario_choice->SetSize( wxSize( 1.5 * GetMaxStringWidth( *this, current.cbx_choices ), 30 ) );
            fgSizer1->Add( m_scenario_choice, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


            wxString tooltip;
            int i = 0;
            for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor )
            {
                tooltip+= _T("\n") + itor->name + _T(": ") + itor->desc;
                OptionsWrapper temp;
                wxString filename = sk_dir + _T("/") + itor->key + _T(".lua") ;
                temp.loadOptions( OptionsWrapper::SkirmishOptions, SLcustomizations().GetModname(), filename );
                m_skirmishes[current.cbx_choices[i]] =  temp;
                i++;
            }
            m_scenario_choice->SetToolTip(TE(tooltip));

            m_scenario_choice->SetName(current.key);

            break;
        }
	}

    optFlag = OptionsWrapper::SkirmishOptions;
    mmOptionList suggested_maps;
    mmOptionList suggested_sides;
    OptionsWrapper map_op = m_skirmishes[m_scenario_choice->GetStringSelection()];
    for ( IUnitSync::OptionMapListConstIter it = map_op.m_opts[optFlag].list_map.begin(); it != map_op.m_opts[optFlag].list_map.end(); ++it) {
	    mmOptionList current = it->second;
        if ( _T("suggested_maps") == current.key ) {
            suggested_maps = current;
        }
        else if ( _T("suggested_sides") == current.key ) {
            suggested_sides = current;
        }
    }

	m_map_label = new wxStaticText( this, wxID_ANY, _("Map"), wxDefaultPosition, wxDefaultSize, 0 );
	m_map_label->Wrap( -1 );
	fgSizer1->Add( m_map_label, 0, wxALIGN_CENTER_VERTICAL| wxALL, 5 );
	wxArrayString m_mapChoices = suggested_maps.cbx_choices.Count() > 0 ? suggested_maps.cbx_choices : usync().GetMapList() ;
	m_map = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_mapChoices, 0 );
	m_map->SetSelection( 0 );
	fgSizer1->Add( m_map, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
//	m_map_random = new wxCheckBox( this, wxID_ANY, _("random"), wxDefaultPosition, wxDefaultSize, 0 );
//	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
//	fgSizer1->Add( m_map_random, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	m_sides_label = new wxStaticText( this, wxID_ANY, _("Faction"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sides_label->Wrap( -1 );
	fgSizer1->Add( m_sides_label, 0, wxALIGN_CENTER_VERTICAL| wxALL, 5 );

	wxArrayString m_sidesChoices = suggested_sides.cbx_choices.Count() > 0 ? suggested_sides.cbx_choices : usync().GetSides( SLcustomizations().GetModname() ) ;
	m_sides = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_sidesChoices, 0 );
	m_sides->SetSelection( 0 );
	fgSizer1->Add( m_sides, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


//	wxBoxSizer* all_sizer = new wxBoxSizer( wxVERTICAL );
//	all_sizer->Add( radio_sizer, 0, wxALL, 0 );
//	all_sizer->Add( map_sizer, 0, wxALL, 0 );
//	all_sizer->Add( sides_sizer, 0, wxALL, 0 );

	this->SetSizer( fgSizer1 );


//    SetSize( m_bg_img.GetWidth(), m_bg_img.GetHeight() );
    this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_scenario_choice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SkirmishDialog::OnScenarioChoice ), NULL, this );
//	m_map_random->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SkirmishDialog::OnRandom ), NULL, this );

	std::srand(time(NULL));

}

SkirmishDialog::~SkirmishDialog()
{
	// Disconnect Events
	m_scenario_choice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SkirmishDialog::OnScenarioChoice ), NULL, this );
//	m_map_random->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SkirmishDialog::OnRandom ), NULL, this );
}

void SkirmishDialog::OnBack( wxCommandEvent&  )
{
    m_parent->ShowSP( false );
}

void SkirmishDialog::OnRandom( wxCommandEvent&  )
{
//    bool checked = m_map_random->IsChecked();
//    m_map->Enable( !checked );
}

void SkirmishDialog::OnScenarioChoice( wxCommandEvent&  )
{
    m_map->Clear();
    m_sides->Clear();
    OptionsWrapper::GameOption optFlag = OptionsWrapper::SkirmishOptions;
    wxArrayString maps = usync().GetMapList();
    wxArrayString sides = usync().GetSides( SLcustomizations().GetModname() );
    OptionsWrapper map_op = m_skirmishes[m_scenario_choice->GetStringSelection()];
    for ( IUnitSync::OptionMapListConstIter it = map_op.m_opts[optFlag].list_map.begin(); it != map_op.m_opts[optFlag].list_map.end(); ++it) {
	    mmOptionList current = it->second;
        if ( _T("suggested_maps") == current.key ) {
            maps = current.cbx_choices;
        }
        else if ( _T("suggested_sides") == current.key ) {
            sides = current.cbx_choices;
        }
    }

    for ( size_t i = 0; i < maps.Count() ; ++i ) {
        m_map->Append( maps[i] );
    }
    for ( size_t i = 0; i < sides.Count() ; ++i ) {
        m_sides->Append( sides[i] );
    }
    m_map->SetSelection( 0 );
    m_sides->SetSelection( 0 );

}

void SkirmishDialog::OnAdvanced( wxCommandEvent&  )
{
    ui().mw().ShowSingleplayer();
    ui().mw().Show( true );
    m_parent->Close();
}


void SkirmishDialog::OnStart( wxCommandEvent& )
{
    OptionsWrapper& opts = m_skirmishes[m_scenario_choice->GetStringSelection()];
    // this overwrites any modoptions with those found in the skirmish definition
    m_battle.CustomBattleOptions().MergeOptions( opts, OptionsWrapper::ModOption );

    //now add AIs
    OptionsWrapper::GameOption optFlag = OptionsWrapper::SkirmishOptions;
    // we need to store Sides for AIs first, so we can later add them in batch w/o needing to remember a mapping
    std::vector<wxString> ai_sides;
    for ( IUnitSync::OptionMapListConstIter it = opts.m_opts[optFlag].list_map.begin(); it != opts.m_opts[optFlag].list_map.end(); ++it) {
        mmOptionList current = it->second;
        if ( current.key == _T("ai_sides") ) {

            for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor ) {
                ai_sides.push_back( itor->name );
            }
        break;
        }
    }

    wxString default_ai = m_mod_customs.getSingleValue( _T("default_ai" ) );
    std::vector<wxString> ai_names ( ai_sides.size(), default_ai );
    for ( IUnitSync::OptionMapListConstIter it = opts.m_opts[optFlag].list_map.begin(); it != opts.m_opts[optFlag].list_map.end(); ++it) {
        mmOptionList current = it->second;
        if ( current.key == _T("ai_names") ) {
            for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor) {
                size_t idx = FromwxString<size_t>( itor->key );
                if ( idx < ai_sides.size() )
                    ai_names[idx] = itor->name;
            }
        break;
        }
    }

    for ( IUnitSync::OptionMapListConstIter it = opts.m_opts[optFlag].list_map.begin(); it != opts.m_opts[optFlag].list_map.end(); ++it) {
        mmOptionList current = it->second;
        if ( current.key == _T("ai_team_ids") ) {

            size_t i = 0;
            for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor, ++i ) {
                ASSERT_EXCEPTION( i < ai_sides.size(), _T("The setup is listing more AI opponents than AI sides") );
                wxString ai = ai_names.size() > i ? ai_names[i] : default_ai;
                m_battle.AddBot( ai, FromwxString<int>( itor->name ), ai_sides[i] );
            }
        break;
        }
    }

    User& me = m_battle.GetMe();
    me.BattleStatus().side = m_battle.GetSideIndex( m_sides->GetStringSelection() );

//    if ( m_map_random->IsChecked() )
//        m_map->SetSelection( std::rand() % ( m_map->GetCount() ) ); //if anyone complains about this not being a uniform distribution imma invoke stab-over-tcp ((c) BD )
    m_battle.SetHostMap( m_map->GetStringSelection() , _T("") );
    m_battle.StartSpring();
}
