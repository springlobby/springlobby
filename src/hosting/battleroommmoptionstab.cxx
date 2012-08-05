
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/choicdlg.h>
#include <wx/defs.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/tipwin.h>
#include <wx/tooltip.h>
#include <wx/textctrl.h>
#include <map>

#include "utils/controls.h"
#include <lslutils/misc.h>
#include <lslunitsync/optionswrapper.h>
#include <lslunitsync/unitsync.h>
#include "battle.h"
#include "gui/spinctl/spinctrl.h"
#include "utils/customdialogs.h"
#include "server.h"
#include "settings.h"
#include "ui.h"
#include "aui/auimanager.h"


const char sep = *("_");
const wxString wxsep = _T("_");


// note that the SpinCtrlDouble change is hadnled explicitly via the control calling the right handler
BEGIN_EVENT_TABLE_TEMPLATE1( BattleroomMMOptionsTab, wxPanel, BattleType)
	EVT_COMBOBOX					(wxID_ANY, BattleroomMMOptionsTab::OnComBoxChange)
	EVT_CHECKBOX					(wxID_ANY, BattleroomMMOptionsTab::OnChkBoxChange)
	EVT_TEXT_ENTER					(wxID_ANY,  BattleroomMMOptionsTab::OnTextCtrlChange)

//  EVT_BUTTON( BOPTS_LOADPRES, BattleroomMMOptionsTab::OnLoadPreset )
//  EVT_BUTTON( BOPTS_SAVEPRES, BattleroomMMOptionsTab::OnSavePreset )
//  EVT_BUTTON( BOPTS_DELETEPRES, BattleroomMMOptionsTab::OnDeletePreset )
//  EVT_BUTTON( BOPTS_SETDEFAULTPRES, BattleroomMMOptionsTab::OnSetModDefaultPreset )

  EVT_BUTTON( wxID_ANY, BattleroomMMOptionsTab::OnButton )
END_EVENT_TABLE()

