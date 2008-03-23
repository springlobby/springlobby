#ifndef AUTOBALANCEDIALOG_H
#define AUTOBALANCEDIALOG_H

//(*Headers(AutoBalanceDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/statline.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class AutoBalanceDialog: public wxDialog
{
	public:

		AutoBalanceDialog(wxWindow* parent);
		virtual ~AutoBalanceDialog();

		//(*Declarations(AutoBalanceDialog)
		wxButton* m_ok_button;
		wxCheckBox* m_strong_clans_choice;
		wxChoice* m_method_choice;
		wxStaticLine* m_separator1;
		wxCheckBox* m_clans_choice;
		wxStaticText* m_method_label;
		wxButton* m_cancel_button;
		//*)

	protected:

		//(*Identifiers(AutoBalanceDialog)
		static const long ID_STATICTEXT1;
		static const long ID_METHOD;
		static const long ID_CLANS;
		static const long ID_STRONG_CLANS;
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
