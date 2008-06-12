
#include "battleroommmoptionstab.h"

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/defs.h>
#include <wx/intl.h>
#include <map>
#include <stdexcept>

#include "utils.h"
#include "mmoptionswrapper.h"
#include "ibattle.h"
#include "spinctld.h"
#include "settings++/custom_dialogs.h"
#include "server.h"

const char sep = *("_");
const wxString wxsep = _T("_");

BEGIN_EVENT_TABLE( BattleroomMMOptionsTab,  wxPanel)
	EVT_COMBOBOX					(wxID_ANY, BattleroomMMOptionsTab::OnComBoxChange)
	EVT_CHECKBOX					(wxID_ANY, BattleroomMMOptionsTab::OnChkBoxChange)
	EVT_TEXT_ENTER					(wxID_ANY,  BattleroomMMOptionsTab::OnTextCtrlChange)
	EVT_SPINCTRL					(wxID_ANY,  BattleroomMMOptionsTab::OnSpinCtrlChange)

END_EVENT_TABLE()

BattleroomMMOptionsTab::BattleroomMMOptionsTab(  IBattle& battle, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxScrolledWindow( parent, id, pos, size, style | wxHSCROLL ),m_battle(battle)
{
	m_main_sizer = new wxBoxSizer( wxVERTICAL );

	m_mod_options_sizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Mod Options") ), wxVERTICAL );
	m_mod_layout = new wxBoxSizer( wxVERTICAL);
	setupOptionsSizer(m_mod_layout,ModOption);
	m_mod_options_sizer->Add( m_mod_layout, 1, wxEXPAND, 5 );

	m_map_options_sizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Map Options") ), wxVERTICAL );
	m_map_layout = new wxBoxSizer( wxVERTICAL);
	setupOptionsSizer(m_map_layout,MapOption);
	m_map_options_sizer->Add( m_map_layout, 1, wxEXPAND, 5 );

	m_main_sizer->Add( m_mod_options_sizer, 1, wxEXPAND, 5 );
	m_main_sizer->Add( m_map_options_sizer, 1, wxEXPAND, 5 );

    SetScrollRate( 3, 3 );
	this->SetSizer( m_main_sizer );
	this->Layout();
}

BattleroomMMOptionsTab::~BattleroomMMOptionsTab()
{

}

// always set the controls name to the option key
void BattleroomMMOptionsTab::setupOptionsSizer(wxBoxSizer* optFlagSizer,GameOption optFlag)
{
	wxString pref = wxString::Format( _T("%d"),optFlag) + wxsep;
	mmOptionsWrapper optWrap = *m_battle.CustomBattleOptions();
	bool enable = m_battle.IsFounderMe();
	wxFlexGridSizer* cbxSizer =  new wxFlexGridSizer( 2, 2, 10, 10 );
	wxFlexGridSizer* spinSizer =  new wxFlexGridSizer( 2, 2, 10, 10 );
	wxFlexGridSizer* textSizer =  new wxFlexGridSizer( 2, 2, 10, 10 );
	wxFlexGridSizer* chkSizer = new wxFlexGridSizer( 2, 2, 10, 10 );

	int ctrl_count = 0;
	for (optionMapBoolIter i = optWrap.opts[optFlag].bool_map.begin(); i != optWrap.opts[optFlag].bool_map.end();++i)
		{
			mmOptionBool current = i->second;
			wxCheckBox* temp = new wxCheckBox(this,BOOL_START_ID+ctrl_count,current.name);
			temp->SetToolTip(current.description);
			temp->SetName(pref+current.key);
			m_chkbox_map[pref+current.key] = temp;
			temp->SetValue(current.value);
			temp->Enable(enable);
			chkSizer->Add(temp);
			ctrl_count++;
		}

	ctrl_count = 0;
	for ( optionMapFloatIter it = optWrap.opts[optFlag].float_map.begin(); it != optWrap.opts[optFlag].float_map.end(); ++it)
	{
			mmOptionFloat current = it->second;
			wxBoxSizer* tempbox = new wxBoxSizer(wxHORIZONTAL);
			wxSpinCtrlDbl* tempspin = new wxSpinCtrlDbl();
			tempspin->Create(this, FLOAT_START_ID+ctrl_count, _T(""),
					wxDefaultPosition, wxDefaultSize, 0, double(current.min), double(current.max),
					double(current.value),double(current.stepping), wxSPINCTRLDBL_AUTODIGITS, current.key);
			tempspin->SetToolTip(current.description);
			tempspin->Enable(enable);
			tempspin->SetName(pref+current.key);
			m_spinctrl_map[pref+current.key] = tempspin;
			 wxStaticText* tempst = new wxStaticText(this,-1,current.name);
			 m_statictext_map[pref+current.key] = tempst;
			tempbox->Add(tempst,0,5 );
			tempbox->Add(tempspin);
			spinSizer->Add(tempbox);
			ctrl_count++;
	}

	ctrl_count = 0;
	for ( optionMapListIter it = optWrap.opts[optFlag].list_map.begin(); it != optWrap.opts[optFlag].list_map.end(); ++it)
	{
		mmOptionList current = it->second;
		wxBoxSizer* tempbox = new wxBoxSizer(wxHORIZONTAL);


		int index = CLAMP(current.cur_choice_index,0,int(current.cbx_choices.GetCount()-1));
		wxComboBox* tempchoice = new wxComboBox(this, LIST_START_ID+ctrl_count, current.cbx_choices[index], wxDefaultPosition,
				wxDefaultSize, current.cbx_choices, 0, wxDefaultValidator);

		tempchoice->SetToolTip(current.description);
		tempchoice->SetName(pref+current.key);
		tempchoice->Enable(enable);
		m_combox_map[pref+current.key] = tempchoice;
		wxStaticText* tempst = new wxStaticText(this,-1,current.name);
		m_statictext_map[pref+current.key] = tempst;
		tempbox->Add(tempst,0,5);
		tempbox->Add(tempchoice);

		cbxSizer->Add(tempbox);
		ctrl_count++;
	}

	ctrl_count = 0;
	for ( optionMapStringIter it = optWrap.opts[optFlag].string_map.begin(); it != optWrap.opts[optFlag].string_map.end(); ++it)
	{
		mmOptionString current = it->second;
		wxBoxSizer* tempbox = new wxBoxSizer(wxHORIZONTAL);
		wxTextCtrl* temptext = new wxTextCtrl(this, STRING_START_ID+ctrl_count, current.value, wxDefaultPosition,
				wxDefaultSize, 0, wxDefaultValidator, current.key);
		temptext->SetToolTip(current.description);
		temptext->SetName(pref+current.key);
		temptext->Enable(enable);
		m_textctrl_map[pref+current.key] = temptext;
		wxStaticText* tempst = new wxStaticText(this,-1,current.name);
		m_statictext_map[pref+current.key] = tempst;
		tempbox->Add(tempst,0,5);
		tempbox->Add(temptext);

		textSizer->Add(tempbox);
		ctrl_count++;
	}

	optFlagSizer->Add(chkSizer,0,wxALL,10);
	optFlagSizer->Add(spinSizer,0,wxALL,10);
	optFlagSizer->Add(cbxSizer,0,wxALL,10);
	optFlagSizer->Add(textSizer,0,wxALL,10);


}

void BattleroomMMOptionsTab::OnChkBoxChange(wxCommandEvent& event)
{
	mmOptionsWrapper* optWrap = m_battle.CustomBattleOptions();
	wxCheckBox* box = (wxCheckBox*) event.GetEventObject();
	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption ;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);

	if( optWrap->setSingleOption( key , (box->GetValue() ? _T("1") : _T("0")) , (GameOption)gameoption ) )
	{
        if (m_battle.IsFounderMe())
        {
          m_battle.SendHostInfo( (wxString()<<gameoption) + wxsep + key );
        }
	}
}

