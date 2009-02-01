
#include "battleroommmoptionstab.h"

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/defs.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <map>

#include "utils.h"
#include "mmoptionswrapper.h"
#include "battle.h"
#include "spinctld.h"
#include "settings++/custom_dialogs.h"
#include "server.h"
#include "settings.h"

#ifndef HAVE_WX26
#include "aui/auimanager.h"
#endif

const char sep = *("_");
const wxString wxsep = _T("_");

BEGIN_EVENT_TABLE_TEMPLATE1( BattleroomMMOptionsTab, wxPanel, BattleType)
	EVT_COMBOBOX					(wxID_ANY, BattleroomMMOptionsTab::OnComBoxChange)
	EVT_CHECKBOX					(wxID_ANY, BattleroomMMOptionsTab::OnChkBoxChange)
	EVT_TEXT_ENTER					(wxID_ANY,  BattleroomMMOptionsTab::OnTextCtrlChange)
	EVT_SPINCTRL					(wxID_ANY,  BattleroomMMOptionsTab::OnSpinCtrlChange)

  EVT_BUTTON( BOPTS_LOADPRES, BattleroomMMOptionsTab::OnLoadPreset )
  EVT_BUTTON( BOPTS_SAVEPRES, BattleroomMMOptionsTab::OnSavePreset )
  EVT_BUTTON( BOPTS_DELETEPRES, BattleroomMMOptionsTab::OnDeletePreset )
  EVT_BUTTON( BOPTS_SETDEFAULTPRES, BattleroomMMOptionsTab::OnSetModDefaultPreset )

END_EVENT_TABLE()

