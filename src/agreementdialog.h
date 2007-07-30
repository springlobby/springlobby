#ifndef SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H

class wxTextCtrl;
class wxStaticText;
class wxStaticLine;
class wxButton;

class AgreementDialog : public wxDialog 
{
	public:
		AgreementDialog( wxWindow* parent, const wxString& agreement );
	
    void OnYes( wxCommandEvent& event );
    void OnNo( wxCommandEvent& event );

	protected:
		wxTextCtrl* m_text;
		wxStaticText* m_accept_lbl;
		wxStaticLine* m_button_sep;
		wxButton* m_yes_btn;
		wxButton* m_no_btn;
	
    DECLARE_EVENT_TABLE()

};

enum
{
    AGREE_YES = wxID_HIGHEST,
    AGREE_NO,
};

#endif // SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H