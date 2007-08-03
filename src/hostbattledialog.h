#ifndef SPRINGLOBBY_HEADERGUARD_HOSTBATTLEDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_HOSTBATTLEDIALOG_H


#include <wx/dialog.h>

class wxStaticText;
class wxTextCtrl;
class wxChoice;
class wxSlider;
class wxRadioBox;
class wxStaticLine;
class wxButton;

class HostBattleDialog : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			HOST_CANCEL = 1000,
			HOST_OK,
		};
		
		wxStaticText* m_desc_lbl;
		wxTextCtrl* m_desc_text;
		wxStaticText* m_mod_lbl;
		wxChoice* m_mod_pic;
		wxStaticText* m_pwd_lbl;
		wxTextCtrl* m_pwd_text;
		wxStaticText* m_port_lbl;
		wxTextCtrl* m_port_text;
		wxSlider* m_players_slide;
		wxRadioBox* m_nat_radios;
		wxStaticLine* m_buttons_sep;
		wxButton* m_cancel_btn;
		wxButton* m_host_btn;
	
	public:
		HostBattleDialog( wxWindow* parent, int id = wxID_ANY, wxString title = _("Host new battle"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 340,300 ), int style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif // SPRINGLOBBY_HEADERGUARD_HOSTBATTLEDIALOG_H