void BattleroomMMOptionsTab::OnComBoxChange(wxCommandEvent& event)
{
	mmOptionsWrapper* optWrap = m_battle.CustomBattleOptions();
	wxComboBox* box = (wxComboBox*) event.GetEventObject();

	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);
	wxString itemKey = optWrap->GetNameListOptItemKey(key,  box->GetValue(), (GameOption)gameoption );

	if(optWrap->setSingleOption( key, itemKey, (GameOption)gameoption ) )
	{
        if (m_battle.IsFounderMe())
        {
          m_battle.SendHostInfo( (wxString()<< gameoption) + wxsep + key );
        }
	}
}

void BattleroomMMOptionsTab::OnTextCtrlChange(wxCommandEvent& event)
{
	mmOptionsWrapper* optWrap = m_battle.CustomBattleOptions();
	wxTextCtrl* box = (wxTextCtrl*) event.GetEventObject();
	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);
	if(optWrap->setSingleOption( key, box->GetValue(), (GameOption)gameoption ) )
	{
		if (m_battle.IsFounderMe())
		{
		  m_battle.SendHostInfo(  (wxString()<< gameoption) + wxsep + key );
		}

	}
}

void BattleroomMMOptionsTab::OnSpinCtrlChange(wxSpinEvent& event)
{
	mmOptionsWrapper* optWrap = m_battle.CustomBattleOptions();
	wxSpinCtrlDbl* box = (wxSpinCtrlDbl*) event.GetEventObject();
	wxString key = (box->GetName()).AfterFirst(sep);
	long gameoption;
	box->GetName().BeforeFirst(sep).ToLong(&gameoption);
	if(optWrap->setSingleOption( key,wxString::Format( _T("%f"),box->GetValue() ), (GameOption)gameoption ) )
	{
		if (m_battle.IsFounderMe())
		{
		  m_battle.SendHostInfo(  (wxString()<< gameoption) + wxsep + key );
		}
	}
}

