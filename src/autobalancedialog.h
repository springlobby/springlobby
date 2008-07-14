#ifndef AUTOBALANCEDIALOG_H
#define AUTOBALANCEDIALOG_H

//(*Headers(AutoBalanceDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

/** \brief lets host automatically assign players to teams, optionally taking clan/rank into account
 * dialog sets balancing options in Settings handler for the calling class to use after modal display ends
 * \todo use this in a non-modal way? */
class AutoBalanceDialog: public wxDialog
{
	public:

		AutoBalanceDialog(wxWindow* parent);
		virtual ~AutoBalanceDialog();

		//(*Declarations(AutoBalanceDialog)
		wxButton* m_ok_button;
		wxChoice* m_method_choice;
		wxStaticLine* m_separator1;
		wxStaticText* StaticText1;
		wxChoice* m_clans_choice;
		wxStaticText* m_method_label;
		wxButton* m_cancel_button;
		//*)

	protected:

		//(*Identifiers(AutoBalanceDialog)
		static const long ID_STATICTEXT1;
		static const long ID_METHOD;
		static const long ID_STATICTEXT2;
		static const long ID_CHOICE1;
		static const long ID_STATICLINE1;
		static const long ID_CANCEL;
		static const long ID_OK;
		//*)

	private:

		//(*Handlers(AutoBalanceDialog)
		void OnCancel(wxCommandEvent& event);
		void OnOk(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
