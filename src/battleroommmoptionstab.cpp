#include "battleroommmoptionstab.h"

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/choice.h>
#include <wx/stattext.h>
#include <wx/defs.h>
#include "mmoptionswrapper.h"

BattleroomMMOptionsTab::BattleroomMMOptionsTab( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_mapmodoptions = new mmOptionsWrapper();
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
	for (unsigned int i = 0; i < m_mapmodoptions->m_mod_options_float.size();++i)
	{
		m_mod_layout->Add(new wxStaticText(this,-1,m_mapmodoptions->m_mod_options_float[i].description),0,10);
		m_mod_layout->Add(new wxSlider(this,-1,
				int(m_mapmodoptions->m_mod_options_float[i].def*100),
				int(m_mapmodoptions->m_mod_options_float[i].min*100),
				int(m_mapmodoptions->m_mod_options_float[i].max*100),
				wxDefaultPosition,wxSize(200,-1),wxSL_HORIZONTAL|wxSL_TOP|wxSL_AUTOTICKS|wxSL_LABELS),200,10);
		
	}
	
	for (unsigned int i = 0; i < m_mapmodoptions->m_mod_options_bool.size();++i)
	{
		m_mod_layout->Add(new wxCheckBox(this,-1,m_mapmodoptions->m_mod_options_bool[i].description));
	}
	
	for (unsigned int i = 0; i < m_mapmodoptions->m_mod_options_list.size();++i)
	{
		m_mod_layout->Add(new wxStaticText(this,-1,m_mapmodoptions->m_mod_options_list[i].description),0,5);
		m_mod_layout->Add(new wxChoice(this,-1,wxDefaultPosition,wxDefaultSize,m_mapmodoptions->m_mod_options_list[i].cbx_choices));
	}
	
}


