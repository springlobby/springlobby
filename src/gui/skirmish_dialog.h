
#ifndef __skirmish_dialog__
#define __skirmish_dialog__

#include <wx/intl.h>

#include <wx/frame.h>
#include "../mmoptionswrapper.h"
#include "../singleplayerbattle.h"

class wxRadioBox;
class wxChoice;
class wxStaticText;
class wxButton;
class wxCheckBox;


///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class skirmish_dialog
///////////////////////////////////////////////////////////////////////////////
class SkirmishDialog : public wxFrame
{
	private:

	protected:
		wxRadioBox* m_radioBox1;
		wxStaticText* m_map_label;
		wxChoice* m_map;
		wxCheckBox* m_map_random;
		wxButton* m_back;
		wxButton* m_advanced;
		wxButton* m_start;

		OptionsWrapper m_mod_customs;
		const wxString m_modname;
		NoGuiSinglePlayerBattle m_battle;

		// Virtual event handlers, overide them in your derived class
		virtual void OnBack( wxCommandEvent& event );
		virtual void OnAdvanced( wxCommandEvent& event );
		virtual void OnStart( wxCommandEvent& event );
		virtual void OnRadioBox( wxCommandEvent& event );

        typedef std::map< wxString, OptionsWrapper >
            SkirmishMap;

        SkirmishMap m_skirmishes;

	public:
		SkirmishDialog( wxWindow* parent, const wxString& modname, OptionsWrapper mod_customs, wxWindowID id = wxID_ANY,
                        const wxString& title = _("Skirmish"), const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~SkirmishDialog();

};

#endif //__skirmish_dialog__
