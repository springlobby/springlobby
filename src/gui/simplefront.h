#ifndef SPRINGLOBBY_HEADERGUARD_SIMPLEFRONT_H
#define SPRINGLOBBY_HEADERGUARD_SIMPLEFRONT_H

class settings_frame;
class SkirmishDialog;
class wxBoxSizer;
class wxGradientButton;

#include <wx/frame.h>

//! substitute for MainWindow when SpringLobby runs in simple mode
class SimpleFront : public wxFrame
{
protected:
	// Handlers for SimpleFrontBase events.
	void OnSingleplayer( wxCommandEvent& event );
	void OnMultiplayer( wxCommandEvent& event );
	void OnSettings( wxCommandEvent& event );
	void OnHelp( wxCommandEvent& event );
	void OnExit( wxCommandEvent& event );
	void OnSkipChecked( wxCommandEvent& event );

	settings_frame* m_settings_frame;
	SkirmishDialog* m_skirmish;
	const wxString m_modname;

    wxBoxSizer* m_button_sizer;
    wxBoxSizer* m_skirmish_sizer;
    wxBoxSizer* m_sp_button_sizer;

    wxBoxSizer* bSizer1;

    wxGradientButton* m_back;
    wxGradientButton* m_advanced;
    wxGradientButton* m_start;

    wxGradientButton* m_sp;
    wxGradientButton* m_mp;
    wxGradientButton* m_settings;
    wxGradientButton* m_exit;
    wxGradientButton* m_help;

public:
	/** Constructor */
	SimpleFront( wxWindow* parent );
	~SimpleFront();

    void ShowSP( bool show );

    void Close();
};

#endif //SPRINGLOBBY_HEADERGUARD_SIMPLEFRONT_H
