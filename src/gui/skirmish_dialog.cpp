
#include "skirmish_dialog.h"

#include <wx/string.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/settings.h>

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

///////////////////////////////////////////////////////////////////////////

SkirmishDialog::SkirmishDialog( SimpleFront* parent, const wxIcon& app_icon, const wxBitmap& bg_img, const wxString& modname, OptionsWrapper& mod_customs, wxWindowID id )
    : wxPanel( (wxWindow*)parent, id ),
    m_mod_customs( mod_customs ),
    m_modname( modname ),
    m_bg_img( bg_img ),
    m_parent( parent )
{

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    m_battle.SetHostMod( m_modname, wxEmptyString );
    m_battle.CustomBattleOptions().loadOptions( OptionsWrapper::ModOption, m_modname );
	const wxString sk_dir = m_mod_customs.getSingleValue( _T("skirmish_directory"), OptionsWrapper::ModCustomizations );

    //this block populates the radiobox and loads the skirmish options into the map
    wxBoxSizer* radio_sizer = new wxBoxSizer( wxVERTICAL );
	OptionsWrapper::GameOption optFlag = OptionsWrapper::ModCustomizations;
    for ( IUnitSync::OptionMapListConstIter it = m_mod_customs.m_opts[optFlag].list_map.begin(); it != m_mod_customs.m_opts[optFlag].list_map.end(); ++it) {
	    mmOptionList current = it->second;
	    if ( _T("scenarios") == current.key ) {
            m_radioBox1 = new wxRadioBox( this, wxID_ANY, _("Select skirmish setup"), wxDefaultPosition, wxDefaultSize, current.cbx_choices, 1, wxRA_SPECIFY_ROWS );
            m_radioBox1->SetSelection( 0 );
            radio_sizer->Add( m_radioBox1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 10 );

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
    mmOptionList suggested_sides;
    OptionsWrapper map_op = m_skirmishes[m_radioBox1->GetStringSelection()];
    for ( IUnitSync::OptionMapListConstIter it = map_op.m_opts[optFlag].list_map.begin(); it != map_op.m_opts[optFlag].list_map.end(); ++it) {
	    mmOptionList current = it->second;
        if ( _T("suggested_maps") == current.key ) {
            suggested_maps = current;
        }
        else if ( _T("suggested_sides") == current.key ) {
            suggested_sides = current;
        }
    }

    wxBoxSizer* map_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_map_label = new wxStaticText( this, wxID_ANY, _("Map"), wxDefaultPosition, wxDefaultSize, 0 );
	m_map_label->Wrap( -1 );
	map_sizer->Add( m_map_label, 0, wxALIGN_CENTER_VERTICAL| wxALL, 5 );
	wxArrayString m_mapChoices = suggested_maps.cbx_choices.Count() > 0 ? suggested_maps.cbx_choices : usync().GetMapList() ;
	m_map = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_mapChoices, 0 );
	m_map->SetSelection( 0 );
	map_sizer->Add( m_map, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	m_map_random = new wxCheckBox( this, wxID_ANY, _("random"), wxDefaultPosition, wxDefaultSize, 0 );
	map_sizer->Add( m_map_random, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


    wxBoxSizer* sides_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_sides_label = new wxStaticText( this, wxID_ANY, _("Select faction"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sides_label->Wrap( -1 );
	sides_sizer->Add( m_sides_label, 0, wxALIGN_CENTER_VERTICAL| wxALL, 5 );

	wxArrayString m_sidesChoices = suggested_sides.cbx_choices.Count() > 0 ? suggested_sides.cbx_choices : usync().GetSides( m_modname ) ;
	m_sides = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_sidesChoices, 0 );
	m_sides->SetSelection( 0 );
	sides_sizer->Add( m_sides, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer* button_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_back = new wxButton( this, wxID_ANY, _("Back"), wxDefaultPosition, wxDefaultSize, 0 );
	button_sizer->Add( m_back, 0, wxALL, 5 );

	m_advanced = new wxButton( this, wxID_ANY, _("Advanced setup"), wxDefaultPosition, wxDefaultSize, 0 );
	button_sizer->Add( m_advanced, 0, wxALL, 5 );

	m_start = new wxButton( this, wxID_ANY, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_start->SetDefault();
	button_sizer->Add( m_start, 0, wxALL, 5 );

	wxBoxSizer* all_sizer = new wxBoxSizer( wxVERTICAL );
	all_sizer->Add( radio_sizer, 0, wxALL, 0 );
	all_sizer->Add( map_sizer, 0, wxALL, 0 );
	all_sizer->Add( sides_sizer, 0, wxALL, 0 );
	all_sizer->Add( button_sizer, 0, wxALL, 0 );

//	all_sizer->Add( 0, 0, 1, wxEXPAND, 0 );
//	all_sizer->Add( all_panel, 0 , wxALIGN_CENTER, 0 );
//	all_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

	this->SetSizer( all_sizer );


    PushEventHandler( new wxBackgroundBitmap( m_bg_img ) );
    SetSize( m_bg_img.GetWidth(), m_bg_img.GetHeight() );
    this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_back->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnBack ), NULL, this );
	m_advanced->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnAdvanced ), NULL, this );
	m_start->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnStart ), NULL, this );
	m_radioBox1->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( SkirmishDialog::OnRadioBox ), NULL, this );
	m_map_random->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SkirmishDialog::OnRandom ), NULL, this );

	std::srand(time(NULL));
}

SkirmishDialog::~SkirmishDialog()
{
	// Disconnect Events
	m_back->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnBack ), NULL, this );
	m_advanced->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnAdvanced ), NULL, this );
	m_start->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnStart ), NULL, this );
	m_radioBox1->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( SkirmishDialog::OnRadioBox ), NULL, this );
	m_map_random->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SkirmishDialog::OnRandom ), NULL, this );
}

void SkirmishDialog::OnBack( wxCommandEvent& event )
{
    m_parent->ShowSP( false );
}

void SkirmishDialog::OnRandom( wxCommandEvent& event )
{
    bool checked = m_map_random->IsChecked();
    m_map->Enable( !checked );
}

void SkirmishDialog::OnRadioBox( wxCommandEvent& event )
{
    m_map->Clear();
    m_sides->Clear();
    OptionsWrapper::GameOption optFlag = OptionsWrapper::SkirmishOptions;
    wxArrayString maps = usync().GetMapList();
    wxArrayString sides = usync().GetSides( m_modname );
    OptionsWrapper map_op = m_skirmishes[m_radioBox1->GetStringSelection()];
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

void SkirmishDialog::OnAdvanced( wxCommandEvent& event )
{
    ui().mw().ShowSingleplayer();
}


void SkirmishDialog::OnStart( wxCommandEvent& event )
{
    OptionsWrapper& opts = m_skirmishes[m_radioBox1->GetStringSelection()];
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
                if ( idx < ai_sides.size() && idx >= 0 )
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

    if ( m_map_random->IsChecked() )
        m_map->SetSelection( std::rand() % ( m_map->GetCount() ) ); //if anyone complains about this not being a uniform distribution imma invoke stab-over-tcp ((c) BD )
    m_battle.SetHostMap( m_map->GetStringSelection() , _T("") );
    m_battle.StartSpring();
}
