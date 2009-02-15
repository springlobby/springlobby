#ifndef MMOPTIONWINDOWS_H_INCLUDED
#define MMOPTIONWINDOWS_H_INCLUDED

#include <wx/dialog.h>

class wxCheckBox;
class wxComboBox;
class wxCommandEvent;
class Battle;
class wxSpinCtrlDbl;
class wxTextCtrl;
class wxStaticText;
class wxButton;

class SingleOptionDialog: public wxDialog
{

	public:
			SingleOptionDialog( Battle& battle, const wxString& optiontag );

			void OnOk(wxCommandEvent& event);
			void OnCancel(wxCommandEvent& event);

	protected:
			Battle& m_battle;
			wxString m_tag;

			wxCheckBox* m_checkbox;
			wxComboBox* m_combobox;
			wxSpinCtrlDbl* m_spinctrl;
			wxTextCtrl* m_textctrl;
			wxButton* m_cancel_button;
			wxButton* m_ok_button;

			DECLARE_EVENT_TABLE();
};

enum
{
	ID_CANCEL = wxID_HIGHEST,
	ID_OK
};

#endif // MMOPTIONWINDOWS_H_INCLUDED
