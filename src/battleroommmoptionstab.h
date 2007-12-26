#ifndef BATTLEROOMMMOPTIONSTAB_H_
#define BATTLEROOMMMOPTIONSTAB_H_

#include <wx/panel.h>

class wxBoxSizer;
class wxStaticBoxSizer;
class wxFlexGridSizer;
class mmOptionsWrapper;

class BattleroomMMOptionsTab : public wxPanel 
{
	private:
	
	protected:
		wxBoxSizer* m_main_sizer;
		wxStaticBoxSizer* m_mod_options_sizer;
		wxFlexGridSizer* m_mod_layout;
		wxStaticBoxSizer* m_map_options_sizer;
		wxFlexGridSizer* m_map_layout;
		mmOptionsWrapper* m_mapmodoptions;
		
		void setupModOptionsSizer();
	
	public:
		BattleroomMMOptionsTab( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~BattleroomMMOptionsTab();
		void setupMapOptionsSizer();
		
	
};

#endif /*BATTLEROOMMMOPTIONSTAB_H_*/
