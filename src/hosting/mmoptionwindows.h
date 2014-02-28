/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef MMOPTIONWINDOWS_H_INCLUDED
#define MMOPTIONWINDOWS_H_INCLUDED

#include <wx/dialog.h>

class wxCheckBox;
class wxComboBox;
class wxCommandEvent;
class IBattle;
class SlSpinCtrlDouble;
class SlSpinDoubleEvent;
class wxTextCtrl;
class wxStaticText;
class wxButton;

class SingleOptionDialog: public wxDialog
{

	public:
			SingleOptionDialog( IBattle& battle, const wxString& optiontag );

			void OnOk(wxCommandEvent& event);
			void OnCancel(wxCommandEvent& event);

			//! dummy event receiver, mandated by the control implementation
			void OnSpinCtrlDoubleChange(SlSpinDoubleEvent& /*event*/){}

	protected:
			IBattle& m_battle;
			wxString m_tag;

			wxCheckBox* m_checkbox;
			wxComboBox* m_combobox;
			SlSpinCtrlDouble* m_spinctrl;
			wxTextCtrl* m_textctrl;
			wxButton* m_cancel_button;
			wxButton* m_ok_button;
};

enum
{
	ID_CANCEL = wxID_HIGHEST,
	ID_OK
};

#endif // MMOPTIONWINDOWS_H_INCLUDED
