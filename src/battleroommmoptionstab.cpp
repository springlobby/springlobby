#include "battleroommmoptionstab.h"

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/defs.h>
#include "mmoptionswrapper.h"
#include "ibattle.h"
#include "spinctld.h"
#include <map>
#include "settings++/custom_dialogs.h"
#include "server.h"

BEGIN_EVENT_TABLE( BattleroomMMOptionsTab,  wxPanel)
	EVT_COMBOBOX(wxID_ANY, BattleroomMMOptionsTab::OnComBoxChange)
	EVT_CHECKBOX(wxID_ANY, BattleroomMMOptionsTab::OnChkBoxChange)

END_EVENT_TABLE()

BattleroomMMOptionsTab::BattleroomMMOptionsTab(  IBattle& battle, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxPanel( parent, id, pos, size, style ),m_battle(battle)
{

	for ( GameOption g = 0; g < mmOptionsWrapper::optionCategoriesCount; g++ )
	{
		m_chkbox_vec[g] = new chkBoxVec;
		m_combox_vec[g] = new comboBoxVec;
	}
	m_mapmodoptions = battle.CustomBattleOptions();
	m_mapmodoptions->loadOptions(ModOption);
	m_main_sizer = new wxBoxSizer( wxVERTICAL );


	m_mod_options_sizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Mod Options") ), wxVERTICAL );


	m_mod_layout = new wxFlexGridSizer( 2, 2, 10, 10 );
	m_mod_layout->SetFlexibleDirection( wxBOTH );
	m_mod_layout->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_mod_options_sizer->Add( m_mod_layout, 1, wxEXPAND, 5 );

	setupModOptionsSizer();

	m_main_sizer->Add( m_mod_options_sizer, 1, wxEXPAND, 5 );


	//m_main_sizer->Add( 0, 5, 1, wxEXPAND, 5 );

	m_map_options_sizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Map Options") ), wxVERTICAL );


	m_map_layout = new wxFlexGridSizer( 2, 2, 0, 0 );
	m_map_layout->SetFlexibleDirection( wxBOTH );
	m_map_layout->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_map_options_sizer->Add( m_map_layout, 1, wxEXPAND, 5 );

	m_main_sizer->Add( m_map_options_sizer, 1, wxEXPAND, 5 );

	this->SetSizer( m_main_sizer );
	this->Layout();
}

BattleroomMMOptionsTab::~BattleroomMMOptionsTab()
{
	m_mapmodoptions->unLoadOptions();
}

// always set the controls name to the option key
void BattleroomMMOptionsTab::setupModOptionsSizer()
{
	mmOptionsWrapper* optWrap = m_mapmodoptions;
	bool enable = m_battle.IsFounderMe();
	int ctrl_count = 0;
	for (optionMapBoolIter i = optWrap->m_boolMaps[ModOption]->begin(); i != optWrap->m_boolMaps[ModOption]->end();++i)
		{
			mmOptionBool current = i->second;
			wxCheckBox* temp = new wxCheckBox(this,BOOL_START_ID+ctrl_count,current.name);
			temp->SetToolTip(current.description);
			temp->SetName(current.key);
			m_chkbox_vec[ModOption]->push_back(temp);
			temp->SetValue(current.value);
			temp->Enable(enable);
			m_mod_layout->Add(temp);
			ctrl_count++;
		}


/**** use iterators *****/

	/*  wxSpinCtrlDbl( wxWindow &parent, wxWindowID id,
	                   const wxString &value = wxEmptyString,
	                   const wxPoint& pos = wxDefaultPosition,
	                   const wxSize& size = wxSize(95,-1),
	                   long style = 0,
	                   double min = 0.0, double max = 100.0,
	                   double initial = 0.0,
	                   double increment = 1.0, int digits = wxSPINCTRLDBL_AUTODIGITS,
	                   const wxString& name = _T("wxSpinCtrlDbl") ) */
	ctrl_count = 0;
//	for ( optionMapFloatIter it = (*optWrap->m_floatMaps[ModOption]).begin(); it != (*optWrap->m_floatMaps[ModOption]).end(); ++it)
//	{
//			mmOptionFloat current = it->second;
//			wxBoxSizer* tempbox = new wxBoxSizer(wxHORIZONTAL);
//			wxSpinCtrlDbl* tempchoice = new wxSpinCtrlDbl();
//			tempchoice->Create(this, -1, _T(""),
//					wxDefaultPosition, wxDefaultSize, 0, double(current.min), double(current.max),
//														double(current.value),double(current.stepping) );
//			tempchoice->SetToolTip(current.description);
//
//			tempbox->Add(new wxStaticText(this,-1,current.name),0,5);
//			tempbox->Add(tempchoice);
//			m_mod_layout->Add(tempbox);
//			ctrl_count++;
//	}

	ctrl_count = 0;
	for ( optionMapListIter it = (*optWrap->m_listMaps[ModOption]).begin(); it != (*optWrap->m_listMaps[ModOption]).end(); ++it)
	{
		mmOptionList current = it->second;
		wxBoxSizer* tempbox = new wxBoxSizer(wxHORIZONTAL);
		wxComboBox* tempchoice = new wxComboBox(this, -1, current.value, wxDefaultPosition, wxDefaultSize, current.cbx_choices);
		tempchoice->SetToolTip(current.description);
		tempchoice->SetName(current.key);
		tempchoice->Enable(enable);
		m_combox_vec[ModOption]->push_back(tempchoice);
		tempbox->Add(new wxStaticText(this,-1,current.name),0,5);
		tempbox->Add(tempchoice);

		m_mod_layout->Add(tempbox);
		ctrl_count++;
	}

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
        if (m_battle.IsFounderMe())
          m_battle.SendHostInfo(HI_Options);
		}
	}

}

void BattleroomMMOptionsTab::OnComBoxChange(wxCommandEvent& event)
{

}

// might be slow as hell (if hell is slow :P )
void BattleroomMMOptionsTab::UpdateOptControls(/* wxstringpairvec* list **/)
{
	typedef std::map<wxString,wxString> wxStringMap;
	mmOptionsWrapper* optWrap = m_mapmodoptions;

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
	}





}
