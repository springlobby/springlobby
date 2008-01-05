#include "battleroommmoptionstab.h"

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/defs.h>
#include "utils.h"
#include "mmoptionswrapper.h"
#include "ibattle.h"
#include "spinctld.h"
#include <map>
#include "settings++/custom_dialogs.h"
#include "server.h"
#include <stdexcept>

BEGIN_EVENT_TABLE( BattleroomMMOptionsTab,  wxPanel)
	EVT_COMBOBOX					(wxID_ANY, BattleroomMMOptionsTab::OnComBoxChange)
	EVT_CHECKBOX					(wxID_ANY, BattleroomMMOptionsTab::OnChkBoxChange)
	EVT_TEXT_ENTER					(wxID_ANY,  BattleroomMMOptionsTab::OnTextCtrlChange)
	EVT_SPINCTRL					(wxID_ANY,  BattleroomMMOptionsTab::OnSpinCtrlChange)

END_EVENT_TABLE()

BattleroomMMOptionsTab::BattleroomMMOptionsTab(  IBattle& battle, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxScrolledWindow( parent, id, pos, size, style | wxHSCROLL ),m_battle(battle)
{

	for ( GameOption g = 0; g < mmOptionsWrapper::optionCategoriesCount; g++ )
	{
		m_chkbox_vec[g] = new chkBoxVec;
		m_combox_vec[g] = new comboBoxVec;
		m_spinctrl_vec[g] = new spinCtrlVec;
		m_textctrl_vec[g] = new textCtrlVec;
	}

	battle.CustomBattleOptions()->loadOptions(ModOption);
	//ASSERT_LOGIC( (battle.GetMapName()!= _T("")), _T("no mapname") );
	//battle.CustomBattleOptions()->loadOptions(MapOption,battle.GetMapName());

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

	this->SetSizer( m_main_sizer );
	this->Layout();
}

BattleroomMMOptionsTab::~BattleroomMMOptionsTab()
{

}

// always set the controls name to the option key
void BattleroomMMOptionsTab::setupOptionsSizer(wxBoxSizer* optFlagSizer,GameOption optFlag)
{
	mmOptionsWrapper* optWrap = m_battle.CustomBattleOptions();
	bool enable = m_battle.IsFounderMe();
	wxFlexGridSizer* cbxSizer =  new wxFlexGridSizer( 2, 2, 10, 10 );
	wxFlexGridSizer* spinSizer =  new wxFlexGridSizer( 2, 2, 10, 10 );
	wxFlexGridSizer* textSizer =  new wxFlexGridSizer( 2, 2, 10, 10 );
	wxFlexGridSizer* chkSizer = new wxFlexGridSizer( 2, 2, 10, 10 );

	int ctrl_count = 0;
	for (optionMapBoolIter i = optWrap->m_boolMaps[optFlag]->begin(); i != optWrap->m_boolMaps[optFlag]->end();++i)
		{
			mmOptionBool current = i->second;
			wxCheckBox* temp = new wxCheckBox(this,BOOL_START_ID+ctrl_count,current.name);
			temp->SetToolTip(current.description);
			temp->SetName(current.key);
			m_chkbox_vec[optFlag]->push_back(temp);
			temp->SetValue(current.value);
			temp->Enable(enable);
			chkSizer->Add(temp);
			ctrl_count++;
		}

	ctrl_count = 0;
	for ( optionMapFloatIter it = (*optWrap->m_floatMaps[optFlag]).begin(); it != (*optWrap->m_floatMaps[optFlag]).end(); ++it)
	{
			mmOptionFloat current = it->second;
			wxBoxSizer* tempbox = new wxBoxSizer(wxHORIZONTAL);
			wxSpinCtrlDbl* tempspin = new wxSpinCtrlDbl();
			tempspin->Create(this, FLOAT_START_ID+ctrl_count, _T(""),
					wxDefaultPosition, wxDefaultSize, 0, double(current.min), double(current.max),
					double(current.value),double(current.stepping), wxSPINCTRLDBL_AUTODIGITS, current.key);
			tempspin->SetToolTip(current.description);
			m_spinctrl_vec[optFlag]->push_back(tempspin);
			tempbox->Add(new wxStaticText(this,-1,current.name),0,5);
			tempbox->Add(tempspin);
			spinSizer->Add(tempbox);
			ctrl_count++;
	}

	ctrl_count = 0;
	for ( optionMapListIter it = (*optWrap->m_listMaps[optFlag]).begin(); it != (*optWrap->m_listMaps[optFlag]).end(); ++it)
	{
		mmOptionList current = it->second;
		wxBoxSizer* tempbox = new wxBoxSizer(wxHORIZONTAL);
		wxComboBox* tempchoice = new wxComboBox(this, LIST_START_ID+ctrl_count, current.def, wxDefaultPosition,
				wxDefaultSize, current.cbx_choices, 0, wxDefaultValidator, current.key);
		tempchoice->SetToolTip(current.description);
		tempchoice->SetName(current.key);
		tempchoice->Enable(enable);
		m_combox_vec[optFlag]->push_back(tempchoice);
		tempbox->Add(new wxStaticText(this,-1,current.name),0,5);
		tempbox->Add(tempchoice);

		cbxSizer->Add(tempbox);
		ctrl_count++;
	}

	ctrl_count = 0;
	for ( optionMapStringIter it = (*optWrap->m_stringMaps[optFlag]).begin(); it != (*optWrap->m_stringMaps[optFlag]).end(); ++it)
	{
		mmOptionString current = it->second;
		wxBoxSizer* tempbox = new wxBoxSizer(wxHORIZONTAL);
		wxTextCtrl* temptext = new wxTextCtrl(this, STRING_START_ID+ctrl_count, current.value, wxDefaultPosition,
				wxDefaultSize, 0, wxDefaultValidator, current.key);
		temptext->SetToolTip(current.description);
		temptext->SetName(current.key);
		temptext->Enable(enable);
		m_textctrl_vec[optFlag]->push_back(temptext);
		tempbox->Add(new wxStaticText(this,-1,current.name),0,5);
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
	int event_id = event.GetId();
	for ( GameOption g = 0; g < mmOptionsWrapper::optionCategoriesCount; g++ )
	{
		if (event_id < BOOL_START_ID || event_id > int(BOOL_START_ID +m_chkbox_vec[g]->size()))
			return;
		else
		{
			wxCheckBox* box = (wxCheckBox*) event.GetEventObject();

			if(optWrap->setSingleOption( box->GetName(),wxString::Format( _T("%d"),box->GetValue() ) ))
			{
		        if (m_battle.IsFounderMe())
		          m_battle.SendHostInfo(HI_Options);
			}

		}
	}

}

void BattleroomMMOptionsTab::OnComBoxChange(wxCommandEvent& event)
{
	mmOptionsWrapper* optWrap = m_battle.CustomBattleOptions();
		int event_id = event.GetId();
		for ( GameOption g = 0; g < mmOptionsWrapper::optionCategoriesCount; g++ )
		{
			if (event_id < LIST_START_ID|| event_id > int( LIST_START_ID+m_combox_vec[g]->size()))
				return;
			else
			{
				wxComboBox* box = (wxComboBox*) event.GetEventObject();

				if(optWrap->setSingleOption( box->GetName(), box->GetValue() ) )
				{
			        if (m_battle.IsFounderMe())
			          m_battle.SendHostInfo(HI_Options);
			        //you can't set more than one option at once, can you?
			        break;
				}

			}
		}
}

void BattleroomMMOptionsTab::OnTextCtrlChange(wxCommandEvent& event)
{
	mmOptionsWrapper* optWrap = m_battle.CustomBattleOptions();
		int event_id = event.GetId();
		for ( GameOption g = 0; g < mmOptionsWrapper::optionCategoriesCount; g++ )
		{
			if (event_id < STRING_START_ID|| STRING_START_ID > int( STRING_START_ID + m_textctrl_vec[g]->size()))
				return;
			else
			{
				wxTextCtrl* box = (wxTextCtrl*) event.GetEventObject();

				if(optWrap->setSingleOption( box->GetName(), box->GetValue() ) )
				{
			        if (m_battle.IsFounderMe())
			          m_battle.SendHostInfo(HI_Options);
			        //you can't set more than one option at once, can you?
			        break;
				}

			}
		}
}

void BattleroomMMOptionsTab::OnSpinCtrlChange(wxSpinEvent& event)
{
	mmOptionsWrapper* optWrap = m_battle.CustomBattleOptions();
		int event_id = event.GetId();
		for ( GameOption g = 0; g < mmOptionsWrapper::optionCategoriesCount; g++ )
		{
			if (event_id < FLOAT_START_ID|| event_id > int(FLOAT_START_ID +m_spinctrl_vec[g]->size()))
				return;
			else
			{
				wxSpinCtrlDbl* box = (wxSpinCtrlDbl*) event.GetEventObject();

				if(optWrap->setSingleOption( box->GetName(),wxString::Format( _T("%f"),box->GetValue() ) ))
				{
			        if (m_battle.IsFounderMe())
			          m_battle.SendHostInfo(HI_Options);
			        //you can't set more than one option at once, can you?
			        break;
				}

			}
		}
}

// might be slow as hell (if hell is slow :P )
void BattleroomMMOptionsTab::UpdateOptControls(/* wxstringpairvec* list **/)
{
	typedef std::map<wxString,wxString> wxStringMap;
	mmOptionsWrapper* optWrap = m_battle.CustomBattleOptions();

	for ( GameOption g = 0; g < mmOptionsWrapper::optionCategoriesCount; g++ )
	{
		wxStringMap* opt_map = new wxStringMap;
		optWrap->getOptionsMap(opt_map,g);

		for(chkBoxVec::iterator it = m_chkbox_vec[g]->begin(); it != m_chkbox_vec[g]->end(); ++it)
		{
			wxCheckBox* cur = (*it);
			wxString value = (*opt_map)[cur->GetName()];
			long* l_val = new long;
			value.ToLong(l_val);
			cur->SetValue(l_val);

		}

		for(comboBoxVec::iterator it = m_combox_vec[g]->begin(); it != m_combox_vec[g]->end(); ++it)
		{
			wxComboBox* cur = (*it);
			wxString value = (*opt_map)[cur->GetName()];

			cur->SetValue(value);

		}

		for(textCtrlVec::iterator it = m_textctrl_vec[g]->begin(); it != m_textctrl_vec[g]->end(); ++it)
		{
			wxTextCtrl* cur = (*it);
			wxString value = (*opt_map)[cur->GetName()];

			cur->SetValue(value);

		}

		for(spinCtrlVec::iterator it = m_spinctrl_vec[g]->begin(); it != m_spinctrl_vec[g]->end(); ++it)
		{
			wxSpinCtrlDbl* cur = (*it);
			wxString value = (*opt_map)[cur->GetName()];
			double* l_val = new double;
			value.ToDouble(l_val);
			cur->SetValue(*l_val);

		}
	}
}

