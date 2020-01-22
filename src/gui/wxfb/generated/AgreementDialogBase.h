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
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AgreementDialogBase
///////////////////////////////////////////////////////////////////////////////
class AgreementDialogBase : public wxDialog
{
	private:

	protected:
		wxTextCtrl* m_text;
		wxStaticLine* m_verif_sep;
		wxStaticText* m_verif_lbl;
		wxTextCtrl* m_verif_text;
		wxStaticLine* m_staticline2;
		wxStaticText* m_accept_lbl;
		wxButton* m_no_btn;
		wxButton* m_yes_btn;

		// Virtual event handlers, overide them in your derived class
		virtual void OnNo( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnYes( wxCommandEvent& event ) { event.Skip(); }


	public:

		AgreementDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Terms of Service"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~AgreementDialogBase();

};

