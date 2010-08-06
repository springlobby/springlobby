#ifndef INTROGUIDE_H
#define INTROGUIDE_H

#include <wx/frame.h>
#include <wx/string.h>

class wxTextCtrl;
class wxToggleButton;
class wxCheckBox;
class wxTextUrlEvent;
class wxCommandEvent;

class IntroGuide : public wxFrame
{
public:
    IntroGuide();
	void OnSpringDesc( wxCommandEvent& event );
	void OnInstallDesc( wxCommandEvent& event );
	void OnSingleDesc( wxCommandEvent& event );
	void OnMultiDesc( wxCommandEvent& event );
	void OnGraphicTroubles( wxCommandEvent& event );
	void OnWikiLinks( wxCommandEvent& event );
	void OnLinkEvent( wxTextUrlEvent& event );
private:
	wxString SpringOverview;
	wxString InstallOverview;
	wxString SinglePlayer;
	wxString MultiPlayer;
	wxString GraphicProblems;
	wxString UsefulLinks;

	wxTextCtrl* m_text_stuff;
#if wxUSE_TOGGLEBTN
	wxToggleButton* SpringDescription;
	wxToggleButton* InstallingContent;
	wxToggleButton* SingleDescription;
	wxToggleButton* MultiDescription;
	wxToggleButton* GraphicTroubles;
	wxToggleButton* WikiLinks;
#else
	wxCheckBox* SpringDescription;
	wxCheckBox* InstallingContent;
	wxCheckBox* SingleDescription;
	wxCheckBox* MultiDescription;
	wxCheckBox* GraphicTroubles;
	wxCheckBox* WikiLinks;
#endif
	enum
	{
		TEXT_DISPLAY = wxID_HIGHEST,
		SPRING_DESC,
		MULTI_DESC,
		SINGLE_DESC,
		INSTALL_DESC,
		GRAPHIC_TROUB,
		HELP_LINKS
	};

	DECLARE_EVENT_TABLE();
};

#endif // INTROGUIDE_H
