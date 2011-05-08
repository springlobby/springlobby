#ifndef SPRINGLOBBY_HEADERGUARD_SKIRMISHDIALOG
#define SPRINGLOBBY_HEADERGUARD_SKIRMISHDIALOG

#include <wx/string.h>

#include "gradientpanel.h"
#include "../mmoptionswrapper.h"
#include "../singleplayerbattle.h"
#include "qt/noguisingleplayerbattle.h"

class wxRadioBox;
class wxChoice;
class wxStaticText;
class wxButton;
class wxCheckBox;
class wxComboBox;
class SimpleFront;


///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class skirmish_dialog
///////////////////////////////////////////////////////////////////////////////
class SkirmishDialog : public wxGradientPanel
{
	private:

	protected:
		wxChoice* m_scenario_choice;
		wxStaticText* m_map_label;
		wxStaticText* m_sides_label;
		wxChoice* m_map;
		wxChoice* m_sides;
		wxCheckBox* m_map_random;

		SimpleFront* m_parent;
        OptionsWrapper m_mod_customs;

		NoGuiSinglePlayerBattle m_battle;

        typedef std::map< wxString, OptionsWrapper >
            SkirmishMap;

        SkirmishMap m_skirmishes;

	public:
		SkirmishDialog( SimpleFront* parent, wxWindowID id = wxID_ANY );
		~SkirmishDialog();

        virtual void OnBack( wxCommandEvent& event );
		virtual void OnAdvanced( wxCommandEvent& event );
		virtual void OnStart( wxCommandEvent& event );
		virtual void OnScenarioChoice( wxCommandEvent& event );
		virtual void OnRandom( wxCommandEvent& event );

};

#endif //SPRINGLOBBY_HEADERGUARD_SKIRMISHDIALOG
