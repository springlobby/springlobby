///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Jan 16 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ConnectWindowBase
///////////////////////////////////////////////////////////////////////////////
class ConnectWindowBase : public wxDialog
{
	private:

	protected:
		wxButton* m_loginreg_button;
		wxStaticText* m_server_label;
		wxComboBox* m_server_combo;
		wxStaticText* m_email_label;
		wxTextCtrl* m_email_text;
		wxStaticText* m_nickname_label;
		wxTextCtrl* m_nickname_text;
		wxStaticText* m_password1_label;
		wxTextCtrl* m_password1_hidden_text;
		wxTextCtrl* m_password1_plain_text;
		wxStaticText* m_password2_label;
		wxTextCtrl* m_password2_hidden_text;
		wxTextCtrl* m_password2_plain_text;
		wxCheckBox* m_password_show_check;
		wxCheckBox* m_remember_password_check;
		wxCheckBox* m_autoconnect_check;
		wxTextCtrl* m_note_text;
		wxButton* m_cancel_button;
		wxButton* m_ok_button;

		// Virtual event handlers, overide them in your derived class
		virtual void OnChangeMode( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnServerChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOk( wxCommandEvent& event ) { event.Skip(); }


	public:

		ConnectWindowBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Connect to lobby server"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ConnectWindowBase();

};

