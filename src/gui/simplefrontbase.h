///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __simplefrontbase__
#define __simplefrontbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class SimpleFrontBase
///////////////////////////////////////////////////////////////////////////////
class SimpleFrontBase : public wxFrame 
{
	private:
	
	protected:
		
		wxButton* m_sp;
		
		wxButton* m_mp;
		
		wxButton* m_settings;
		
		wxButton* m_exit;
		
		wxCheckBox* m_skip;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSingleplayer( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnMultiplayer( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSettings( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSkipChecked( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		SimpleFrontBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("LALALALAUNCHER"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 164,314 ), long style = wxCAPTION|wxCLOSE_BOX|wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~SimpleFrontBase();
	
};

#endif //__simplefrontbase__