template < class BattleType >
BattleroomMMOptionsTab<BattleType>::BattleroomMMOptionsTab(  BattleType* battle, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxScrolledWindow( parent, id, pos, size, style | wxHSCROLL ),m_battle(battle)
{
  GetAui().manager->AddPane( this, wxLEFT, _T("battleroommmoptionstab") );

	m_main_sizer = new wxBoxSizer( wxVERTICAL );

  wxStaticBoxSizer* m_preset_sizer;
  m_preset_sizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Manage Presets") ), wxHORIZONTAL );

  m_options_preset_sel = new wxComboBox( this, BOPTS_CHOSEPRES, _T(""), wxDefaultPosition, wxDefaultSize,  sett().GetPresetList(), wxCB_READONLY );
  m_options_preset_sel->SetToolTip(TE(_("Set name.")));

  m_preset_sizer->Add( m_options_preset_sel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

  m_load_btn = new wxButton( this, BOPTS_LOADPRES, _("Load..."), wxDefaultPosition, wxDefaultSize, 0 );
  m_load_btn->SetToolTip( TE(_("Load a saved set of options.")) );

  m_preset_sizer->Add( m_load_btn, 0, wxALL, 5 );

  m_save_btn = new wxButton( this, BOPTS_SAVEPRES, _("Save..."), wxDefaultPosition, wxDefaultSize, 0 );
  m_save_btn->SetToolTip( TE(_("Save a set of options.")) );

  m_preset_sizer->Add( m_save_btn, 0, wxALL, 5 );

  m_delete_btn = new wxButton( this, BOPTS_DELETEPRES, _("Delete..."), wxDefaultPosition, wxDefaultSize, 0 );
  m_delete_btn->SetToolTip( TE(_("Delete a set of options.")) );

  m_preset_sizer->Add( m_delete_btn, 0, wxALL, 5 );

  m_default_btn = new wxButton( this, BOPTS_SETDEFAULTPRES, _("Set default..."), wxDefaultPosition, wxDefaultSize, 0 );
  m_default_btn->SetToolTip( TE(_("Use the current set of options as game's default.")) );

  m_preset_sizer->Add( m_default_btn, 0, wxALL, 5 );

  m_preset_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

    m_map_mod_container = new wxBoxSizer( wxVERTICAL );

	m_mod_options_sizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Game Options") ), wxVERTICAL );
	m_mod_layout = new wxBoxSizer( wxVERTICAL);
    setupOptionsSizer(m_mod_layout, LSL::OptionsWrapper::ModOption);
	m_mod_options_sizer->Add( m_mod_layout, 1, wxEXPAND, 5 );

	m_map_options_sizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Map Options") ), wxVERTICAL );
	m_map_layout = new wxBoxSizer( wxVERTICAL);
    setupOptionsSizer(m_map_layout, LSL::OptionsWrapper::MapOption);
	m_map_options_sizer->Add( m_map_layout, 1, wxEXPAND, 5 );


    m_main_sizer->Add( m_preset_sizer, 0, wxALL|wxEXPAND, 5 );
	m_map_mod_container->Add( m_mod_options_sizer, 0, wxALL|wxEXPAND, 5 );
	m_map_mod_container->Add( m_map_options_sizer, 0, wxALL|wxEXPAND, 5 );
	m_main_sizer->Add( m_map_mod_container, 1, wxALL|wxEXPAND, 5 );


  //m_main_sizer->FitInside(this);

  SetBattle( battle );

    SetScrollRate( SCROLL_RATE, SCROLL_RATE );
	SetSizer( m_main_sizer );
	Layout();


}

template < class BattleType >
BattleroomMMOptionsTab<BattleType>::~BattleroomMMOptionsTab()
{
    if(GetAui().manager)GetAui().manager->DetachPane( this );
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::setupOptionsSizer( wxBoxSizer* parent_sizer,
                                                            LSL::OptionsWrapper::GameOption optFlag )
{
    if ( !m_battle )
        return;
    const auto& sections = m_battle->CustomBattleOptions().m_opts[optFlag].section_map;

    unsigned int num_options = 0;
    auto it = sections.begin();
    for ( ; it != sections.end(); ++it )
    {
        wxStaticBoxSizer* section_sizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, it->second.name ), wxVERTICAL );
        //only add non-empty sizer
        if ( setupOptionsSectionSizer( it->second, section_sizer, optFlag ) ) {
            parent_sizer->Add( section_sizer, 0 , wxALL, section_sizer->GetChildren().size() > 0 ? 5 : 0 );
            num_options++;
        }
        else
            delete section_sizer;
    }

    //adds options with no asociated section
    LSL::mmOptionSection dummy;
    wxBoxSizer* section_sizer = new wxBoxSizer( wxVERTICAL );
    if ( setupOptionsSectionSizer( dummy, section_sizer, optFlag ) == 0 ) {
        if ( num_options == 0 ) {
			wxString name = wxFormat( _T("%d%sno_opts") ) %optFlag % wxsep;
            wxStaticText* none_found = new wxStaticText( this, wxID_ANY, _("no options available"),
                                            wxDefaultPosition, wxDefaultSize, 0, name  );
            m_statictext_map[name] = none_found;
            parent_sizer->Add( none_found, 0, wxALL, 3 );
        }
    }
    else {
        wxStaticBoxSizer* other_section = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("other") ), wxVERTICAL );
        other_section->Add( section_sizer, 0 , wxALL, section_sizer->GetChildren().size() > 0 ? 5 : 0 );
        parent_sizer->Add( other_section, 0 , wxALL, 0 );
    }

}

