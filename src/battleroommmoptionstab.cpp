#include "battleroommmoptionstab.h"

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/defs.h>
#include "mmoptionswrapper.h"
#include "spinctld.h"

BattleroomMMOptionsTab::BattleroomMMOptionsTab( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_mapmodoptions = new mmOptionsWrapper();
	if (!m_mapmodoptions->loadOptions(ModOption))
	{
		//TODO display error and call m_mapmodoptions->loadOptions(MapOption)
	}
	
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
}

void BattleroomMMOptionsTab::setupModOptionsSizer()
{
	for (optionMapBoolIter i = (*m_mapmodoptions->m_boolMaps[ModOption]).begin(); i != (*m_mapmodoptions->m_boolMaps[ModOption]).end();++i)
		{
			mmOptionBool current = i->second;
			wxCheckBox* temp = new wxCheckBox(this,-1,current.name);
			temp->SetToolTip(current.description);
			temp->SetValue(current.value);
			m_mod_layout->Add(temp);
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
	for ( optionMapFloatIter it = (*m_mapmodoptions->m_floatMaps[ModOption]).begin(); it != (*m_mapmodoptions->m_floatMaps[ModOption]).end(); ++it)	
	{
			mmOptionFloat current = it->second;
			wxBoxSizer* tempbox = new wxBoxSizer(wxHORIZONTAL);
			wxSpinCtrlDbl* tempchoice = new wxSpinCtrlDbl();
			tempchoice->Create(this, -1, _T(""),
					wxDefaultPosition, wxDefaultSize, 0, double(current.min), double(current.max),
														double(current.value),double(current.stepping) );
			tempchoice->SetToolTip(current.description);
			
			tempbox->Add(new wxStaticText(this,-1,current.name),0,5);
			tempbox->Add(tempchoice);
			m_mod_layout->Add(tempbox);
	}
	
	
//	for (optionMapFloatIter i = m_mapmodoptions->m_mod_options_float.begin(); i != m_mapmodoptions->m_mod_options_float.end();++i)
//	{
//		mmOptionFloat current = i->second;
//		m_mod_layout->Add(new wxStaticText(this,-1,current.description),0,10);
//		m_mod_layout->Add(new wxSlider(this,-1,
//				int(current.def*100),
//				int(current.min*100),
//				int(current.max*100),
//				wxDefaultPosition,wxSize(200,-1),wxSL_HORIZONTAL|wxSL_TOP|wxSL_AUTOTICKS|wxSL_LABELS),200,10);
//		
//	}
	
//	for (unsigned int i = 0; i < m_mapmodoptions->m_mod_options_bool.size();++i)
//	{
//		m_mod_layout->Add(new wxCheckBox(this,-1,m_mapmodoptions->m_mod_options_bool[i].description));
//	}
//	
	
	for ( optionMapListIter it = (*m_mapmodoptions->m_listMaps[ModOption]).begin(); it != (*m_mapmodoptions->m_listMaps[ModOption]).end(); ++it)	
	{
		mmOptionList current = it->second;
		wxBoxSizer* tempbox = new wxBoxSizer(wxHORIZONTAL);
		wxComboBox* tempchoice = new wxComboBox(this, -1, current.value, wxDefaultPosition, wxDefaultSize, current.cbx_choices);
		tempchoice->SetToolTip(current.description);
		
		tempbox->Add(new wxStaticText(this,-1,current.name),0,5);
		tempbox->Add(tempchoice);
		m_mod_layout->Add(tempbox);
	}
	
}