void BattleroomMMOptionsTab::UpdateOptControls(wxString controlName)
{
	mmOptionsWrapper* optWrap = m_battle.CustomBattleOptions();
	long gameoption;
	controlName.BeforeFirst(sep).ToLong(&gameoption);
	wxString optKey = controlName.AfterFirst(sep);
	wxString value = optWrap->getSingleValue( optKey, (GameOption)gameoption );
	if ( m_chkbox_map.find(controlName) != m_chkbox_map.end() )
	{
		wxCheckBox* cur = m_chkbox_map[controlName] ;
		long l_val;
		value.ToLong(&l_val);
		cur->SetValue(l_val);
	}

	 if ( m_combox_map.find(controlName) != m_combox_map.end() )
	{
		wxComboBox* cur = m_combox_map[controlName];
		cur->SetValue(optWrap->GetNameListOptValue( optKey, (GameOption)gameoption));
	}

	 if ( m_textctrl_map.find(controlName) != m_textctrl_map.end() )
	{
		wxTextCtrl* cur = m_textctrl_map[controlName];
		cur->SetValue(value);
	}

	 if ( m_spinctrl_map.find(controlName) != m_spinctrl_map.end() )
	{
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

void BattleroomMMOptionsTab::OnRefreshControls(GameOption flag)
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
		case ModOption:
			m_mod_options_sizer->Remove(m_mod_layout);
			m_mod_layout = new wxBoxSizer( wxVERTICAL);
			setupOptionsSizer(m_mod_layout,ModOption);
			//m_mod_options_sizer->Add( m_mod_options_sizer, 1, wxEXPAND, 5 );
			m_mod_options_sizer->Add( m_mod_layout, 1, wxEXPAND, 5 );
			break;
		case MapOption:
			m_map_options_sizer->Remove(m_map_layout);
			m_map_layout = new wxBoxSizer( wxVERTICAL);
			setupOptionsSizer(m_map_layout,MapOption);
			m_map_options_sizer->Add( m_map_layout, 1, wxEXPAND, 5 );
			break;
        default:
            break;
	}


	//this->SetSizer( m_main_sizer, true );
	this->Layout();

}