template < class BattleType >
BattleroomMMOptionsTab<BattleType>::BattleroomMMOptionsTab(  BattleType& battle, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxScrolledWindow( parent, id, pos, size, style | wxHSCROLL ),m_battle(battle)
{
  #ifndef HAVE_WX26
  GetAui().manager->AddPane( this, wxLEFT, _T("battleroommmoptionstab") );
  #endif
	m_main_sizer = new wxBoxSizer( wxVERTICAL );

  wxStaticBoxSizer* m_preset_sizer;
  m_preset_sizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Manage Presets") ), wxHORIZONTAL );

  m_options_preset_sel = new wxComboBox( this, BOPTS_CHOSEPRES, sett().GetModDefaultPresetName( m_battle.GetHostModName() ), wxDefaultPosition, wxDefaultSize,  sett().GetPresetList() );
  m_options_preset_sel->SetToolTip(TE(_("Set name.")));

  m_preset_sizer->Add( m_options_preset_sel, 0, wxALL, 5 );

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
  m_default_btn->SetToolTip( TE(_("Use the current set of options as mod's default.")) );

  m_preset_sizer->Add( m_default_btn, 0, wxALL, 5 );

  m_preset_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

    m_map_mod_container = new wxBoxSizer( wxVERTICAL );

	m_mod_options_sizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Mod Options") ), wxVERTICAL );
	m_mod_layout = new wxBoxSizer( wxVERTICAL);
	setupOptionsSizer(m_mod_layout,OptionsWrapper::ModOption);
	m_mod_options_sizer->Add( m_mod_layout, 1, wxEXPAND, 5 );

	m_map_options_sizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Map Options") ), wxVERTICAL );
	m_map_layout = new wxBoxSizer( wxVERTICAL);
	setupOptionsSizer(m_map_layout,OptionsWrapper::MapOption);
	m_map_options_sizer->Add( m_map_layout, 1, wxEXPAND, 5 );


    m_main_sizer->Add( m_preset_sizer, 0, wxALL|wxEXPAND, 5 );
	m_map_mod_container->Add( m_mod_options_sizer, 0, wxALL|wxEXPAND, 5 );
	m_map_mod_container->Add( m_map_options_sizer, 0, wxALL|wxEXPAND, 5 );
	m_main_sizer->Add( m_map_mod_container, 1, wxALL|wxEXPAND, 5 );


  //m_main_sizer->FitInside(this);


	this->SetSizer( m_main_sizer );
	this->Layout();

  if ( !m_battle.IsFounderMe() )
  {
    m_options_preset_sel->Disable();
    m_load_btn->Disable();
    m_save_btn->Disable();
    m_delete_btn->Disable();
    m_default_btn->Disable();
  }

	//SetScrollbars( 10, 10, 62, 62 );
}

template < class BattleType >
BattleroomMMOptionsTab<BattleType>::~BattleroomMMOptionsTab()
{
  #ifndef HAVE_WX26
  if(GetAui().manager)GetAui().manager->DetachPane( this );
  #endif
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::setupOptionsSizer( wxBoxSizer* parent_sizer, OptionsWrapper::GameOption optFlag )
{
    const IUnitSync::OptionMapSection& sections = m_battle.CustomBattleOptions().opts[optFlag].section_map;

    unsigned int num_options = 0;
    IUnitSync::OptionMapSectionConstIter it = sections.begin();
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
    mmOptionSection dummy;
    wxBoxSizer* section_sizer = new wxBoxSizer( wxVERTICAL );
    if ( setupOptionsSectionSizer( dummy, section_sizer, optFlag ) == 0 ) {
        if ( num_options == 0 ) {
            wxString name = wxString::Format( _T("%d"),optFlag) + wxsep + _T("no_opts");
            wxStaticText* none_found = new wxStaticText( this, wxID_ANY, _("no options available"),
                                            wxDefaultPosition, wxDefaultSize, 0, name  );
            m_statictext_map[name] = none_found;
            parent_sizer->Add( none_found, 0, wxALL, 3 );
        }
    }
    else {
       parent_sizer->Add( section_sizer, 0 , wxALL, section_sizer->GetChildren().size() > 0 ? 5 : 0 );
    }

}

template < class BattleType >
int BattleroomMMOptionsTab<BattleType>::setupOptionsSectionSizer(const mmOptionSection& section,
    wxBoxSizer* parent_sizer, OptionsWrapper::GameOption optFlag)
{
    const int col_gap = 35;
	wxString pref = wxString::Format( _T("%d"),optFlag) + wxsep;
	OptionsWrapper optWrap = m_battle.CustomBattleOptions();
	bool enable = m_battle.IsFounderMe();
	wxFlexGridSizer* cbxSizer =  new wxFlexGridSizer( 4, 2, 10, 10 );
	wxFlexGridSizer* spinSizer =  new wxFlexGridSizer( 4, 10, 10 );
	wxFlexGridSizer* textSizer =  new wxFlexGridSizer( 4, 10, 10 );
	wxFlexGridSizer* chkSizer = new wxFlexGridSizer( 4, 10, 10 );

    int total_count = 0;
	int ctrl_count = 0;
	for (IUnitSync::OptionMapBoolIter i = optWrap.opts[optFlag].bool_map.begin(); i != optWrap.opts[optFlag].bool_map.end();++i)
    {
        if ( i->second.section == section.key )
        {
			mmOptionBool current = i->second;
			wxCheckBox* temp = new wxCheckBox(this,BOOL_START_ID+ctrl_count,current.name);
			temp->SetToolTip(TE(current.description));
			temp->SetName(pref+current.key);
			m_chkbox_map[pref+current.key] = temp;
			temp->SetValue(current.value);
			temp->Enable(enable);
			chkSizer->Add(temp, 0, wxRIGHT, col_gap);
			ctrl_count++;
        }
	}

    total_count += ctrl_count;
	ctrl_count = 0;
	for ( IUnitSync::OptionMapFloatIter it = optWrap.opts[optFlag].float_map.begin(); it != optWrap.opts[optFlag].float_map.end(); ++it)
	{
	    wxString seckey = it->second.section;
	    wxString kkey = section.key ;
        if ( it->second.section == section.key )
        {
			mmOptionFloat current = it->second;
			wxSpinCtrlDbl* tempspin = new wxSpinCtrlDbl();
			tempspin->Create(this, FLOAT_START_ID+ctrl_count, _T(""),
					wxDefaultPosition, wxDefaultSize, 0, double(current.min), double(current.max),
					double(current.value),double(current.stepping), wxSPINCTRLDBL_AUTODIGITS, current.key);
			tempspin->SetToolTip(TE(current.description));
			tempspin->Enable(enable);
			tempspin->SetName(pref+current.key);
			m_spinctrl_map[pref+current.key] = tempspin;
			 wxStaticText* tempst = new wxStaticText(this,-1,current.name);
			 m_statictext_map[pref+current.key] = tempst;
			spinSizer->Add(tempst,0, wxALIGN_CENTER_VERTICAL);
			spinSizer->Add(tempspin, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, col_gap);
			ctrl_count++;
        }
	}

    total_count += ctrl_count;
	ctrl_count = 0;
	for ( IUnitSync::OptionMapListIter it = optWrap.opts[optFlag].list_map.begin(); it != optWrap.opts[optFlag].list_map.end(); ++it)
	{
	    if ( it->second.section == section.key )
        {
            mmOptionList current = it->second;

            int temp = int(current.cbx_choices.GetCount()-1);
            int index = CLAMP(current.cur_choice_index,0,temp);
            wxComboBox* tempchoice = new wxComboBox(this, LIST_START_ID+ctrl_count, current.cbx_choices[index], wxDefaultPosition,
                    wxDefaultSize, current.cbx_choices, wxCB_READONLY, wxDefaultValidator);

            tempchoice->SetToolTip(TE(current.description));
            tempchoice->SetName(pref+current.key);
            tempchoice->Enable(enable);
            m_combox_map[pref+current.key] = tempchoice;
            wxStaticText* tempst = new wxStaticText(this,-1,current.name);
            m_statictext_map[pref+current.key] = tempst;
            cbxSizer->Add(tempst,0, wxALIGN_CENTER_VERTICAL);
            cbxSizer->Add(tempchoice, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, col_gap);

            ctrl_count++;
        }
	}

    total_count += ctrl_count;
	ctrl_count = 0;
	for ( IUnitSync::OptionMapStringIter it = optWrap.opts[optFlag].string_map.begin(); it != optWrap.opts[optFlag].string_map.end(); ++it)
	{
	    if ( it->second.section == section.key )
        {
            mmOptionString current = it->second;
            wxTextCtrl* temptext = new wxTextCtrl(this, STRING_START_ID+ctrl_count, current.value, wxDefaultPosition,
                    wxDefaultSize, 0, wxDefaultValidator, current.key);
            temptext->SetToolTip(TE(current.description));
            temptext->SetName(pref+current.key);
            temptext->Enable(enable);
            m_textctrl_map[pref+current.key] = temptext;
            wxStaticText* tempst = new wxStaticText(this,-1,current.name);
            m_statictext_map[pref+current.key] = tempst;
            textSizer->Add(tempst,0, wxALIGN_CENTER_VERTICAL);
            textSizer->Add(temptext,0, wxALIGN_CENTER_VERTICAL | wxRIGHT, col_gap);

            ctrl_count++;
        }
	}

	parent_sizer->Add( chkSizer, 0, wxALL, chkSizer->GetChildren().size() > 0 ? 5 : 0 );
	parent_sizer->Add( spinSizer, 0, wxALL, spinSizer->GetChildren().size() > 0 ? 5 : 0 );
	parent_sizer->Add( cbxSizer, 0, wxALL, cbxSizer->GetChildren().size() > 0 ? 5 : 0 );
	parent_sizer->Add( textSizer, 0, wxALL, textSizer->GetChildren().size() > 0 ? 5 : 0 );

    return total_count;

}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnChkBoxChange(wxCommandEvent& event)
{
	wxCheckBox* box = (wxCheckBox*) event.GetEventObject();
	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption ;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);

	if( m_battle.CustomBattleOptions().setSingleOption( key , (box->GetValue() ? _T("1") : _T("0")) , (OptionsWrapper::GameOption)gameoption ) )
	{
        if (m_battle.IsFounderMe())
        {
          m_battle.SendHostInfo( (wxString()<<gameoption) + wxsep + key );
        }
	}
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnComBoxChange(wxCommandEvent& event)
{
	wxComboBox* box = (wxComboBox*) event.GetEventObject();

	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);
	wxString itemKey = m_battle.CustomBattleOptions().GetNameListOptItemKey(key,  box->GetValue(), (OptionsWrapper::GameOption)gameoption );

	if(m_battle.CustomBattleOptions().setSingleOption( key, itemKey, (OptionsWrapper::GameOption)gameoption ) )
	{
        if (m_battle.IsFounderMe())
        {
          m_battle.SendHostInfo( (wxString()<< gameoption) + wxsep + key );
        }
	}
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnTextCtrlChange(wxCommandEvent& event)
{
	wxTextCtrl* box = (wxTextCtrl*) event.GetEventObject();
	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);
	if(m_battle.CustomBattleOptions().setSingleOption( key, box->GetValue(), (OptionsWrapper::GameOption)gameoption ) )
	{
		if (m_battle.IsFounderMe())
		{
		  m_battle.SendHostInfo(  (wxString()<< gameoption) + wxsep + key );
		}

	}
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnSpinCtrlChange(wxSpinEvent& event)
{
	wxSpinCtrlDbl* box = (wxSpinCtrlDbl*) event.GetEventObject();
	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);
	if(m_battle.CustomBattleOptions().setSingleOption( key,wxString::Format( _T("%f"),box->GetValue() ), (OptionsWrapper::GameOption)gameoption ) )
	{
		if (m_battle.IsFounderMe())
		{
		  m_battle.SendHostInfo(  (wxString()<< gameoption) + wxsep + key );
		}
	}
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::UpdateOptControls(wxString controlName)
{
	long gameoption;
	controlName.BeforeFirst(sep).ToLong(&gameoption);
	wxString optKey = controlName.AfterFirst(sep);

	if ( gameoption == OptionsWrapper::PrivateOptions )
	{
    if ( optKey == _T("mapname") ) OnReloadControls( OptionsWrapper::MapOption );
    if ( optKey == _T("modname") ) OnReloadControls( OptionsWrapper::ModOption );
    return;
	}

	if ( m_chkbox_map.find(controlName) != m_chkbox_map.end() )
	{
	    wxString value = m_battle.CustomBattleOptions().getSingleValue( optKey, (OptionsWrapper::GameOption)gameoption );
		wxCheckBox* cur = m_chkbox_map[controlName] ;
		long l_val;
		value.ToLong(&l_val);
		cur->SetValue(l_val);
	}

	 if ( m_combox_map.find(controlName) != m_combox_map.end() )
	{
		wxString value = m_battle.CustomBattleOptions().getSingleValue( optKey, (OptionsWrapper::GameOption)gameoption );
		wxComboBox* cur = m_combox_map[controlName];
		cur->SetValue(m_battle.CustomBattleOptions().GetNameListOptValue( optKey, (OptionsWrapper::GameOption)gameoption));
	}

	 if ( m_textctrl_map.find(controlName) != m_textctrl_map.end() )
	{
		wxString value = m_battle.CustomBattleOptions().getSingleValue( optKey, (OptionsWrapper::GameOption)gameoption );
		wxTextCtrl* cur = m_textctrl_map[controlName];
		cur->SetValue(value);
	}

	 if ( m_spinctrl_map.find(controlName) != m_spinctrl_map.end() )
	{
		wxString value = m_battle.CustomBattleOptions().getSingleValue( optKey, (OptionsWrapper::GameOption)gameoption );
		wxSpinCtrlDbl* cur = m_spinctrl_map[controlName] ;
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
void BattleroomMMOptionsTab<BattleType>::OnReloadControls(OptionsWrapper::GameOption flag)
{
	wxString pref = wxString::Format( _T("%d"),flag) + wxsep;

	//purgin existing keys from map
	RemovePrefixed(m_chkbox_map,pref);
	RemovePrefixed(m_spinctrl_map,pref);
	RemovePrefixed(m_textctrl_map,pref);
	RemovePrefixed(m_combox_map,pref);
	RemovePrefixed(m_statictext_map,pref);

	//reloading the controls
	switch (flag)
	{
		case OptionsWrapper::ModOption:
			m_mod_options_sizer->Remove(m_mod_layout);
			m_mod_layout = new wxBoxSizer( wxVERTICAL);
			setupOptionsSizer(m_mod_layout,OptionsWrapper::ModOption);
			//m_mod_options_sizer->Add( m_mod_options_sizer, 1, wxEXPAND, 5 );
			m_mod_options_sizer->Add( m_mod_layout, 1, wxALL|wxEXPAND, 5 );
			break;
		case OptionsWrapper::MapOption:
			m_map_options_sizer->Remove(m_map_layout);
			m_map_layout = new wxBoxSizer( wxVERTICAL);
			setupOptionsSizer(m_map_layout,OptionsWrapper::MapOption);
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
    for ( unsigned int i = 0; i < OptionsWrapper::LastOption; i++)
        OnReloadControls( (OptionsWrapper::GameOption) i );
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnLoadPreset( wxCommandEvent& event )
{
  wxString presetname = m_options_preset_sel->GetValue();
  if ( presetname.IsEmpty() )
  {
     customMessageBoxNoModal( SL_MAIN_ICON , _("Cannot load an options set without a name\nPlease select one from the list and try again."), _("error"), wxICON_EXCLAMATION|wxOK );
     return;
  }
  m_battle.LoadOptionsPreset( presetname );
  m_battle.SendHostInfo( IBattle::HI_Send_All_opts );
  OnReloadControls( );
}


template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnSavePreset( wxCommandEvent& event )
{
  wxString presetname = m_options_preset_sel->GetValue();
  if ( presetname.IsEmpty() )
  {
     customMessageBoxNoModal( SL_MAIN_ICON , _("Cannot save an options set without a name\nPlease write one in the list or chose an existing to overwrite and try again."), _("error"), wxICON_EXCLAMATION|wxOK );
     return;
  }
  m_battle.SaveOptionsPreset( presetname );
}


template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnDeletePreset( wxCommandEvent& event )
{
  wxString presetname = m_options_preset_sel->GetValue();
  if ( presetname.IsEmpty() )
  {
     customMessageBoxNoModal( SL_MAIN_ICON , _("Cannot delete an options set without a name\nPlease select one from the list and try again."), _("error"), wxICON_EXCLAMATION|wxOK );
     return;
  }
  m_battle.DeletePreset( presetname );
}

template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::OnSetModDefaultPreset( wxCommandEvent& event )
{
  wxString presetname = m_options_preset_sel->GetValue();
  if ( presetname.IsEmpty() )
  {
     customMessageBoxNoModal( SL_MAIN_ICON , _("No options set is selected to set as default\nPlease select one from the list and try again."), _("error"), wxICON_EXCLAMATION|wxOK );
     return;
  }
  sett().SetModDefaultPresetName( m_battle.GetHostModName(), presetname );
}


template < class BattleType >
void BattleroomMMOptionsTab<BattleType>::UpdatePresetList()
{
    m_options_preset_sel->Clear();
    m_options_preset_sel->Append(sett().GetPresetList());
    m_options_preset_sel->SetStringSelection(  m_battle.GetCurrentPreset() );
}
