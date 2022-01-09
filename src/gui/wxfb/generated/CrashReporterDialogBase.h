///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
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
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CrashReporterDialogBase
///////////////////////////////////////////////////////////////////////////////
class CrashReporterDialogBase : public wxDialog
{
	private:

	protected:
		wxBoxSizer* m_main_sizer;
		wxTextCtrl* m_report_text;
		wxStaticText* m_report_file_name_label;
		wxTextCtrl* m_report_file_text;
		wxCheckBox* m_log_upload_check;
		wxStaticLine* m_staticline1;
		wxTextCtrl* m_log_comment_text;
		wxButton* m_bug_button;
		wxButton* m_cancel_button;
		wxButton* m_safe_button;
		wxButton* m_norm_button;

		// Virtual event handlers, override them in your derived class
		virtual void OnClickBugReport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSafe( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNorm( wxCommandEvent& event ) { event.Skip(); }


	public:

		CrashReporterDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

		~CrashReporterDialogBase();

};