template < class BattleType >
wxButton* BattleroomMMOptionsTab<BattleType>::getButton( const wxWindowID id, const wxString& name )
{
		if ( !m_battle ) return 0;
    m_button_map[name] = new wxButton(this, id + BUTTON_ID_OFFSET, _T("?"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxBU_EXACTFIT, wxDefaultValidator, name );
    return m_button_map[name];
}

template < class BattleType >
int BattleroomMMOptionsTab<BattleType>::setupOptionsSectionSizer(const LSL::mmOptionSection& section,
    wxBoxSizer* parent_sizer, LSL::OptionsWrapper::GameOption optFlag)
{
	if ( !m_battle ) return -1;
    const int col_gap = 35;
	wxString pref = wxFormat( _T("%d%s") ) % optFlag % wxsep;
    LSL::OptionsWrapper optWrap = m_battle->CustomBattleOptions();
	bool enable = m_battle->IsFounderMe();
	wxFlexGridSizer* cbxSizer =  new wxFlexGridSizer( 4, 2, 10, 10 );
	wxFlexGridSizer* spinSizer =  new wxFlexGridSizer( 4, 10, 10 );
	wxFlexGridSizer* textSizer =  new wxFlexGridSizer( 4, 10, 10 );
	wxFlexGridSizer* chkSizer = new wxFlexGridSizer( 4, 10, 10 );

    const int b_gap = 1;

    int total_count = 0;
	int ctrl_count = 0;
    for (auto i = optWrap.m_opts[optFlag].bool_map.begin(); i != optWrap.m_opts[optFlag].bool_map.end();++i)
    {
        if ( i->second.section == section.key )
        {
            LSL::mmOptionBool current = i->second;
			wxCheckBox* temp = new wxCheckBox(this,BOOL_START_ID+ctrl_count,current.name);
            temp->SetToolTip(TES(current.description));
            m_name_info_map[pref+TowxString(current.key)] = TowxString(current.description);
            temp->SetName(pref+TowxString(current.key));
            m_chkbox_map[pref+TowxString(current.key)] = temp;
            temp->SetValue(TowxString(current.value));
			temp->Enable(enable);
			wxBoxSizer* ct_sizer = new wxBoxSizer( wxHORIZONTAL );
			ct_sizer->Add(temp, 0, wxRIGHT| wxALIGN_CENTER_VERTICAL, b_gap);
            ct_sizer->Add(getButton(BOOL_START_ID+ctrl_count,pref+TowxString(current.key)), 0, wxRIGHT| wxALIGN_CENTER_VERTICAL, col_gap);
			chkSizer->Add( ct_sizer );
			ctrl_count++;
        }
	}

    total_count += ctrl_count;
	ctrl_count = 0;
    for ( auto it = optWrap.m_opts[optFlag].float_map.begin(); it != optWrap.m_opts[optFlag].float_map.end(); ++it)
	{
	    wxString seckey = it->second.section;
	    wxString kkey = section.key ;
        if ( it->second.section == section.key )
        {
            LSL::mmOptionFloat current = it->second;
			SlSpinCtrlDouble<ThisType>* tempspin = new SlSpinCtrlDouble<ThisType>();
			tempspin->Create(this, FLOAT_START_ID+ctrl_count, _T(""),
					wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, double(current.min), double(current.max),
                    double(current.value),double(current.stepping), TowxString(current.key));
            tempspin->SetSnapToTicks( true );
            tempspin->SetToolTip(TES(current.description));
            const wxString pref_key = pref + TowxString(current.key);
            m_name_info_map[pref_key] = current.description;
			tempspin->Enable(enable);
            tempspin->SetName(pref_key);
            m_spinctrl_map[pref_key] = tempspin;
			 wxStaticText* tempst = new wxStaticText(this,-1,current.name);
             m_statictext_map[pref_key] = tempst;
			spinSizer->Add(tempst,0, wxALIGN_CENTER_VERTICAL);
			wxBoxSizer* ct_sizer = new wxBoxSizer( wxHORIZONTAL );
			ct_sizer->Add(tempspin, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, b_gap);
            ct_sizer->Add(getButton(FLOAT_START_ID+ctrl_count,pref_key), 0, wxRIGHT , col_gap);
			spinSizer->Add( ct_sizer );
			ctrl_count++;
        }
	}

    total_count += ctrl_count;
	ctrl_count = 0;
    for ( auto it = optWrap.m_opts[optFlag].list_map.begin(); it != optWrap.m_opts[optFlag].list_map.end(); ++it)
	{
	    if ( it->second.section == section.key )
        {
            LSL::mmOptionList current = it->second;

            int temp = int(current.cbx_choices.size()-1);
            int index = LSL::Util::Clamp(current.cur_choice_index,0,temp);
            wxComboBox* tempchoice = new wxComboBox(this, LIST_START_ID+ctrl_count,
                                                    current.cbx_choices[index], wxDefaultPosition,
                                                    wxDefaultSize, current.cbx_choices, wxCB_READONLY, wxDefaultValidator);
            wxString tooltip = TowxString(current.description + std::string("\n"));
            for ( LSL::ListItemVec::const_iterator itor = current.listitems.begin();
                  itor != current.listitems.end(); ++itor )
            {
                tooltip+= std::string("\n") + itor->name + std::string(": ") + itor->desc;
            }
            tempchoice->SetToolTip(TE(tooltip));
            const wxString pref_key = pref + TowxString(current.key);
            m_name_info_map[pref_key] = tooltip;
            tempchoice->SetName(pref_key);
            tempchoice->Enable(enable);
            m_combox_map[pref_key] = tempchoice;
            wxStaticText* tempst = new wxStaticText(this,-1,current.name);
            m_statictext_map[pref_key] = tempst;
            cbxSizer->Add(tempst,0, wxALIGN_CENTER_VERTICAL);
            wxBoxSizer* ct_sizer = new wxBoxSizer( wxHORIZONTAL );
            ct_sizer->Add(tempchoice, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, b_gap);
            ct_sizer->Add(getButton(LIST_START_ID+ctrl_count,pref_key), 0,  wxRIGHT, col_gap);
            cbxSizer->Add( ct_sizer );

            ctrl_count++;
        }
	}

    total_count += ctrl_count;
	ctrl_count = 0;
    for ( auto it = optWrap.m_opts[optFlag].string_map.begin();
          it != optWrap.m_opts[optFlag].string_map.end(); ++it)
	{
	    if ( it->second.section == section.key )
        {
            LSL::mmOptionString current = it->second;
            wxTextCtrl* temptext = new wxTextCtrl(this, STRING_START_ID+ctrl_count, TowxString(current.value), wxDefaultPosition,
                    wxDefaultSize, 0, wxDefaultValidator, TowxString(current.key));
            temptext->SetToolTip(TES(current.description));
            const wxString pref_key = pref + TowxString(current.key);
            m_name_info_map[pref_key] = current.description;
            temptext->SetName(pref_key);
            temptext->Enable(enable);
            m_textctrl_map[pref_key] = temptext;
            wxStaticText* tempst = new wxStaticText(this,-1,TowxString(current.name));
            m_statictext_map[pref_key] = tempst;
            textSizer->Add(tempst,0, wxALIGN_CENTER_VERTICAL);
            wxBoxSizer* ct_sizer = new wxBoxSizer( wxHORIZONTAL );
            ct_sizer->Add(temptext,0, wxALIGN_CENTER_VERTICAL | wxRIGHT, b_gap);
            ct_sizer->Add(getButton(STRING_START_ID+ctrl_count,pref_key),0, wxRIGHT, col_gap);
            textSizer->Add( ct_sizer );

            ctrl_count++;
        }
	}
    total_count += ctrl_count;

	parent_sizer->Add( chkSizer, 0, wxALL, chkSizer->GetChildren().size() > 0 ? 5 : 0 );
	parent_sizer->Add( spinSizer, 0, wxALL, spinSizer->GetChildren().size() > 0 ? 5 : 0 );
	parent_sizer->Add( cbxSizer, 0, wxALL, cbxSizer->GetChildren().size() > 0 ? 5 : 0 );
	parent_sizer->Add( textSizer, 0, wxALL, textSizer->GetChildren().size() > 0 ? 5 : 0 );

    return total_count;

}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnChkBoxChange(wxCommandEvent& event)
{
	if ( !m_battle ) return;
	wxCheckBox* box = (wxCheckBox*) event.GetEventObject();
	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption ;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);

    if( m_battle->CustomBattleOptions().setSingleOption( key , (box->GetValue() ? _T("1") : _T("0")),
                                                         (LSL::OptionsWrapper::GameOption)gameoption ) )
	{
        if (m_battle->IsFounderMe())
        {
          m_battle->SendHostInfo( (wxString()<<gameoption) + wxsep + key );
        }
	}
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnComBoxChange(wxCommandEvent& event)
{
	if ( !m_battle ) return;
	wxComboBox* box = (wxComboBox*) event.GetEventObject();

	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);
    wxString itemKey = m_battle->CustomBattleOptions().GetNameListOptItemKey(key,  box->GetValue(),
                                                                             (LSL::OptionsWrapper::GameOption)gameoption );

    if(m_battle->CustomBattleOptions().setSingleOption( key, itemKey,
                                                        (LSL::OptionsWrapper::GameOption)gameoption ) )
	{
        if (m_battle->IsFounderMe())
        {
          m_battle->SendHostInfo( (wxString()<< gameoption) + wxsep + key );
        }
	}
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnTextCtrlChange(wxCommandEvent& event)
{
	if ( !m_battle ) return;
	wxTextCtrl* box = (wxTextCtrl*) event.GetEventObject();
	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);
    if(m_battle->CustomBattleOptions().setSingleOption( key, box->GetValue(),
                                                        (LSL::OptionsWrapper::GameOption)gameoption ) )
	{
		if (m_battle->IsFounderMe())
		{
		  m_battle->SendHostInfo(  (wxString()<< gameoption) + wxsep + key );
		}

	}
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnSpinCtrlDoubleChange(SlSpinDoubleEvent& event)
{
	if ( !m_battle ) return;
	SlSpinCtrlDouble<ThisType>* box = (SlSpinCtrlDouble<ThisType>*) event.GetEventObject();
	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);
    if(m_battle->CustomBattleOptions().setSingleOption( key,wxFormat( _T("%f") ) % box->GetValue(),
                                                        (LSL::OptionsWrapper::GameOption)gameoption ) )
	{
		if (m_battle->IsFounderMe())
		{
		  m_battle->SendHostInfo(  (wxString()<< gameoption) + wxsep + key );
		}
	}
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::UpdateOptControls(wxString controlName)
{
	if ( !m_battle ) return;
	long gameoption;
	controlName.BeforeFirst(sep).ToLong(&gameoption);
	wxString optKey = controlName.AfterFirst(sep);

    if ( gameoption == LSL::OptionsWrapper::PrivateOptions )
	{
    if ( optKey == _T("mapname") ) OnReloadControls( LSL::OptionsWrapper::MapOption );
    if ( optKey == _T("modname") ) OnReloadControls( LSL::OptionsWrapper::ModOption );
    return;
	}

	if ( m_chkbox_map.find(controlName) != m_chkbox_map.end() )
	{
        wxString value = m_battle->CustomBattleOptions().getSingleValue( optKey, (LSL::OptionsWrapper::GameOption)gameoption );
		wxCheckBox* cur = m_chkbox_map[controlName] ;
		long l_val;
		value.ToLong(&l_val);
		cur->SetValue(l_val);
	}

	 if ( m_combox_map.find(controlName) != m_combox_map.end() )
	{
        wxString value = m_battle->CustomBattleOptions().getSingleValue( optKey, (LSL::OptionsWrapper::GameOption)gameoption );
		wxComboBox* cur = m_combox_map[controlName];
        cur->SetValue(m_battle->CustomBattleOptions().GetNameListOptValue( optKey, (LSL::OptionsWrapper::GameOption)gameoption));
	}

	 if ( m_textctrl_map.find(controlName) != m_textctrl_map.end() )
	{
        wxString value = m_battle->CustomBattleOptions().getSingleValue( optKey, (LSL::OptionsWrapper::GameOption)gameoption );
		wxTextCtrl* cur = m_textctrl_map[controlName];
		cur->SetValue(value);
	}

	 if ( m_spinctrl_map.find(controlName) != m_spinctrl_map.end() )
	{
        wxString value = m_battle->CustomBattleOptions().getSingleValue( optKey, (LSL::OptionsWrapper::GameOption)gameoption );
		SlSpinCtrlDouble<ThisType>* cur = m_spinctrl_map[controlName] ;
		double l_val;
		value.ToDouble(&l_val);
		cur->SetValue(l_val);
	}

}

template<class T>
void RemovePrefixed(T &v, wxString pref){
  typename T::iterator it = v.begin();
  while(it != v.end())
  {
    typename T::iterator next = it;
    ++next;
    wxString key = it->first;
    if (key.StartsWith(pref))
    {
      delete it->second;
      v.erase(it);
    }
    it=next;
  }
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnReloadControls(LSL::OptionsWrapper::GameOption flag)
{
	if ( !m_battle ) return;
	wxString pref = wxFormat( _T("%d%s") ) % flag % wxsep;

	//purgin existing keys from map
	RemovePrefixed(m_chkbox_map,pref);
	RemovePrefixed(m_spinctrl_map,pref);
	RemovePrefixed(m_textctrl_map,pref);
	RemovePrefixed(m_combox_map,pref);
	RemovePrefixed(m_statictext_map,pref);
	RemovePrefixed(m_button_map,pref);

	//reloading the controls
	switch (flag)
	{
        case LSL::OptionsWrapper::ModOption:
			m_mod_options_sizer->Remove(m_mod_layout);
			m_mod_layout = new wxBoxSizer( wxVERTICAL);
            setupOptionsSizer(m_mod_layout, LSL::OptionsWrapper::ModOption);
			//m_mod_options_sizer->Add( m_mod_options_sizer, 1, wxEXPAND, 5 );
			m_mod_options_sizer->Add( m_mod_layout, 1, wxALL|wxEXPAND, 5 );
			break;
        case LSL::OptionsWrapper::MapOption:
			m_map_options_sizer->Remove(m_map_layout);
			m_map_layout = new wxBoxSizer( wxVERTICAL);
            setupOptionsSizer(m_map_layout, LSL::OptionsWrapper::MapOption);
			m_map_options_sizer->Add( m_map_layout, 1, wxALL|wxEXPAND, 5 );
			break;
		default:
			break;
	}


	//this->SetSizer( m_main_sizer, true );
	m_main_sizer->FitInside(this);

	this->Layout();
    SetScrollbars( 10, 10, 62, 62 );
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnReloadControls()
{
		if ( !m_battle ) return;
    for ( unsigned int i = 0; i < LSL::OptionsWrapper::LastOption; i++)
        OnReloadControls( (LSL::OptionsWrapper::GameOption) i );
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnLoadPreset( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
  wxString presetname = m_options_preset_sel->GetValue();
  if ( presetname.IsEmpty() )
  {
     customMessageBoxNoModal( SL_MAIN_ICON , _("Cannot load an options set without a name\nPlease select one from the list and try again."), _("error"), wxICON_EXCLAMATION|wxOK );
     return;
  }
  m_battle->LoadOptionsPreset( presetname );
  m_battle->SendHostInfo( IBattle::HI_Send_All_opts );
  OnReloadControls( );
}


template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnSavePreset( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
    wxString presetname;
	if ( !ui().AskText( _("Enter preset name"), _("Enter a name to save the current set of options\nIf a preset with the same name already exist, it will be overwritten"), presetname ) )
        return;
    if ( presetname.IsEmpty() )
    {
        customMessageBoxNoModal( SL_MAIN_ICON , _("Cannot save an options set without a name."), _("error"), wxICON_EXCLAMATION|wxOK );
        return;
    }
    m_battle->SaveOptionsPreset( presetname );
}


template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnDeletePreset( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
  wxArrayString choices = m_battle->GetPresetList();
	int result = wxGetSingleChoiceIndex(_("Pick an existing option set from the list"),_("Delete preset"), choices );
	if ( result < 0 ) return;
  m_battle->DeletePreset( choices[result] );
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnSetModDefaultPreset( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
  wxArrayString choices = m_battle->GetPresetList();
	int result = wxGetSingleChoiceIndex(_("Pick an existing option set from the list"),_("Set game default preset"), choices );
	if ( result < 0 ) return;
  sett().SetModDefaultPresetName( m_battle->GetHostModName(), choices[result] );
}


template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::UpdatePresetList()
{
		if ( !m_battle ) return;
    m_options_preset_sel->Clear();
    m_options_preset_sel->Append(sett().GetPresetList());
    m_options_preset_sel->SetStringSelection(  m_battle->GetCurrentPreset() );
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnButton( wxCommandEvent& event )
{
    switch ( event.GetId() ) {
        case BOPTS_LOADPRES: OnLoadPreset ( event ); break;
        case BOPTS_SAVEPRES: OnSavePreset ( event ); break;
        case BOPTS_DELETEPRES: OnDeletePreset ( event ); break;
        case BOPTS_SETDEFAULTPRES: OnSetModDefaultPreset ( event ); break;
        default: OnInfoButton( event ); break;

    }

}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnInfoButton( wxCommandEvent& event )
{
		if ( !m_battle ) return;
    #ifdef wxUSE_TIPWINDOW
    wxWindow* button = (wxWindow*) event.GetEventObject();
    if ( button ) {
        nameInfoMap::const_iterator iter = m_name_info_map.find( button->GetName() );
        if ( iter != m_name_info_map.end() ) {
            //needs to be moved a little away from cursor pos
            wxPoint pos =  wxGetMousePosition();
            wxTipWindow* tip = new wxTipWindow ( this, iter->second , 1000 );
            tip->Move( pos.x, pos.y - tip->GetSize().GetHeight() );
        }
    }
    #endif
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::SetBattle( BattleType* battle )
{
	m_battle = battle;
	m_options_preset_sel->Enable(m_battle);
	m_load_btn->Enable(m_battle);
	m_save_btn->Enable(m_battle);
	m_delete_btn->Enable(m_battle);
	m_default_btn->Enable(m_battle);
	for ( chkBoxMap::iterator itor = m_chkbox_map.begin(); itor != m_chkbox_map.end(); ++itor ) itor->second->Enable(m_battle);
	for ( comboBoxMap::iterator itor = m_combox_map.begin(); itor != m_combox_map.end(); ++itor ) itor->second->Enable(m_battle);
	for ( typename spinCtrlMap::iterator itor = m_spinctrl_map.begin(); itor != m_spinctrl_map.end(); ++itor ) itor->second->Enable(m_battle);
	for ( textCtrlMap::iterator itor = m_textctrl_map.begin(); itor != m_textctrl_map.end(); ++itor ) itor->second->Enable(m_battle);
	for ( buttonMap::iterator itor = m_button_map.begin(); itor != m_button_map.end(); ++itor ) itor->second->Enable(m_battle);

	if ( m_battle )
	{
		m_options_preset_sel->SetStringSelection( sett().GetModDefaultPresetName( m_battle->GetHostModName() ) );
    if ( !m_battle->IsFounderMe() )
    {
        m_options_preset_sel->Disable();
        m_load_btn->Disable();
        m_save_btn->Disable();
        m_delete_btn->Disable();
        m_default_btn->Disable();
    }
    OnReloadControls();
	}
}

template < class BattleType >
BattleType* BattleroomMMOptionsTab<BattleType>::GetBattle()
{
	return m_battle;
}

